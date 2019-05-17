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

char** parse_move(char* move) {
    char **parsed = (char **) malloc(sizeof(char*) * 4);
    char *delim = ",";

    move = strtok(move, delim);
    for (int i = 0; i < 4; i++) {
        while(move != NULL) {
            parsed[i] = move;
        }
    }

    return parsed;
}

char* eval_move(char** move, int* dice, int player_id) {
    bool win = false;
    char* result = calloc(25, sizeof(char));

    if (strstr(move[2], "EVEN") && (dice[0] + dice[1]) % 2 == 0) {
        win = true;
    }

    else if (strstr(move[2], "ODD"), (dice[0] + dice[1]) % 2 != 0) {
        win = true;
    }

    else if (strstr(move[2], "DOUB"), dice[0] == dice[1]) {
        win = true;
    } 

    else if (strstr(move[2], "CON") && (dice[0] == atoi(move[3]) || dice[1] == atoi(move[3]))) {
       win = true;
    }

    if (win) {
        sprintf(result, "%d,PASS", player_id);
    }
    else {
        sprintf(result, "%d,FAIL", player_id);
    }
    return result;
}



