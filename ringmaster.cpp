#include  <unistd.h>
#include "potato.hpp"
#include <vector> 
#include <iostream>
#include <ostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <cstdlib>
#include <cstdio>
#include "socket.hpp"
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>

int main (int argc, char ** argv){

//ringmaster <port_num> <num_players> <num_hops>
if (argc != 4){
    std::cerr << " Please check the number of arguments" << std::endl;
    return 1; 
}

if (atoi(argv[2]) <= 1 || atoi(argv[3]) < 0){
    std::cerr << "There is either not enough players or not enough hops defined" << std::endl;
    return 1;
}

const char * hostname = NULL;
const char * port = argv[1];
int num_of_players = atoi(argv[2]);
int num_of_hops = atoi(argv[3]);
std::vector <int> all_player_fd;
std::string client_hostname;
std::vector <std::string> all_player_hostname;
std::vector <int> all_player_port;
int player_port;

int listen_fd = create_listening_socket (hostname, port);
announce_game_started (num_of_players, num_of_hops);

//receive all player's port and hostname and send them their player id
for(int i =0; i<num_of_players; i++){
    int player_fd = accept_connection_request (listen_fd,client_hostname);
    all_player_fd.push_back(player_fd);
    all_player_hostname.push_back(client_hostname);
    recv(player_fd, &player_port, sizeof(player_port),0 );
    all_player_port.push_back (player_port);
    send (player_fd, &i, sizeof(i), 0);
    send (player_fd, &num_of_players, sizeof(num_of_players), 0);
    send (player_fd, &num_of_hops, sizeof(num_of_hops), 0);
    std::cout << "Player " << i << " is ready to play" <<std::endl;
}

if (num_of_hops==0){
    for (int i = 0; i < num_of_players; i++) {
    close (all_player_fd[i]);  
    }

    close(listen_fd);

    return EXIT_SUCCESS;
}

//send to each player their neighboring player's info

for (int i = 0; i < num_of_players; i++){
    int next_player;
    if (i + 1 == num_of_players){
        next_player = i+1-num_of_players;
    } else {
        next_player= i + 1;
    }

   char char_hostname[LARGE_NUM];
    memset(char_hostname, 0, sizeof(char_hostname));
    strcpy(char_hostname, all_player_hostname[next_player].c_str());
    char char_port[LARGE_NUM];
    memset(char_port, 0, sizeof(char_port));
    snprintf(char_port, sizeof(char_port), "%d", all_player_port[next_player]);
    //std::cout<< char_hostname<<std::endl;
    send ( all_player_fd[i], &char_port, sizeof (char_port),0);
    send ( all_player_fd[i], &char_hostname, sizeof(char_hostname),0);

}

//start the game by passing the potato to random player

Hot_Potato potato;
potato.hops = num_of_hops;

srand((unsigned int) time(nullptr));
int random_index = rand() % num_of_players;
int random_player_fd = all_player_fd[random_index];
send(random_player_fd, &potato, sizeof(potato),0 );
std::cout << "Ready to start the game, sending potato to player " << random_index << std::endl;

//listen to all players to get the potato back

fd_set fdset;
while (1){
    FD_ZERO(&fdset);
    int fdmax= all_player_fd[0];
    int received;
    for(int i =0; i < num_of_players;i++) {
        fdmax = std::max(fdmax, all_player_fd[i]);
        FD_SET(all_player_fd[i], &fdset);
    }
    int status =  select(fdmax + 1, &fdset, NULL, NULL, NULL);
    if (status == -1) {
        std::cerr << "Not valid - error with select()" << std::endl;
        return EXIT_FAILURE;
    }
    else if (status == 0) {
    // do nothing
    }
    else {
        for (int i = 0; i < num_of_players; i++) {
            if (FD_ISSET(all_player_fd[i], &fdset)) {
                received = recv(all_player_fd[i], &potato, sizeof(potato), 0);
                if(received > 0){
                potato.done = 1;
                //std::cout << "Game over" << std::endl;
                }
                break;
            }
        }
    }

    if (potato.done==1){
        break;
    }
}

for (int i = 0; i < num_of_players; i++) {
    send(all_player_fd[i], &potato, sizeof(potato), 0);
}
    //Print the trace of potato
std::cout << "Trace of potato:" << std::endl;

 for (int i = 0; i < num_of_hops; i++) {
        if (i != num_of_hops - 1)
            std::cout << potato.trace[i] << ",";
        else
            std::cout << potato.trace[i] << std::endl;
    }

for (int i = 0; i < num_of_players; i++) {
close (all_player_fd[i]);  
}

close(listen_fd);

return EXIT_SUCCESS;
}