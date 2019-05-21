#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>

#define BUFFER_SIZE 1024
#define MAX_PLAYERS 4
#define IN_GAME true

struct player {
    int client_id;
    int player_lives;
};

struct game_session {
    struct player *session_players;
    int player_number;
    int rounds;
};

struct game_session init_game();
void add_player(struct game_session game, int player_id);
int* diceroll();
char* eval_move(char* move, int* dice, int player_id);
void send_message(char* message, int player_fd, struct game_session game);
bool game_session(struct game_session game, int player_fd);