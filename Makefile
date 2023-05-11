CC=g++
CFLAGS=-g

all : ringmaster player

ringmaster : ringmaster.cpp potato.hpp socket.hpp
			 $(CC) $(CFLAGS) -o $@ ringmaster.cpp

player : player.cpp potato.hpp socket.hpp
		 $(CC) $(CFLAGS) -o $@ player.cpp

clean: 
		rm -rf ringmaster player *.o *~