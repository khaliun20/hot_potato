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
#include <climits>
#define MEM 3




int main (int argc, char ** argv){

if (argc != 3){
    std::cerr << " Please check the number of arguments" << std::endl;
    return 1;
}
const char * ringmaster_hostname = argv[1];
const char * ringmaster_port  = argv[2];
const char * my_port=""; // let OS pick port number
const char * my_hostname= NULL;
int player_id; 
int num_of_players;
int num_of_hops;

int ringmaster_fd = request_connection ( ringmaster_hostname, ringmaster_port);

//add error handling
//std:: cout << "successfully connected to server:" <<ringmaster_hostname<<" & " << ringmaster_port<<std::endl;

// player now becomes a server
int listen_fd = create_listening_socket (my_hostname, my_port);
int port = get_port(listen_fd);
send (ringmaster_fd, &port, sizeof(port),0 );
//std::cout<< "My port number is " << port << std::endl;

//player received it next players's hostname and port
recv ( ringmaster_fd, &player_id, sizeof(player_id),0);
recv ( ringmaster_fd, &num_of_players, sizeof(num_of_players),0);
recv ( ringmaster_fd, &num_of_hops, sizeof(num_of_hops),0);
std::cout<< "Connected as player " <<player_id << " out of "  << num_of_players << " total players"<< std::endl;


if(num_of_hops == 0) {
        close(ringmaster_fd);

        return EXIT_SUCCESS;
}

char port_buffer[LARGE_NUM];
char hostname_buffer[LARGE_NUM];

int recved = recv ( ringmaster_fd, &port_buffer, sizeof(port_buffer),0);
int recved1 = recv ( ringmaster_fd, &hostname_buffer, sizeof(hostname_buffer),0);

port_buffer[recved-1] = '\0';
const char * next_player_port  = port_buffer;

hostname_buffer[recved1-1] = '\0';
const char * next_player_hostname  = hostname_buffer;

//std::cout<< "My next player hostname is " << next_player_hostname << std::endl;
//std::cout<< "My next player port is " << next_player_port << std::endl;

//players connects to next player

int next_player_fd = request_connection ( next_player_hostname, next_player_port);
//player acccepts prev player
std::string prev_player_hostname;
int prev_player_fd = accept_connection_request(listen_fd, prev_player_hostname);
std::vector <int> pair;
pair.push_back(prev_player_fd);
pair.push_back(next_player_fd);
pair.push_back(ringmaster_fd);

 

fd_set my_fd_set;
srand((unsigned int) time(nullptr));
Hot_Potato potato;
   
   while(true){

    FD_ZERO(&my_fd_set);
    int fdmax=pair[0];
    int received;
    for ( int i = 0; i < MEM; i++){
        FD_SET(pair[i], &my_fd_set);
        fdmax=std::max(pair[i], fdmax);
    }
   
    int status =  select(fdmax + 1, &my_fd_set, NULL, NULL, NULL);
    if (status == -1) {
        std::cerr << "Not valid - error with select()" << std::endl;
        return EXIT_FAILURE;
    }
    else if (status == 0) {
    // do nothing
    }
    else {
        for (int i = 0; i < 3; i++) {
            if (FD_ISSET(pair[i], &my_fd_set)) {
                received = recv(pair[i], &potato, sizeof(potato), 0);
               // std::cout << "I Received potato "<< player_id << std::endl;;
                break;
            }
        }
    }


    if (potato.hops==0 || received==0 || potato.done==1){
        break;
    
   }

   if (potato.hops == 1) {
  //cout << "num_hops= " << p.num_hops << endl;
  //cout << "I'm the last one" <<endl;
        potato.hops--;
        potato.hops_done++;
        potato.trace[potato.hops_done-1] = player_id;
    std::cout << "I'm it" << std::endl;
    send(pair[2], &potato, sizeof(potato), 0);//ring_fd
        //cout << "I'm it" << endl;
    }
    //if hops >1, send it to neigbors randomly, send potato to neighbor
    else{
        potato.hops--;
        potato.hops_done++;
        potato.trace[potato.hops_done-1] = player_id;    
        //std::cout<<potato.trace << std::cout;
        int next =rand() % 2;
        if(next) {
            send(pair[1], &potato, sizeof(potato), 0);//right_fd
            int next_player_id = (player_id + 1) % num_of_players;
            std::cout << "Sending potato to " << next_player_id << std::endl;
        }
        else {
            
            send(pair[0], &potato, sizeof(potato), 0);//left_fd
            int next_id = (player_id - 1 + num_of_players) % num_of_players;
            std::cout << "Sending potato to " << next_id << std::endl;
        }
    }
  
}

close(ringmaster_fd);
close(next_player_fd);
close(prev_player_fd);
    
return EXIT_SUCCESS;
}
