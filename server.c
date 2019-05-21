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

    int port = atoi(argv[1]);

    int server_fd, client_fd, err, opt_val;
    struct sockaddr_in server, client; 
    struct game_session current_game;
    char *buf;
    fd_set read_fds;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd < 0){
        fprintf(stderr,"Could not create socket\n");
        exit(EXIT_FAILURE);
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    opt_val = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof opt_val);

    err = bind(server_fd, (struct sockaddr *) &server, sizeof(server));
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

    socklen_t client_len = sizeof(client);

    while (true) {
        int pid;
        char* buf = calloc(BUFFER_SIZE, sizeof(char));
        client_fd = accept(server_fd, (struct sockaddr *) &client, &client_len);  
          

        if (client_fd < 0) {
            fprintf(stderr,"Could not accept new connection.\n");
            exit(EXIT_FAILURE);
        }

        pid = fork();
        if (pid < 0) {
            fprintf(stderr,"Can't create child process\n");
        }

        else if (pid == 0) {

            current_game.player_number++;
            printf("Setting player count up one");
            close(server_fd);
            
            while(true) {
                printf("Player number from server: %d\n", current_game.player_number);
                game_session(current_game, client_fd);

                /*
                int client_read = recv(client_fd, buf, BUFFER_SIZE, 0);
                if (client_read < 0) {
                    printf("Can't read from client");
                }
                printf("Receiving messeges from %d", client_fd);

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
                    if((strstr(buf, "INIT"))) {
                        if(current_game.player_number <= MAX_PLAYERS) {
                            add_player(current_game, client_fd);
                            current_game.player_number++;
                            
                            buf[0] = '\0';
                            sprintf(buf, "WELCOME,%d", client_fd);
                            send(client_fd, buf, strlen(buf), 0);
                            sleep(10); 
                        }

                        else {
                            buf[0] = '\0';
                            sprintf(buf, "REJECT");
                            send(client_fd, buf, strlen(buf), 0);
                            close(client_fd);
                        }

                        if (current_game.player_number >= 2) {
                            buf[0] = '\0';
                            sprintf(buf, "START,%d,%d", current_game.player_number, 3);
                            send(client_fd, buf, strlen(buf), 0);
                            sleep(10);

                            while (true) {
                                int round = 1;
                                printf("ROUND %d OUT OF %d\n", round, current_game.rounds);
                                char *move = calloc(BUFFER_SIZE, sizeof(char));
                                int* dice_result = diceroll();
                                sprintf(buf, "ROUND %d OUT OF %d\n", round, current_game.rounds);
                                send(client_fd, buf, strlen(buf), 0);
                                err = send(client_fd, buf, strlen(buf), 0);
                                // printf("I GOT HERE\n");

                                if (err < 0) {
                                    printf("Error in sending round info\n");
                                }

                                buf[0] = '\0';
                                err = recv(client_fd, move, BUFFER_SIZE, 0);
                                if (err < 0) {
                                    printf("Error in receiving player move");
                                }
                                // printf("NOW I'M HERE\n");
                                if (strstr(buf, "MOV")) {
                                    printf("PLAYER %d's move: %s\n", client_fd, move);
                                    char* result = eval_move(buf, dice_result, client_fd);
                                    send(client_fd, result, strlen(result), 0);
                                }
                                else {
                                    buf[0] = '\0';
                                    sprintf(buf, "%d,FAIL", client_fd);
                                    send(client_fd, buf, strlen(buf), 0);
                                    // this is a great line learn
                                }
                
                            }
                        }
                    } 
                    else {
                        buf[0] = '\0';
                        sprintf(buf, "Expected INIT message. Got gibberish. Goodbye.");
                        send(client_fd, buf, strlen(buf), 0);
                        close(client_fd);
                    }

                } */
                
            } 
        }
        else {
            printf("Connection being made by player %d\n", client_fd);
        }
    }
}
