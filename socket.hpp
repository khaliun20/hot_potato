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
#include <unistd.h>
#include <arpa/inet.h>
#define LARGE_NUM 100


int request_connection ( const char * & hostname, const char * & port ){ 
  int status;
  int socket_fd;
  struct addrinfo host_info;
  struct addrinfo *host_info_list;

    memset(&host_info, 0, sizeof(host_info));
    host_info.ai_family   = AF_UNSPEC; 
    host_info.ai_socktype = SOCK_STREAM; 

    status = getaddrinfo(hostname, port, &host_info, &host_info_list);
    if (status != 0) {
      std::cerr << "Error: cannot get address info for host" << std::endl;
      std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
    exit(EXIT_FAILURE);
    } //if

  socket_fd = socket(host_info_list->ai_family, 
		     host_info_list->ai_socktype, 
		     host_info_list->ai_protocol);
    if (socket_fd == -1) {
     std::cerr << "Error: cannot create socket" << std::endl;
     std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
    exit(EXIT_FAILURE);
    }
    status = connect(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
      if (status == -1) {
       std::cerr << "Error: cannot connect to socket" << std::endl;
        std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
     exit(EXIT_FAILURE);
      } 

      freeaddrinfo(host_info_list);

      return socket_fd;  
  }

int create_listening_socket (const  char * & hostname, const char * & port ){

int server_fd;
struct addrinfo host_info;
struct addrinfo * host_info_list;


memset (& host_info, 0, sizeof(host_info));
//specify the type of socket connection required
host_info.ai_family   = AF_UNSPEC; 
host_info.ai_socktype = SOCK_STREAM; 
host_info.ai_flags    = AI_PASSIVE;

int status = getaddrinfo(hostname, port, &host_info, &host_info_list);
if (status != 0) {
    std::cerr << "Error: cannot get address info for host" << std::endl;
    std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
    exit(EXIT_FAILURE);
} 

// loop through the getaddrinfo to identify the host_info that is working 


server_fd = socket(host_info_list->ai_family, 
		     host_info_list->ai_socktype, 
		     host_info_list->ai_protocol);  
                                      
if (server_fd == -1) {
    std::cerr << "Error: cannot create socket" << std::endl;
    std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
    exit(EXIT_FAILURE);
} 

int yes = 1;

status = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

status = bind(server_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);

if (status == -1) {
    std::cerr << "Error: cannot bind socket" << std::endl;
    std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
    exit(EXIT_FAILURE);
}

status = listen(server_fd, 100);

if (status == -1) {
    std::cerr << "Error: cannot listen on socket" << std::endl; 
    std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
    exit(EXIT_FAILURE);
} 
 
freeaddrinfo(host_info_list);

 //std::cout << "server successfully created" << std::endl;
 //std::cout << "this is the server fd:" << server_fd << std::endl;

 return server_fd;    
}


int accept_connection_request ( int & socket_fd, std::string & hostname ){
  // int & socket_fd = listening socket of the server
  // std::string & hostname 

 struct sockaddr_storage socket_addr; 
  socklen_t socket_addr_len = sizeof(socket_addr);
  int client_connection_fd;


client_connection_fd = accept(socket_fd, (struct sockaddr *)&socket_addr, &socket_addr_len);
  if (client_connection_fd == -1) {
    std::cerr << "Error: cannot accept connection on socket" << std::endl;;
    return -1;
  } 

  struct sockaddr_in * addr = (struct sockaddr_in *)&socket_addr;
  hostname = inet_ntoa(addr->sin_addr);

return client_connection_fd;

}

 void announce_game_started(int &total_players, int & total_hops){
        std::cout << "Potato Ringmaster" << std::endl;
        std::cout << "Players" << " = "<< total_players << std::endl;
        std::cout << "Hops" << " = "<< total_hops << std::endl;

}

int get_port (int player_fd){
 struct sockaddr_in socket_addr;
  socklen_t socket_addrlen = sizeof(socket_addr);
  if(getsockname(player_fd, (struct sockaddr *)&socket_addr, &socket_addrlen)) {
    std::cout<< "Error: cannot accept connection on socket" << std::endl;
    return -1;
  }
  return ntohs(socket_addr.sin_port);

}


