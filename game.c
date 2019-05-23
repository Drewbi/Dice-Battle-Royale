#include "header.h"

struct game_session init_game() {
    struct player *players = calloc(4, sizeof(struct player));
    for(int i = 0; i < MAX_PLAYERS; i++){
        players[i].player_lives = PLAYER_LIVES;
        players[i].client_fd = -1;
    }
    int game_rounds = 0;
    int player_number = 0;
    struct game_session game = { players, player_number, game_rounds };
    return game;
}

void reject_player(int client_fd){
    char* response = calloc(BUFFER_SIZE, sizeof(char));
    response[0] = '\0';
    sprintf(response, "REJECT");
    send(client_fd, response, strlen(response), 0);
    printf("Rejecting player...\n");
}

int* roll_dice() {
    int *result;
    result = (int *) malloc(sizeof(int) * 2);
    result[0] = rand() % 6 + 1;
    result[1] = rand() % 6 + 1; 
    return result;
}

void send_dice(int client_fd, int* dice){
    char* response = calloc(BUFFER_SIZE, sizeof(char));
    response[0] = '\0';
    sprintf(response, "DICE,%d,%d", dice[0], dice[1]);
    send(client_fd, response, strlen(response), 0);
}

void send_message(char* message, int client_id, struct game_session game) {
    int client_fd = game.players[client_id].client_fd;

    char* response = calloc(BUFFER_SIZE, sizeof(char));
    response[0] = '\0';


    if (strstr(message, "WELCOME")) {
        sprintf(response, "WELCOME,%d", client_id);
        printf("Player %d has joined.\n", client_id);
    }

    else if(strstr(message, "START")) {
        sprintf(response, "START,%d,%d", game.player_number, PLAYER_LIVES);
        printf("Starting game for player %d.\n", client_id);
    }

    else if (strstr(message, "CANCEL")) {
        sprintf(response, "CANCEL");
        printf("Not enough players, canceling...\n");
    }

    else if (strstr(message, "PASS")) {
        sprintf(response, "%d,PASS", client_id);
        printf("Player %d has passed.\n", client_id);
    }

    else if (strstr(message, "FAIL")) {
        sprintf(response, "%d,FAIL", client_id);
        printf("Player %d has failed.\n", client_id);
    }

    else if (strstr(message, "ELIM")) {
        sprintf(response, "%d,ELIM", client_id);
        printf("Player %d has been eliminated.\n", client_id);
    }

    else if (strstr(message, "VICT")) {
        sprintf(response, "%d,VICT", client_id);
        printf("Player %d is victorious.\n", client_id);
    }

    else if (strstr(message, "KICK")) {
        sprintf(response, "%d,KICK", client_id);
        printf("Player %d has been kicked for cheating.\n", client_id);
    }
    send(client_fd, response, strlen(response), 0);
    free(response);
}


bool eval_move(char* message, int* dice, int client_id, struct game_session game) {
    bool pass = false;
    int client_fd = game.players[client_id].client_fd;
    printf("Message received from %d as: %s\n", client_id, message);
    if (strcmp(message, "") == 0) {
        char* wakeup = calloc(14, sizeof(char));
        wakeup[0] = '\0';
        sprintf(wakeup, "AWAKE?");
        int sent = send(client_fd, wakeup, strlen(wakeup), 0);
        if (sent < 0) {
            printf("Player %d has exited the game. From eval_move\n", client_fd);
            exit(3);
        }
        else {
            printf("Player %d sent an empty message or disconnected!\n", client_id);
            return false;
        }
    }
    
    // Get incoming player id
    int player_id;
    char * token = strtok(message, ",");
    if (token != NULL) {
        player_id = atoi(token);
    } else {
       return false;
    }
    if(player_id == 0 && strstr(token, "0") == NULL){
        player_id = -1;
    }
    if (player_id != client_id){
        printf("Player id from %d is invalid.\n", client_id);
        send_message("KICK", client_id, game);
    }

    // Get incoming move
    char* move;
    token = strtok(NULL, ",");
    if (token != NULL) {
        move = strdup(token);
    } else {
        move = "NONE";
        printf("Player move from %d is invalid.\n", client_id);
    }

    if(strstr(move, "MOV") != NULL){
        token = strtok(NULL, ",");
        if (token != NULL) {
            move = strdup(token);
        } else {
            move = "NONE";
            printf("Player move from %d is invalid.\n", client_id);
        }
    } else {
        move = "NONE";
        printf("Player move from %d is invalid.\n", client_id);
    }
    
    // Evaluate the move
    if (strstr(move, "EVEN") && (dice[0] + dice[1]) % 2 == 0 && dice[0] != dice[1]) {
        pass = true;
        printf("Player %d has passed from even roll.\n", client_id);
    }

    else if (strstr(move, "ODD") && (dice[0] + dice[1]) % 2 != 0 && dice[0] + dice[1] > 5) {
        pass = true;
        printf("Player %d has passed from odd roll.\n", client_id);
    }

    else if (strstr(move, "DOUB") && dice[0] == dice[1]) {
        pass = true;
        printf("Player %d has passed from double roll.\n", client_id);

    } 

    else if (strstr(move, "CON")) {
        int guess;
        token = strtok(NULL, ",");
        if (token != NULL) {
            guess = atoi(token);
        } else {
            printf("Player %d guessed contains empty.\n", client_id);
        }
        if (strstr(token, "0") || guess > 6){
            send_message("KICK", client_id, game);
        }
        printf("Guess is %d vs dice %d %d\n", guess, dice[0], dice[1]);
        if(dice[0] == guess || dice[1] == guess) {
            pass = true;
            printf("Player %d has passed from guessing %d.\n", client_id, guess);
        }
    }

    else if (strstr(move, "NONE")) {
        printf("No move registered.\n");
    } 

    return pass;
}

char* get_message(int client_id, struct game_session game){
    char* buf = calloc(BUFFER_SIZE, sizeof(char));
    int client_fd = game.players[client_id].client_fd;
    buf[0] = '\0';
    recv(client_fd, buf, BUFFER_SIZE, 0);
    return buf;
}