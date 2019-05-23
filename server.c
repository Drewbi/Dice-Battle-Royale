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

int create_socket(int port);

int run = 1;

int main (int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr,"Usage: %s [port]\n",argv[0]);
        exit(EXIT_FAILURE);
    }
   

    int port = atoi(argv[1]);

    int server_fd = create_socket(port);

    struct sockaddr_in client; 
    socklen_t client_len = sizeof(client);

    struct game_session game = init_game();
    
    while (run) {
        int pid;
        int fd[2];
        int child_status;
        char* buf = calloc(BUFFER_SIZE, sizeof(char));
        int client_fd = accept(server_fd, (struct sockaddr *) &client, &client_len);
        printf("Connection being made\n");  
        if (client_fd < 0) {
            fprintf(stderr,"Could not accept new connection.\n");
            exit(EXIT_FAILURE);
        }
        
        if (game.player_number < MAX_PLAYERS) {
            buf = calloc(BUFFER_SIZE, sizeof(char));
            buf[0] = '\0';
            recv(client_fd, buf, BUFFER_SIZE, 0);
            if (strstr(buf, "INIT") != NULL){
                game.players[game.player_number].client_fd = client_fd;
                send_message("WELCOME", game.player_number, game);
                printf("Player number %d has joined.\n", game.player_number);
                game.player_number++;
            }
            else reject_player(client_fd);
        }
        
        if (game.player_number == MAX_PLAYERS) {
            pid = fork();
            if(pid == 0){
                while(run){
                    int client_fd = accept(server_fd, (struct sockaddr *) &client, &client_len);
                    reject_player(client_fd);
                }
                exit(0);
            }
        }

        if (game.player_number < MAX_PLAYERS) {
            sleep(1);
            continue;
        }


        int children_pid[game.player_number];
        for (int i = 0; i < game.player_number; i++) {
            pid = fork();
            children_pid[i] = pid;
            if (pid == 0) { // Client child communication processes
                int client_id = i;
                int client_fd = game.players[i].client_fd;
                send_message("START", client_id, game);
                
                while(run){
                    game.rounds++;
                    sleep(3); // We were unable to implement move timers
                    char* message = get_message(client_id, game);
                    int* roll = roll_dice();
                    bool passed = eval_move(message, roll, client_id, game);
                    if (passed) {
                        send_message("PASS", client_id, game);
                    }
                    else {
                        send_message("FAIL", client_id, game);
                        game.players[client_id].player_lives--;
                    }

                    if (game.players[client_id].player_lives == 0){
                        send_message("ELIM", client_id, game);
                        exit(3);
                    }
                    if(game.rounds == MAX_ROUNDS){
                        send_message("VICT", client_id, game);
                        exit(3);
                    }
                }
                exit(3);
            }
        }
    
        while (run) { // Parent process
            int players_remaining = game.player_number;
            int winner_id;
            for (int j = 0; j < game.player_number; j++) {
                if (children_pid[j] == -1) {
                    players_remaining--;
                    continue;
                }
                
                int isDead = waitpid(children_pid[j], &child_status, WNOHANG);
                if(isDead != 0 && isDead != -1) {
                    printf("Player %d exited the game.\n", j);
                    children_pid[j] = -1; // Rip Child
                }
                winner_id = j;
            }

            if (players_remaining == 1) {
                send_message("VICT", winner_id, game);
                printf("Exiting...\n");
                exit(3);
            }
        }
    }
}

int create_socket(int port){
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0){
        fprintf(stderr,"Could not create socket\n");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server; 
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    struct timeval timeout;
    timeout.tv_sec = 10000; // 10 second timeout for receives

    int opt_val = 1;
    setsockopt(server_fd, SOL_SOCKET,  SO_REUSEADDR, &opt_val, sizeof(opt_val));
    setsockopt(server_fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(struct timeval));

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
    return server_fd;
}



