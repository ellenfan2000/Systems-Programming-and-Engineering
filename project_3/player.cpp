#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
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
    // hostname= inet_ntoa(((struct sockaddr_in *)&socket_addr)->sin_addr);
    int byte = send(socket_ringmaster, &port_listen, sizeof(port_listen), 0);
    // int byte1 = send(socket_ringmaster, hostname, strlen(hostname), 0);

    char prev_hostname[100];
    unsigned short int prev_port;
    char prev_port_name[9];

    if(info[0]> 0){
        // recv(socket_ringmaster, &prev, sizeof(prev), 0);
        recv(socket_ringmaster, &prev_port, sizeof(prev_port), 0);
        recv(socket_ringmaster, prev_hostname, 32, 0);
        prev_hostname[32] = 0;
        sprintf(prev_port_name, "%u", prev_port);
        // cout<<"Recieve hostname "<< prev_hostname <<" recieve port number " <<prev_port_name <<endl;  
        socket_prev = buildClient(prev_hostname, prev_port_name);
    }
    
    // cout<<"waiting for connection "<<endl;
    struct sockaddr_storage socket_addr_next;
    socklen_t socket_addr_len_next = sizeof(socket_addr_next);
    status = accept(socket_next, (struct sockaddr *)&socket_addr_next, &socket_addr_len_next);
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

    char message = 'R';
    byte = send(socket_ringmaster, &message, sizeof(message), 0);


    close(socket_prev);
    close(socket_next);
    // freeaddrinfo(host_info_list2);
    close(socket_ringmaster);


    return 0;
}
