#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <unistd.h>

#include "socketutils.hpp"

using namespace std;

int connect_to_player(int socket_ringmaster){
    int status;
    char prev_hostname[512];
    unsigned short int prev_port;
    char prev_port_name[9];
    status = recv(socket_ringmaster, &prev_port, sizeof(prev_port), 0);
    if(status == 0){
        cerr<<"Error: connection is cloesd"<<endl;
        exit(EXIT_FAILURE);
    }
    errorHandle(status,"Error: recieve failed",NULL, NULL);
    status = recv(socket_ringmaster, prev_hostname, 32, 0);
    if(status == 0){
        cerr<<"Error: connection is cloesd"<<endl;
        exit(EXIT_FAILURE);
    }
    errorHandle(status,"Error: recieve failed",NULL, NULL);
    prev_hostname[32] = 0;
    sprintf(prev_port_name, "%u", prev_port);
    // cout<<"Recieve hostname "<< prev_hostname <<" recieve port number " <<prev_port_name<<endl;
    return buildClient(prev_hostname, prev_port_name);
}

int main(int argc, char *argv[])
{
    if (argc < 3) {
        cout << "Syntax: player <machine_name> <port_num>\n" << endl;
        return 1;
    }

    int status;
    const char *hostname = argv[1];
    const char *port = argv[2];
    int socket_ringmaster, socket_prev, socket_next;

    //connect to ringmaster
    socket_ringmaster = buildClient(hostname, port);

    //recieve player id
    int info[2];
    status = recv(socket_ringmaster, &info, sizeof(info), 0);
    if(status == 0){
        cerr<<"Error: connection is cloesd"<<endl;
        exit(EXIT_FAILURE);
    }
    errorHandle(status,"Error: recieve failed",NULL, NULL);
    cout << "Connected as player "<< info[0]<<" out of " << info[1] <<" total players" << endl;

    //build Server
    socket_next = buildServer("");

    //get port number
    struct sockaddr_storage socket_addr;
    socklen_t socket_addr_len = sizeof(socket_addr);
    status = getsockname(socket_next,(struct sockaddr *)&socket_addr, &socket_addr_len);
    errorHandle(status,"Error: cannot get port info from socket", NULL, NULL);
    unsigned short int port_listen = ntohs(((struct sockaddr_in *) &socket_addr)->sin_port);
    // cout << "Send port number " << port_listen << endl;

    //send port info，ip info to ringmaster
    char name[512];
    gethostname(name,sizeof(name));
    // cout<<"current host name "<<name<<endl;
    status = send(socket_ringmaster, name, sizeof(name), 0);
    errorHandle(status,"Error: send failed",NULL, NULL);
    status = send(socket_ringmaster, &port_listen, sizeof(port_listen), 0);
    errorHandle(status,"Error: send failed",NULL, NULL);

    //connect to the previous player
    char prev_hostname[512];
    unsigned short int prev_port;
    char prev_port_name[9];

    if(info[0]> 0){
        socket_prev = connect_to_player(socket_ringmaster);
    }
    
    // cout<<"waiting for connection "<<endl;
    //connect with the next player
    struct sockaddr_storage socket_addr_next;
    socklen_t socket_addr_len_next = sizeof(socket_addr_next);
    socket_next = accept(socket_next, (struct sockaddr *)&socket_addr_next, &socket_addr_len_next);
    // cout << "connect to" <<ntohs(((struct sockaddr_in *)&socket_addr_next)->sin_port)<<endl;
    errorHandle(status,"Error: cannot accept", hostname, NULL);

    if(info[0] == 0){
        socket_prev = connect_to_player(socket_ringmaster);
    }
    // cout<<"Recieve hostname "<< prev_hostname <<" recieve port number " <<prev_port_name <<endl;  

    //Player ready
    char message = 'R';
    status = send(socket_ringmaster, &message, sizeof(message), 0);
    errorHandle(status,"Error: send failed",NULL, NULL);

    //start play
    srand((unsigned int)time(NULL)+info[0]);
    Potato p;
    struct timeval time;
    time.tv_sec = 2;
    time.tv_usec = 0;
    
    fd_set master;
    fd_set readfds;

    //get next players id
    int neighbor_ids[2] = {info[0]-1, info[0]+1};
    if(info[0] == 0){
        neighbor_ids[0]= info[1]-1;
    }
    if(info[0] == info[1]-1){
        neighbor_ids[1]= 0;
    }

    FD_ZERO(&master);
    int nfds = socket_ringmaster;
    int fd_list[] = {socket_ringmaster,socket_prev, socket_next};
    // cout<<socket_ringmaster <<"  " <<socket_prev << "  "<<socket_next <<endl;
    for(int i = 0;i < 3; i++){
        FD_SET(fd_list[i], &master);
        if(nfds < fd_list[i]){
            nfds = fd_list[i];
        }
    }
    nfds++;

    //listen to incoming patatos and end
    while(true){
        memset(&p, 0,sizeof(p));
        fd_set readfds = master;
        //recieve potato
        status = select(nfds, &readfds, NULL, NULL, NULL);
        errorHandle(status, "Error: select error", NULL, NULL);
        if(status == 0){
            cout<<"listen time limit"<<endl;
            break;
        }
        else if(status == 1){       
            int socket;
            //get the sender
            for(int i = 0;i < 3; i++){
                if(FD_ISSET(fd_list[i], &readfds)){
                    socket = fd_list[i];
                    break;
                }
            }
            // cout<<"Socket "<< socket <<endl;
            status = recv(socket, &p,sizeof(p),0);
            errorHandle(status,"Error: recieve failed",NULL, NULL);
            if(status == 0){//somebody droped
                cerr<<"Error: connection is cloesd"<<endl;
                break;
            }
            if(p.hops == 0){ //the last one
                p.trace[p.count] = (char)info[0]+'0';
                status = send(socket_ringmaster, &p,sizeof(p),0) ;
                if(status == -1 ){
                    cout <<"send error "<<errno<<endl;
                }
                cout<<"I am it."<<endl;
            }
            else if(p.hops == -1){//game is over
                // cout<<"game over recieve" <<endl;
                break;
            }
            else{
                p.trace[p.count] = (char)info[0]+'0';
                p.count++;
                p.hops--;
                int next = rand()%2;
                status = send(fd_list[next+1], &p,sizeof(p),0);
                errorHandle(status,"Error: send failed",NULL, NULL);
                cout<<"Sending potato to "<< neighbor_ids[next]<<endl;
            }
        }else{
            break;
        }
    }

    close(socket_prev);
    close(socket_next);
    // freeaddrinfo(host_info_list2);
    close(socket_ringmaster);


    return 0;
}
