#ifndef CLIENT_H
#define CLIENT_H

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

#include <pthread.h>

#include "queue.h"
#include "message.h"

#include "slide_master.h"
#include "high_or_low.h"
#include "rainfall.h"
#include "color_switch.h"


#define SIZE_BUFFER		sizeof(msg_t)
#define T_BUFFER_CHECK	0.1
#define T_GAME			10


typedef struct _client_cfg_t {
	int sockfd;
} client_cfg_t;

typedef struct _client_arg_t {
	int sockfd;
} client_arg_t;

typedef enum _client_state_t {
	WF_SELECT = 0,
	IP_READY,
	WF_START,
	IN_GAME,
	WF_RESULT,
	DP_RESULT
} client_state_t;

typedef enum _txrx_t{
	TX = 0,
	RX,
} txrx_t;

queue 			tx_buffer, rx_buffer;
led_matrix_t 	led_matrix;
result_t 		game_result;

void init_client(client_cfg_t *client_cfg, char *ipv4_address, int port_number);
void run_client(client_cfg_t *client_cfg);

void get_msg_from_buffer(txrx_t txrx, msg_t *msg);	// blocking
void push_msg_to_buffer(txrx_t txrx, msg_t *msg);	// non-blocking

void *transmit(void *arg);
void *receive(void *arg);
void *process(void *arg);

#endif // CLIENT_H