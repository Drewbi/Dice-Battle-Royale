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

int* diceroll() {
    int *result;
    result = (int *) malloc(sizeof(int) * 2);
    result[0] = rand() % 6 + 1;
    result[1] = rand() % 6 + 1; 
    return result;
}

void send_message(char* message, int client_id, struct game_session game) {
    char* response = calloc(BUFFER_SIZE, sizeof(char));
    int client_fd = game.players[client_id].client_fd;

    if (strstr(message, "WELCOME")) {
        response[0] = '\0';
        sprintf(response, "WELCOME,%d", client_id);
        send(client_fd, response, strlen(response), 0);
        printf("Player %d has joined.\n", client_id);
    }

    else if (strstr(message, "REJECT")) {
        response[0] = '\0';
        sprintf(response, "REJECT");
        send(client_fd, response, strlen(response), 0);
        printf("Rejecting player...\n");
    }

    else if(strstr(message, "START")) {
        response[0] = '\0';
        sprintf(response, "START,%d,%d", game.player_number, PLAYER_LIVES);
        send(client_fd, response, strlen(response), 0);
        printf("Starting game for player %d.\n", client_id);
    }

    else if (strstr(message, "CANCEL")) {
        response[0] = '\0';
        sprintf(response, "CANCEL");
        send(client_fd, response, strlen(response), 0);
        printf("Not enough players, canceling...\n", client_id);
    }

    else if (strstr(message, "PASS")) {
        response[0] = '\0';
        sprintf(response, "%d,PASS", client_id);
        send(client_fd, response, strlen(response), 0);
        printf("Player %d has passed.\n", client_id);
    }

    else if (strstr(message, "FAIL")) {
        response[0] = '\0';
        sprintf(response, "%d,FAIL", client_id);
        send(client_fd, response, strlen(response), 0);
        printf("Player %d has failed.\n", client_id);
    }

    else if (strstr(message, "ELIM")) {
        response[0] = '\0';
        sprintf(response, "%d,ELIM", client_id);
        send(client_fd, response, strlen(response), 0);
        printf("Player %d has been eliminated.\n", client_id);
    }

    else if (strstr(message, "VICT")) {
        response[0] = '\0';
        sprintf(response, "%d,VICT", client_id);
        send(client_fd, response, strlen(response), 0);
        printf("Player %d is victorious.\n", client_id);
    }

    else if (strstr(message, "KICK")) {
        response[0] = '\0';
        sprintf(response, "%d,KICK", client_id);
        send(client_fd, response, strlen(response), 0);
        printf("Player %d has been kicked for cheating.\n", client_id);
    }

    free(response);
}


char* eval_move(char* message, int* dice, int client_id, struct game_session game) {
    bool pass = false;

    // Get incoming player id
    int player_id;
    char * token = strtok(message, ',');
    if (token != NULL) {
        player_id = atoi(token);
    } else {
        player_id = -1;
    }
    if (player_id == 0 || player_id != client_id){
        printf("Player id from %d is invalid.\n", client_id);
    }

    // Get incoming move
    char* move;
    token = strtok(NULL, ',');
    if (token != NULL) {
        move = strdup(token);
    } else {
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
        token = strtok(NULL, ',');
        if (token != NULL) {
            guess = atoi(token);
        } else {
            printf("Player %d guessed contains empty.\n", client_id);
        }
        if (guess == 0 || guess > 6){
            // If player submits string instead of number they will be unfortunately kicked with the cheaters
            send_message("KICK", client_id, game);
        }
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

