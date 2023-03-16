#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <vector>

#include "socketutils.hpp"

using namespace std;

void playGame(int num_players, int num_hops, int * player_connection_ids ){
    int status;
    
    Potato p;
    memset(&p, 0, sizeof(p));
    p.hops = num_hops-1;
    // memset(&p, 0, sizeof(p));
    // p = Potato(num_hops-1);

    srand((unsigned int)time(NULL));
    int first = rand() % num_players;

    //send potato to the first player
    if(num_hops == 0){
        for(int i = 0; i < num_players; i++){
            status = send(player_connection_ids[i], &p, sizeof(p), 0);
            errorHandle(status,"Error:  player disconnected",NULL, NULL);
        }
        return;
    }else{
        status = send(player_connection_ids[first], &p, sizeof(p), 0);
        errorHandle(status,"Error: send failed",NULL, NULL);
    }
    
    cout<<"Ready to start the game, sending potato to player " << first<<endl;

    // struct timeval time;
    // time.tv_sec = 10;
    // time.tv_usec = 0;
    
    fd_set readfds;
    FD_ZERO(&readfds);
    int nfds = player_connection_ids[first];
    //find the largest socket_fd and set up readfds
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
        //recieve potato from player, game end
        for(int i = 0;i < num_players; i++){
            if(FD_ISSET(player_connection_ids[i], &readfds)){

                memset(&p, 0, sizeof(p));
                status = recv(player_connection_ids[i], &p,sizeof(p),0);
                connectionEnd(status);
                // errorHandle(status,"Error: player disconnected",NULL, NULL);

                if(p.hops == 0){
                //send to all game over potato with -1 hops
                    p.hops = -1;
                    // cout<<"game over"<<endl;
                    for(int i = 0; i < num_players; i++){
                        status = send(player_connection_ids[i], &p, sizeof(p), 0);
                        errorHandle(status,"Error:  player disconnected",NULL, NULL);
                    }
                }
                break;
            }
        }
    }else{
        return;
    }
    cout<<"Trace of potato:"<<endl;
    for(int j = 0; j<=p.count; j++){
        if(j == 0){
            cout<<p.trace[j];
        }else{
            cout<<","<<p.trace[j];
        }
    }
    cout<<endl;

}

int main(int argc, char *argv[])
{
    if (argc < 4) {
        cout << "Syntax: client <port_num> <num_players> <num_hops>" << endl;
        return EXIT_FAILURE;
    }

    int status;
    const char *hostname = NULL;
    const char *port = argv[1];
    int num_players = strtol(argv[2],NULL, 0);
    int num_hops = strtol(argv[3],NULL, 0);
    
    //input is not valid
    if(num_players <= 1 || num_hops < 0 || num_hops > 512){
        cout << "Error: Wrong input value, player number > 1, 0 <= hop < 512 " << endl;
        return EXIT_FAILURE;
    }

    cout << "Potato Ringmaster" << endl;
    cout << "Players = " << num_players << endl;
    cout << "Hops = " << num_hops << endl;

    //build sever socket
    int socket_fd = buildServer(port);

    //store information for each player's server
    int player_connection_ids[num_players];
    struct sockaddr_storage socket_addrs[num_players];
    unsigned short int player_ports[num_players];

    char prev_hostname[512];
    memset(prev_hostname, 0, 512);
    char last_hostname[512];
    memset(last_hostname, 0, 512);
    char curr_hostname[512];

    for(int num = 0; num <= num_players; num++){
        if(num < num_players){
            //new player connect to ringmaster
            socklen_t socket_addr_len = sizeof(socket_addrs[num]);
            player_connection_ids[num] = accept(socket_fd, (struct sockaddr *)&socket_addrs[num], &socket_addr_len);
            errorHandle(player_connection_ids[num], "Error: cannot accept connection on socket", hostname, port);

            // cout<<"Player ip is "<< inet_ntoa(((struct sockaddr_in *)&socket_addrs[num])->sin_addr) << endl;
            
            //send player_id information to the connected player
            int info[2] = {num,num_players};

            status = send(player_connection_ids[num], info, sizeof(info), 0);
            errorHandle(status,"Error: send failed",NULL, NULL);
            
            //recieve port and ip information from player
            memset(&curr_hostname, 0, sizeof(curr_hostname));
            status = recv(player_connection_ids[num], curr_hostname, sizeof(curr_hostname), 0);
            connectionEnd(status);
            // errorHandle(status,"Error: recieve failed",NULL, NULL);
            memset(&player_ports[num], 0, sizeof(player_ports[num]));
            status = recv(player_connection_ids[num], &player_ports[num], sizeof(player_ports[num]), 0);
            connectionEnd(status);
            // errorHandle(status,"Error: recieve failed",NULL, NULL);

            // std::cout<<player_hostname<< " "<<player_ports[num] <<std::endl;
            if(num == num_players-1){
                strcpy(last_hostname, curr_hostname); 
            }
        }

        //if it is the first connected player, does not know its previous player's info
        if(num == 0){
            strcpy(prev_hostname, curr_hostname);
            continue;
        }

        //send previous player's port number and ip
        int prev_id = num-1;
        int cur_id = num;
        if(num == num_players){
            cur_id = 0;
        }

        status = send(player_connection_ids[cur_id], &player_ports[prev_id], sizeof(player_ports[prev_id]), 0);
        errorHandle(status,"Error: send failed",NULL, NULL);
        if(cur_id == 0){
            status = send(player_connection_ids[cur_id],last_hostname, sizeof(prev_hostname), 0);
        }else{
            status = send(player_connection_ids[cur_id], prev_hostname, sizeof(prev_hostname), 0);
        }
        errorHandle(status,"Error: send failed",NULL, NULL);
        
        if(num < num_players){
            strcpy(prev_hostname, curr_hostname);
            // cout<<"sending hostname is " << prev_hostname << " Port is " << player_ports[prev_id] << endl;
        }

        //from the second player, it should be ready when the player after it is connected to Ringmaster
        if(num > 1){
            char ready;
            memset(&ready, 0,  sizeof(ready));
            status = recv(player_connection_ids[prev_id], &ready, sizeof(ready), 0);
            connectionEnd(status);
            // errorHandle(status,"Error: ready status recieve failed",NULL, NULL);

            if(ready == 'R'){
                cout<< "Player " << prev_id << " is ready to play"<<endl;
            }
            if(num == num_players){
                memset(&ready, 0,  sizeof(ready));
                status = recv(player_connection_ids[0], &ready, sizeof(ready), 0);
                connectionEnd(status);
                // errorHandle(status,"Error: ready status recieve failed",NULL, NULL);
                if(ready == 'R'){
                    cout<< "Player " << 0 << " is ready to play"<<endl;
                }
            }
        }

    }

    // sleep(10);
    playGame(num_players, num_hops, player_connection_ids);

    for(int i = 0; i < num_players; i++){
        close(player_connection_ids[i]);
    }
    close(socket_fd);

    return 0;
}
