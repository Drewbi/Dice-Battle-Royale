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
 * 
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
        char* buf = calloc(BUFFER_SIZE, sizeof(char));
        int client_fd = accept(server_fd, (struct sockaddr *) &client, &client_len);  
          

        if (client_fd < 0) {
            fprintf(stderr,"Could not accept new connection.\n");
            exit(EXIT_FAILURE);
        }

        pid = fork();
        if (pid < 0) {
            fprintf(stderr,"Can't create child process\n");
        }

        else if (pid == 0) {
            close(server_fd);
            
            while(true) {
                printf("Player number from server: %d\n", current_game.player_number);
                game_session(current_game, client_fd);
            } 
        }
        else {
            printf("Connection being made by player %d\n", client_fd);
            current_game.player_number++;
        }
    }
}
