#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string>

struct playerInfo_t{
    char * hostname;
    char * port;
};

void errorHandle(int status, std::string message,const char * hostname, const char * port){
    if (status < 0) {
        std::cerr << message << std::endl;
        std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
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
    // std::cout << "Builded " << std::endl;
    return socket_fd;

    // char buffer[512];
        // recv(player_connection_ids[num], buffer, 9, 0);
        // buffer[9] = 0;

        // cout << "Server received: " << buffer << endl;

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

    // std::cout << "Connecting to " << hostname << " on port " << port << "..." << std::endl;

    status = connect(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
    errorHandle(status, "Error: cannot connect to socket", hostname, port);
    freeaddrinfo(host_info_list);
    return socket_fd;
}