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

    // char name[32];
    // gethostname(name,sizeof(name));
    // cout<<"current host name"<<name<<endl;
    //connect to ringmaster
    socket_ringmaster = buildClient(hostname, port);

    //recieve player id
    int info[2];
    recv(socket_ringmaster, &info, sizeof(info), 0);
    cout << "Connected as player "<< info[0]<<" out of " << info[1] <<" total players" << endl;

    //build Server
    socket_next = buildServer("");
    //get port number
    struct sockaddr_storage socket_addr;
    socklen_t socket_addr_len = sizeof(socket_addr);
    status = getsockname(socket_next,(struct sockaddr *)&socket_addr, &socket_addr_len);
    unsigned short int port_listen = ntohs(((struct sockaddr_in *) &socket_addr)->sin_port);
    errorHandle(status,"Error: cannot get port info from socket", NULL, port);
    // cout << "Send port number " << port_listen << endl;

    //send port info，ip info to ringmaster
    int byte = send(socket_ringmaster, &port_listen, sizeof(port_listen), 0);

    char prev_hostname[100];
    unsigned short int prev_port;
    char prev_port_name[9];

    if(info[0]> 0){
        // recv(socket_ringmaster, &prev, sizeof(prev), 0);
        recv(socket_ringmaster, &prev_port, sizeof(prev_port), 0);
        recv(socket_ringmaster, prev_hostname, 32, 0);
        prev_hostname[32] = 0;
        sprintf(prev_port_name, "%u", prev_port);
        socket_prev = buildClient(prev_hostname, prev_port_name);
    }
    
    // cout<<"waiting for connection "<<endl;
    struct sockaddr_storage socket_addr_next;
    socklen_t socket_addr_len_next = sizeof(socket_addr_next);
    socket_next = accept(socket_next, (struct sockaddr *)&socket_addr_next, &socket_addr_len_next);
    // cout << "connect to" <<ntohs(((struct sockaddr_in *)&socket_addr_next)->sin_port)<<endl;
    errorHandle(status,"cannot accept", hostname, NULL);

    // cout<<"connect to next "<<endl;



    if(info[0] == 0){
        recv(socket_ringmaster, &prev_port, sizeof(prev_port), 0);
        recv(socket_ringmaster, prev_hostname, 32, 0);
        prev_hostname[32] = 0;
        sprintf(prev_port_name, "%u", prev_port);
        // cout<<"Recieve hostname "<< prev_hostname <<" recieve port number " <<prev_port_name<<endl;
        socket_prev = buildClient(prev_hostname, prev_port_name);
    }
    // cout<<"Recieve hostname "<< prev_hostname <<" recieve port number " <<prev_port_name <<endl;  

    //Player ready
    char message = 'R';
    byte = send(socket_ringmaster, &message, sizeof(message), 0);

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
        // don't care about writefds and exceptfds:
        //recieve potato
        status = select(nfds, &readfds, NULL, NULL, NULL);
        // cout<<status<<endl;
        errorHandle(status, "Error: select error", NULL, NULL);
        if(status == 0){
            cout<<"listen time limit"<<endl;
            break;
        }
        else if(status == 1){       
            // if(FD_ISSET(socket_ringmaster, &readfds)){
            //     recv(socket_ringmaster, &p,sizeof(p),0);
            int socket;
            // }
            for(int i = 0;i < 3; i++){
                if(FD_ISSET(fd_list[i], &readfds)){
                    socket = fd_list[i];
                    break;
                }
            }
            // cout<<"Socket "<< socket <<endl;
            status = recv(socket, &p,sizeof(p),0);
            if(status == -1){
                cerr<<"recieve Wrong"<<endl;
            }
            if(status == 0){
                break;
            }
            if(p.hops == 0){
                p.trace[p.count] = (char)info[0]+'0';
                status = send(socket_ringmaster, &p,sizeof(p),0) ;
                if(status == -1 ){
                    cout <<"send error "<<errno<<endl;
                }
                // cout<<"Sending potato to RingMaster for "<< status << " byte "<<endl;
                cout<<"I am it."<<endl;
                break;
            }
            else{
                p.trace[p.count] = (char)info[0]+'0';
                p.count++;
                p.hops--;
                int next = rand()%2;
                status = send(fd_list[next+1], &p,sizeof(p),0);
                if(status == -1 ){
                    cout <<"send error "<<errno<<fd_list[next+1]<<endl;
                }
                cout<<"Sending potato to "<< neighbor_ids[next]<<endl;
            }
        }else{
            // cout<<"more than one send"<<endl; 
            break;
        }

    }

    close(socket_prev);
    close(socket_next);
    // freeaddrinfo(host_info_list2);
    close(socket_ringmaster);


    return 0;
}
