#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdio.h>

typedef enum _msg_type_t {
	MSG_SELECT = 0,     // server -> client
	MSG_READY,          // client -> server
	MSG_START,          // server -> client
	MSG_FINISH,         // client -> server
	MSG_RESULT          // server -> client
} msg_type_t;

// typedef union _msg_data_t {
//     int game_select;    // MSG_SELECT (0: slide master, 1: high or low, 2: rainfall, 3: color switch)
//     int score;          // MSG_FINISH (game score: int)
//     int result;         // MSG_RESULT (0: lose, 1: win, 2: draw)
// } msg_data_t;

typedef struct _msg_t {
    msg_type_t type;
    int data; //msg_data_t
} msg_t;

void print_msg(msg_t *msg);

#endif // MESSAGE_H