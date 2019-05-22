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

#define MSGSIZE 12
#define BUFFER_SIZE 1024
#define MAX_PLAYERS 4
#define MAX_ROUNDS 10
#define PLAYER_LIVES 5

struct player {
    int client_fd;
    int player_num;
    int player_lives;
};

struct game_session {
    struct player *players;
    int player_number;
    int rounds;
};

struct game_session init_game();
int* diceroll();
void send_message(char* message, int client_id, struct game_session game);
char* eval_move(char* move, int* dice, int client_id, struct game_session game);
