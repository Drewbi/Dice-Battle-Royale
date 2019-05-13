#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

struct player {
    int client_id;
    int player_lives;
};

struct game_session {
    struct player *session_players;
    int rounds;
};

//function invoked when INIT packet sent
struct game_session setup_game() {
    struct player *players;
    
    players = calloc(4, sizeof(*players));

    for (int i = 0; i < 4; i++) {
        players[i].player_lives = rand() % 10 + 1;
        players[i].client_id = rand() % 500 + 1;
    }
    int game_rounds = rand() % 5 + 1;

    struct game_session current_game = { players, game_rounds };
    
    return current_game;

}

int* diceroll() {
    int *result;

    result = (int *) malloc(sizeof(int) * 2);

    result[0] = rand() % 6 + 1;
    result[1] = rand() % 6 + 1; 

    return result;

}
