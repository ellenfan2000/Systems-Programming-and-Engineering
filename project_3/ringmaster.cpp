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
    int num_players = strtol(argv[2],NULL, 0);
    int num_hops = strtol(argv[3],NULL, 0);
    
    //input is not valid
    if(num_players <= 1 || num_hops < 0 || num_hops >512){
        cout << "Error: Wrong input value" << endl;
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
    vector<char *> player_host_names;
    unsigned short int player_ports[num_players];

    char * prev_hostname;
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
            char player_hostname[512];
            status = recv(player_connection_ids[num], player_hostname, sizeof(player_hostname), 0);
            if(status == 0){
                cerr<<"Error: connection is cloesd"<<endl;
                exit(EXIT_FAILURE);
            }
            errorHandle(status,"Error: recieve failed",NULL, NULL);

            status = recv(player_connection_ids[num], &player_ports[num], sizeof(player_ports[num]), 0);
            if(status == 0){
                cerr<<"Error: connection is cloesd"<<endl;
                exit(EXIT_FAILURE);
            }
            errorHandle(status,"Error: recieve failed",NULL, NULL);

            player_host_names.push_back(player_hostname);

            // cout <<"RingMaster recieved ip "<<player_hostname<<endl;
            // cout << "RingMaster recieved port number " << player_ports[num] << endl;
        }

        //if it is the first connected player, does not know its previous player's info
        if(num == 0){
            continue;
        }

        //send previous player's port number and ip
        int prev_id = num-1;
        int cur_id = num;
        if(num == num_players){
            cur_id = 0;
        }
        // prev_hostname= inet_ntoa(((struct sockaddr_in *)&socket_addrs[prev_id])->sin_addr);
        // cout<<"current id is " << cur_id << " prev_id is  " << prev_id << endl;
        prev_hostname = player_host_names[prev_id];
        status = send(player_connection_ids[cur_id], &player_ports[prev_id], sizeof(player_ports[prev_id]), 0);
        errorHandle(status,"Error: send failed",NULL, NULL);
        status = send(player_connection_ids[cur_id], prev_hostname, strlen(prev_hostname), 0);
        errorHandle(status,"Error: send failed",NULL, NULL);
        // cout<<"sending hostname is " << prev_hostname << " Port is " << player_ports[prev_id] << endl;

        //from the second player, it should be ready when the player after it is connected to Ringmaster
        if(num > 1){
            char ready;
            status = recv(player_connection_ids[prev_id], &ready, sizeof(ready), 0);
            if(status == 0){
                cerr<<"Error: connection is cloesd"<<endl;
                exit(EXIT_FAILURE);
            }
            errorHandle(status,"Error: recieve failed",NULL, NULL);

            if(ready == 'R'){
                cout<< "Player " << prev_id << " is ready to play."<<endl;
            }
            if(num == num_players){
                status = recv(player_connection_ids[0], &ready, sizeof(ready), 0);
                if(status == 0){
                    cerr<<"Error: connection is cloesd"<<endl;
                    close(socket_fd);
                    exit(EXIT_FAILURE);
                }
                errorHandle(status,"Error: recieve failed",NULL, NULL);
                if(ready == 'R'){
                    cout<< "Player " << 0 << " is ready to play."<<endl;
                }

            }
        }
    }

    //players are ready
    //play the game

    Potato p = Potato(num_hops-1);
    srand(42);
    int first = rand() % num_players;
    cout<<"Ready to start the game, sending potato to player " << first<<endl;
    
    //send potato to the first player
    status = send(player_connection_ids[first], &p, sizeof(p), 0);
    errorHandle(status,"Error: send failed",NULL, NULL);
    // cout<<"successfully send " << byte1<<endl;

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
                status = recv(player_connection_ids[i], &p,sizeof(p),0);
                if(status == 0){
                    cerr<<"Error: connection is cloesd"<<endl;
                    exit(EXIT_FAILURE);
                }
                errorHandle(status,"Error: recieve failed",NULL, NULL);
                if(p.hops == 0){
                //send to all game over potato with -1 hops
                    p.hops = -1;
                    // cout<<"game over"<<endl;
                    for(int i = 0; i < num_players; i++){
                        status = send(player_connection_ids[i], &p, sizeof(p), 0);
                        errorHandle(status,"Error: send failed",NULL, NULL);
                    }
                }
                break;
            }
        }
    }
    cout<<"Trace of potato:"<<endl;
    for(int j = 0; j<=p.count; j++){
        if(j == 0){
            cout<<p.trace[j];
        }else{
            cout<<", "<<p.trace[j];
        }
    }
    cout<<endl;

    for(int i = 0; i < num_players; i++){
        close(player_connection_ids[i]);
    }

    close(socket_fd);

    return 0;
}
