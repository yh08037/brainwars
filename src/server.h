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
#define NUM_PLAYER		2


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

typedef struct _player_info_t {
	int fd;
	int score;
	int result;
} player_info_t;

fd_set readfds;
queue tx_buffer, rx_buffer, serving_fd;
int num_client;
int tx_semaphore;

void init_server(server_cfg_t *server_cfg, int port_number);
void run_server(server_cfg_t *server_cfg);

void get_msg_from_buffer(txrx_t txrx, int *target_fd, msg_t *msg);
void push_msg_to_buffer(txrx_t txrx, int target_fd, msg_t *msg);

void *transmit(void *arg);
void *receive(void *arg);
void *process(void *arg);

void broadcast(msg_t *tx_msg);
void create_player(int client_sockfd);
void delete_player(int client_sockfd);
player_info_t *search_player(int client_sockfd);
int cmp_player(void *a, void *b);
void eval_player();
void send_result_to_all_player();
void init_player();
void print_result_player();
void print_player();

#endif // SERVER_H