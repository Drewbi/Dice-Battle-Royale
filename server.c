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

int setup_socket(int port);
int accept_connection(int server_fd, struct sockaddr_in client, socklen_t client_len);

int main (int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr,"Usage: %s [port]\n",argv[0]);
        exit(EXIT_FAILURE);
    }
    struct game_session game = init_game();

    int port = atoi(argv[1]);

    int server_fd = setup_socket(port);

    printf("Server is listening on %d\n", port);

    // Connect clients
    struct sockaddr_in client; 
    socklen_t client_len = sizeof(client);

    while (true) {
        
        int socket[2];
        if (pipe(socket) < 0) {
            fprintf(stderr, "Couldn't pipe\n");
        }

        int client_fd = accept_connection(server_fd, client, client_len);

        int pid = fork();
        if (pid < 0) {
            fprintf(stderr,"Can't create child process\n");
        }

        if (pid == 0) {
            close(server_fd);
            close(socket[1]);
            
            ssize_t fork_read = read(socket[0], &game.player_number, sizeof(game.player_number));
            if (fork_read <= 0) {
                fprintf(stderr, "Could not read from parent\n");
            }

            printf("As read from parent: %d\n", game.player_number);
            close(socket[0]);

            while(true) {
                //printf("Player number from server: %d\n", game.player_number);
                //game_session(game, client_fd);
                printf("am connected...\n");
                sleep(10);
            }
        }
        else {
            printf("Connection being made by player %d\n", client_fd);
            game.player_number++;
            printf("Player number %d has joined\n", game.player_number);
            close(socket[0]);

            write(socket[1], &game.player_number, sizeof(game.player_number));

            printf("Parent sending value: %d\n", game.player_number);
            
            close(socket[1]);
        }
    }
}

int setup_socket(int port){
    // Set up socket and connection
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

    return server_fd;
}

int accept_connection(int server_fd, struct sockaddr_in client, socklen_t client_len){
    int client_fd = accept(server_fd, (struct sockaddr *) &client, &client_len);  

    if (client_fd < 0) {
        fprintf(stderr,"Could not accept new connection.\n");
        exit(EXIT_FAILURE);
    }

    return client_fd;
}