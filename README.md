## Hot Potato Game

### Overview: 

This project involves developing a pair of programs to model a game called "hot potato". The game consists of players tossing a potato among themselves until the game ends. 
The objective is to avoid being the last player holding the potato. 

## Game Rules:
The game begins with the ringmaster generating a potato object containing a set number of hops, which is then "thrown" to a player chosen at random. 
Upon receiving the potato, each player reduces the hop count and adds its own identifier to the potato. 
If hops remain, the player selects another player randomly (from the the players standing directly next to him) and passes the potato to them. 
The game concludes when the hop count reaches zero, at which point the player holding the potato is considered the "loser". The loser then returns the potato to the ringmaster, marking the game's end. 
The ringmaster then logs a trace of the game and proceeds to shut down.

## Communication Mechanism
TCP sockets are used for communication between the ringmaster and all the players
Players establish TCP socket connections with their left and right neighbors and the ringmaster.
Command line arguments must adhere to the specified format.
