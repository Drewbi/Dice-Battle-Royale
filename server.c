/**
* Based on code found at https://github.com/mafintosh/echo-servers.c (Copyright (c) 2014 Mathias Buus)
* Copyright 2019 Nicholas Pritchard, Ryan Bunney
**/

/**
 * @brief A simple example of a network server written in C implementing a basic echo
 * 
 * This is a good starting point for observing C-based network code but is by no means complete.
 * We encourage you to use this as a starting point for your project if you're not sure where to start.
 * Food for thought:
 *   - Can we wrap the action of sending ALL of out data and receiving ALL of the data?
 */

#include "header.h"

int main (int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr,"Usage: %s [port]\n",argv[0]);
        exit(EXIT_FAILURE);
    }
    struct game_session current_game;

    // Set up socket and connection
    int port = atoi(argv[1]);

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0){
        fprintf(stderr,"Could not create socket\n");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server; 
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    int opt_val = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof opt_val);

    int err = bind(server_fd, (struct sockaddr *) &server, sizeof(server));
    if (err < 0){
        fprintf(stderr,"Could not bind socket\n");
        exit(EXIT_FAILURE);
    } 

    err = listen(server_fd, 128);
    if (err < 0){
        fprintf(stderr,"Could not listen on socket\n");
        exit(EXIT_FAILURE);
    } 

    printf("Server is listening on %d\n", port);

    struct sockaddr_in client; 
    socklen_t client_len = sizeof(client);

    // Connect clients
    while (true) {
        int pid;
        int client_fd = accept(server_fd, (struct sockaddr *) &client, &client_len);
        if (client_fd < 0) {
            fprintf(stderr,"Could not accept new connection.\n");
            exit(EXIT_FAILURE);
        }
        pid = fork();
        if (pid < 0) {
            fprintf(stderr,"Can't create child process\n");
        }
        if (pid == 0) {
            close(server_fd);
            char* buf = calloc(BUFFER_SIZE, sizeof(char));
            current_game = init_game();

            while(true) {
                int client_read = recv(client_fd, buf, BUFFER_SIZE, 0);
                if (client_read < 0) {
                    printf("Can't read from client");
                }
                // printf("Receiving messeges from %d", client_fd);

                buf[0] = '\0';
                sprintf(buf, "Wanna play fortnite?");
                err = send(client_fd, buf, strlen(buf), 0); 
                sleep(5);

                buf[0] = '\0';
                recv(client_fd, buf, BUFFER_SIZE, 0);
                if((strstr(buf, "No"))) {
                    buf[0] = '\0';
                    sprintf(buf, "Too bad! Because this is EF RNG Battle Royale!");
                    send(client_fd, buf, strlen(buf), 0); 
                    sleep(5);

                    buf[0] = '\0';
                    recv(client_fd, buf, BUFFER_SIZE, 0);
                    if(strstr(buf, "INIT")) {
                        if(current_game.player_number <= MAX_PLAYERS) {
                            add_player(current_game, client_fd);
                            current_game.player_number++;

                            buf[0] = '\0';
                            sprintf(buf, "WELCOME,%d", client_fd);
                            send(client_fd, buf, strlen(buf), 0);
                            sleep(10); 
                        } else {
                            buf[0] = '\0';
                            sprintf(buf, "REJECT");
                            send(client_fd, buf, strlen(buf), 0);
                            close(client_fd);
                        }

                        if (current_game.player_number >= 1) {
                            buf[0] = '\0';
                            sprintf(buf, "START,%d,%d", current_game.player_number, 3);
                            send(client_fd, buf, strlen(buf), 0);
                            sleep(5);

                            for (int r = 1; r <= current_game.rounds; r++) {
                                printf("ROUND %d OUT OF %d\n", r, current_game.rounds);
                                buf[0] = '\0';
                                sprintf(buf, "ROUND %d OUT OF %d", r, current_game.rounds);
                                send(client_fd, buf, strlen(buf), 0);
                                buf[0] = '\0';
                                err = recv(client_fd, buf, BUFFER_SIZE, 0);
                                if(err < 0) {
                                    printf("Error in receiving player move");
                                }
                                if (strstr(buf, "MOV")) {
                                    printf("PLAYER %d's move: %s\n", client_fd, buf);
                                    char** player_move = parse_move(buf);
                                }
                            }
                        }
                    } else {
                        buf[0] = '\0';
                        sprintf(buf, "Expected INIT message. Got gibberish. Goodbye.");
                        send(client_fd, buf, strlen(buf), 0);
                        close(client_fd);
                    }
                }
            }
        }
    }
}


/**
        The following while loop contains some basic code that sends messages back and forth
        between a client (e.g. the socket_client.py client). 
        
        The majority of the server 'action' will happen in this part of the code (unless you decide
        to change this dramatically, which is allowed). The following function names/definitions will 
        hopefully provide inspiration for how you can start to build up the functionality of the server. 
        
        parse_message(char *){...} : 
            * This would be a good 'general' function that reads a message from a client and then 
            determines what the required response is; is the client connecting, making a move, etc. 
            * It may be useful having an enum that is used to track what type of client message is received
            (e.g. CONNECT/MOVE etc.) 
            
        send_message() {...}: 
            * This would send responses based on what the client has sent through, or if the server needs 
            to send all clients messages
        
        play_game_round() {...}: Implements the functionality for a round of the game
            * 'Roll' the dice (using a random number generator) and then check if the move made by the user
            is correct
            * update game state depending on success or failure. 
        
        setup_game/teardown_game() {} : 
            * this will set up the initial state of the game (number of rounds, players
            etc.)/ print out final game results and cancel socket connections. 
        
        Accepting multiple connections (we recommend not starting this until after implementing some
        of the basic message parsing/game playing): 
            * Whilst in a while loop
                - Accept a new connection 
                - Create a child process 
                - In the child process (which is associated with client), perform game_playing functionality
                (or read the messages) 
        **/
        
        /*
        while (true) { 
            int player_count = 0; 
            buf = calloc(BUFFER_SIZE, sizeof(char)); // Clear our buffer so we don't accidentally send/print garbage
            int read = recv(client_fd, buf, BUFFER_SIZE, 0);    // Try to read from the incoming client

            if (read < 0){
                fprintf(stderr,"Client read failed\n");
                exit(EXIT_FAILURE);
            }

            printf("%s\n", buf);

            buf[0] = '\0';
            sprintf(buf, "My politely respondance");
            err = send(client_fd, buf, strlen(buf), 0); // Try to send something back
            buf[0] = '\0';
            sleep(5); //Wait 5 seconds

           
            free(buf);
        } */