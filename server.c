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
    struct game_session current_game = init_game();

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

    int client_1_fd = -1;
    int client_2_fd = -1;

    int client_fds[4] = { -1, -1, -1, -1 };
    
    while (true) {
        int pid;
        int fd[2];
        char* buf = calloc(BUFFER_SIZE, sizeof(char));
        
        int client_fd = accept(server_fd, (struct sockaddr *) &client, &client_len);
        
          
        // if too many connections send reject

        if (client_fd < 0) {
            fprintf(stderr,"Could not accept new connection.\n");
            exit(EXIT_FAILURE);
        }

        if (current_game.player_number == MAX_PLAYERS) {
            buf[0] = '\0';
            sprintf(buf, "REJECT");
            send(client_fd, buf, strlen(buf), 0);
            close(client_fd);
            continue;
        }


        client_fds[current_game.player_number] = client_fd;

        current_game.player_number += 1;

        if (current_game.player_number < MAX_PLAYERS) {
            sleep(1);
            continue;
        }

        // printf("N: %d, 1: %d, 2: %d\n", current_game.player_number, client_1_fd, client_2_fd);

        for (int i = 0; i < current_game.player_number; i++) {
            pid = fork();
            if (pid == 0) {
                while (true) {
                    game_session(current_game, client_fds[i]);
                }
            }
        }



        // if (pipe(fd) < 0) {
        //     fprintf(stderr, "Couldn't pipe\n");
        // }

        // pid = fork();
        // int my_client_id = client_fd;
        // if (pid < 0) {
        //     fprintf(stderr,"Can't create child process\n");
        // }

        // else if (pid == 0) {
        //     close(server_fd);
        //     close(fd[1]);

        //     while(true) {
        //         printf("Player number in child: %d\n", current_game.player_number);
            
        //         ssize_t fork_read = read(fd[0], &current_game.player_number, sizeof(current_game.player_number));

        //         printf("%d fork read bytes: %ld current players: %d client id: %d\n", pid, fork_read, current_game.player_number, my_client_id);
        //         if (fork_read <= 0) {
        //             fprintf(stderr, "Could not read from parent\n");
        //         }

        //         // printf("As read from parent: %d\n", current_game.player_number);
        //         // close(fd[0]);
                
        //         if (current_game.player_number < 2) {
        //             printf("Waiting for more players...\n");
        //             sleep(20);
                    
        //         } else {
        //             game_session(current_game, my_client_id);
        //         }
                


        //         /*
        //         send_message("WELCOME", client_fd, current_game);
        //         add_player(current_game, client_fd);
        //         sleep(5);

        //         printf("Game will start shortly...\n");
        //         sleep(30);

        //         send_message("START", client_fd, current_game);
        //         sleep(15);
            
            
        //         printf("Waiting for more players...\n");
        //         sleep(20);
        //         */
        //     } 
        // }
        // else {
        //     printf("Connection being made by player %d\n", client_fd);
        //     buf[0] = '\0';
        //     sprintf(buf, "Send INIT to play EF Battle Royale!");
        //     send(client_fd, buf, strlen(buf), 0);
        //     sleep(5);

        //     buf[0] = '\0';
        //     recv(client_fd, buf, BUFFER_SIZE, 0);
        //     if (strstr(buf, "INIT") && current_game.player_number < MAX_PLAYERS) {
        //         current_game.player_number++;
        //         printf("%d has sent INIT packet!\n", client_fd);
        //         printf("Player number: %d\n", current_game.player_number);
        //     }
        //     else {
        //         buf[0] = '\0';
        //         sprintf(buf, "REJECT");
        //         send(client_fd, buf, strlen(buf), 0);
        //         close(client_fd);
        //     }
            
        
        //     close(fd[0]);

        //     write(fd[1], &current_game.player_number, sizeof(current_game.player_number));
            
        //     // close(fd[1]);


        // }
    }
}
