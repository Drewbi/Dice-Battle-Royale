#include "header.h"

struct game_session init_game() {
    struct player *players = calloc(4, sizeof(struct player));
    int game_rounds =  rand() % 5 + 1;
    int player_number = 0;
    struct game_session game = { players, player_number, game_rounds };

    return game;
}

//function invoked when INIT packet sent
void add_player(struct game_session game, int player_id) {
    if(game.player_number == 0) {
        game.session_players[0].client_id = player_id;
        game.session_players[0].player_lives = 3;
    }
    else {
        game.session_players[game.player_number].client_id = player_id;
        game.session_players[game.player_number].player_lives = 3;
    }
    
}


int* diceroll() {
    int *result;

    result = (int *) malloc(sizeof(int) * 2);

    result[0] = rand() % 6 + 1;
    result[1] = rand() % 6 + 1; 

    return result;
}

char* eval_move(char* move, int* dice, int player_id) {
    bool win = false;
    char* result = calloc(25, sizeof(char));

    if (strstr(move, "EVEN") && (dice[0] + dice[1]) % 2 == 0) {
        win = true;
    }

    else if (strstr(move, "ODD"), (dice[0] + dice[1]) % 2 != 0) {
        win = true;
    }

    else if (strstr(move, "DOUB"), dice[0] == dice[1]) {
        win = true;
    } 

    // else if (strstr(move, "CON") && (dice[0] == atoi(move[3]) || dice[1] == atoi(move[3]))) {
       // win = true;
    // }

    if (win) {
        sprintf(result, "%d,PASS", player_id);
    }
    else {
        sprintf(result, "%d,FAIL", player_id);
    }
    return result;
}

void send_message(char* message, int player_fd, struct game_session game) {
    char* response = calloc(BUFFER_SIZE, sizeof(char));
    
    if (strstr(message, "INIT") && game.player_number < MAX_PLAYERS) {
        response[0] = '\0';
        sprintf(response, "WELCOME,%d", player_fd);
        send(player_fd, response, strlen(response), 0);
    }
    
    else if (strstr(message, "INIT") && game.player_number == MAX_PLAYERS) {
        response[0] = '\0';
        sprintf(response, "REJECT");
        send(player_fd, response, strlen(response), 0);
    }
    
    else if(strstr(message, "START")) {
        response[0] = '\0';
        sprintf(response, "START,%d,%d", game.player_number, 3);
        send(player_fd, response, strlen(response), 0);
    }
    else if (strstr(message, "CANCEL")) {
        response[0] = '\0';
        sprintf(response, "CANCEL");
        send(player_fd, response, strlen(response), 0);
    }

}



bool game_session(struct game_session game, int player_fd) {
    char* buf = calloc(BUFFER_SIZE, sizeof(char));
    int client_read = recv(player_fd, buf, BUFFER_SIZE, 0);
    if (client_read < 0) {
        printf("Can't read from the client\n");
    }

    if (game.player_number > 0) {
        printf("Waiting for more players...\n");
    }

    buf[0] = '\0';
    sprintf(buf, "Welcome to EF Battle Royale!");
    send(player_fd, buf, strlen(buf), 0);
    sleep(5);

    buf[0] = '\0';
    recv(player_fd, buf, BUFFER_SIZE, 0);
    send_message(buf, player_fd, game);
    printf("I have recieved your message %d\n", player_fd);
    add_player(game, player_fd);
    // game.player_number++;
    printf("Player number: %d\n", game.player_number);
    sleep(12);

    if (game.player_number >= 2) {
        send_message("START", player_fd, game);

    }
    else {
        printf("Not enough players. Closing game...\n");
        send_message("CANCEL", player_fd, game);
        close(player_fd);
    }
    

    return IN_GAME;
}

