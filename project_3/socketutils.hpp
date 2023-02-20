#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include "potato.hpp"

void errorHandle(int status, std::string message,const char * hostname, const char * port){
    if (status < 0) {
        std::cerr << message << std::endl;
        if(hostname!=NULL && port != NULL){
            std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
        }
        exit(EXIT_FAILURE);
    } 
}

int buildServer(const char * port){
    const char * hostname = NULL;
    struct addrinfo host_info;
    struct addrinfo *host_info_list;
    int status, socket_fd;

    memset(&host_info, 0, sizeof(host_info));

    host_info.ai_family   = AF_INET;
    host_info.ai_socktype = SOCK_STREAM;
    host_info.ai_flags    = AI_PASSIVE;

    status = getaddrinfo(hostname, port, &host_info, &host_info_list);
    errorHandle(status,"Error: cannot get address info for host", hostname, port);

    //generate a port
    if(port == ""){
        ((struct sockaddr_in *)host_info_list->ai_addr)->sin_port = 0;
    }

    socket_fd = socket(host_info_list->ai_family, 
                host_info_list->ai_socktype, 
                host_info_list->ai_protocol);
    errorHandle(socket_fd,"Error: cannot create socket", hostname, port);

    int yes = 1;
    status = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));//reuse port
    errorHandle(status,"Error: setsocketopt", hostname, port);

    status = bind(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
    errorHandle(status, "Error: cannot bind socket", hostname, port);

    status = listen(socket_fd, 100);
    errorHandle(status,"Error: cannot listen on socket",hostname, port);

    freeaddrinfo(host_info_list);
    return socket_fd;
}

int buildClient(const char * hostname, const char * port){
    int status;
    int socket_fd;
    struct addrinfo host_info;
    struct addrinfo * host_info_list;

    memset(&host_info, 0, sizeof(host_info));
    host_info.ai_family   = AF_INET;
    host_info.ai_socktype = SOCK_STREAM;

    status = getaddrinfo(hostname, port, &host_info, &host_info_list);
    errorHandle(status,"Error: cannot get address info for host", hostname, port);

    socket_fd = socket(host_info_list->ai_family, 
                host_info_list->ai_socktype, 
                host_info_list->ai_protocol);
    errorHandle(socket_fd,"Error: cannot create socket" , hostname, port);

    status = connect(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
    errorHandle(status, "Error: cannot connect to socket", hostname, port);
    freeaddrinfo(host_info_list);
    return socket_fd;
}

//some test code
// char text;
    // if(info[0] == 1){
    //     status = recv(socket_ringmaster, &text,sizeof(text),MSG_WAITALL);
    //     if(status == -1){
    //         cerr<<"recieve Wrong" <<endl;
    //     }
    // }
    // else{
    //     status = recv(socket_prev, &text,sizeof(text),MSG_WAITALL);
    //     if(status == -1){
    //         cerr<<"recieve Wrong"<<endl;
    //     }
    // }

    // cout <<"Recieved is "<<text<<endl;
    // text++;
    // if(info[0] == info[1]-1){
    //     status = send(socket_ringmaster, &text,sizeof(text),0);
    //     if(status == -1){
    //         cerr<<"send Wrong "<<errno <<endl;
    //     }
    // }

    // status = send(socket_next, &text,sizeof(text),0);
    // if(status == -1){
    //     cout<<"send Wrong next "<<errno<<endl;
    // }
        // status = send(socket_prev, &text,sizeof(text),0);
        // if(status == -1){
        //     cout<<"sned Wrong prev";
        // }
    // cout<<"nfds is" << nfds <<endl;
