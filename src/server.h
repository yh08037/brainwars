#ifndef SERVER_H
#define SERVER_H

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>

#include <pthread.h>

#include "queue.h"
#include "message.h"
#include "httpd.h"

#define SIZE_BUFFER		sizeof(msg_t)
#define T_BUFFER_CHECK	0.1


typedef struct _labeled_msg_t {
	int target_fd;
	msg_t msg;
} labeled_msg_t;

typedef struct _server_cfg_t {
	int server_sockfd;
} server_cfg_t;

typedef struct _rx_arg_t {
	int server_sockfd;
} rx_arg_t;

typedef enum _server_state_t {
	WF_USER = 0,
	IP_SELECT,
	WF_READY,
	IN_GAME,
	DP_RESULT,
} server_state_t;

typedef enum _txrx_t{
	TX = 0,
	RX,
} txrx_t;

fd_set readfds;
queue tx_buffer, rx_buffer;
int num_client;

void init_server(server_cfg_t *server_cfg, int port_number);
void run_server(server_cfg_t *server_cfg);

void get_msg_from_buffer(txrx_t txrx, int *target_fd, msg_t *msg);
void push_msg_to_buffer(txrx_t txrx, int target_fd, msg_t *msg);

void *transmit(void *arg);
void *receive(void *arg);
void *process(void *arg);

#endif // SERVER_H