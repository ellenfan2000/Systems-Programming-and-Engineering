#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <vector>

#include "socketutils.hpp"

using namespace std;


int main(int argc, char *argv[])
{
    if (argc < 4) {
        cout << "Syntax: client <port_num> <num_players> <num_hops>" << endl;
        return 1;
    }
    int status;
    
    const char *hostname = NULL;
    const char *port = argv[1];
    int num_players = std::strtol(argv[2],NULL, 0);
    int num_hops = std::strtol(argv[3],NULL, 0);
    
    //the input is not valid
    if(num_players <= 1 || num_hops < 0 || num_hops >512){
        cout << "Error: Wrong input value" << endl;
        return EXIT_FAILURE;
    }

    cout<< "Potato Ringmaster" <<endl;
    cout<<"Players = " << num_players <<endl;
    cout <<"Hops = "<<num_hops <<endl;

    int socket_fd = buildServer(port);

    int player_connection_ids[num_players];
    struct sockaddr_storage socket_addrs[num_players];
    unsigned short int player_ports[num_players];

    char * prev_hostname;
    int byte1, byte2;
    for(int num = 0; num <= num_players; num++){
        // cout << "\ncurrent num is "<< num << endl;

        if(num < num_players){
            //new player connect to ringmaster
            socklen_t socket_addr_len = sizeof(socket_addrs[num]);
            player_connection_ids[num] = accept(socket_fd, (struct sockaddr *)&socket_addrs[num], &socket_addr_len);
            errorHandle(player_connection_ids[num], "Error: cannot accept connection on socket", hostname, port);

            // cout<<"Player ip is "<< inet_ntoa(((struct sockaddr_in *)&socket_addrs[num])->sin_addr) << endl;
            
            //send player_id information to the connected player
            int info[2] = {num,num_players};
            int byte = send(player_connection_ids[num], info, sizeof(info), 0);
            
            //recieve port information for player
            recv(player_connection_ids[num], &player_ports[num], sizeof(player_ports[num]), 0);
            // cout << "RingMaster recieved port number " << player_ports[num] << endl;
        }
        if(num == 0){
            continue;
        }

        //send previous player's port number and ip
        int prev_id = num-1;
        int cur_id = num;
        if(num == num_players){
            cur_id = 0;
        }
        prev_hostname= inet_ntoa(((struct sockaddr_in *)&socket_addrs[prev_id])->sin_addr);
        // cout<<"current id is " << cur_id << " prev_id is  " << prev_id << endl;
        
        byte2 = send(player_connection_ids[cur_id], &player_ports[prev_id], sizeof(player_ports[prev_id]), 0);
        byte1 = send(player_connection_ids[cur_id], prev_hostname, strlen(prev_hostname), 0);
        // cout<<"sending hostname is " << prev_hostname << " Port is " << player_ports[prev_id] << endl;


        if(num > 1){
            char ready;
            recv(player_connection_ids[prev_id], &ready, sizeof(ready), 0);
            if(ready == 'R'){
                cout<< "Player " << prev_id << " is ready to play."<<endl;
            }
            if(num == num_players){
                recv(player_connection_ids[0], &ready, sizeof(ready), 0);
                if(ready == 'R'){
                    cout<< "Player " << 0 << " is ready to play."<<endl;
                }

            }
        }
    }

    //players are ready
    //play the game

    Potato p = Potato(num_hops);
    srand(42);
    int first = rand() % num_players;
    cout<<"Ready to start the game, sending potato to player " << first<<endl;
    // p.trace[0] = first;
    // p.count ++;

    // cout<<"Potato has " <<p.hops << " left"<<endl;
    // cout<<"Potato has gone through " <<p.count <<endl;

    // char text = 'a';
    // status = send(fd_list[next+1], &p,sizeof(p),0);
    // status = send(player_connection_ids[first], &text, sizeof(text), 0);
    // if(status == -1 ){
    //     cout <<"send error "<<errno<<endl;
    // }
    byte1 = send(player_connection_ids[first], &p, sizeof(p), 0);
    cout<<"successfully send " << byte1<<endl;

    struct timeval time;
    time.tv_sec = 10;
    time.tv_usec = 0;
    
    fd_set readfds;
    FD_ZERO(&readfds);
    int nfds = player_connection_ids[first];
    for(int i = 0;i < num_players; i++){
        FD_SET(player_connection_ids[i], &readfds);
        if(nfds < player_connection_ids[i]){
            nfds = player_connection_ids[i];
        }
    }
    status = select(nfds+1, &readfds, NULL, NULL, NULL);
    errorHandle(status, "Error: select error", NULL, NULL);
    if(status == 0){
        cout<<"listen time limit"<<endl;
    }
    else if(status == 1){
        for(int i = 0;i < 3; i++){
            if(FD_ISSET(player_connection_ids[i], &readfds)){
                recv(player_connection_ids[i], &p,sizeof(p),0);
                // recv(player_connection_ids[i], &text,sizeof(text),0);
            }
        }
    }
    cout<<"Trace of potato:"<<endl;
    for(int j = 0; j<=p.count; j++){
        cout<<p.trace[j]<<" ";
    }
    cout<<endl;
    close(socket_fd);

    return 0;
    }
