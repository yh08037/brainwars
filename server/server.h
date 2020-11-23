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

#include <string.h>
#include <pthread.h>

#include "queue.h"

#define SIZE_BUFFER 256


static void usage(void) {
    fprintf(stderr, "\n"
		"usage: server [-h] [options]"
		"\n"
		"options:\n"
		"    -h  show this message\n"
		"    -p  port number of server (default: 9734)\n"
		"\n"
	);
	exit(1);
}

fd_set readfds;
queue tx_buffer;

typedef struct _labeled_msg_t {
	int target_fd;
	char tx_msg[SIZE_BUFFER];
} labeled_msg_t;

typedef struct _server_cfg_t {
	int server_sockfd;
} server_cfg_t;

typedef struct _rx_arg_t {
	int server_sockfd;
} rx_arg_t;

void init_server(server_cfg_t *server_cfg, int port_number);
void run_server(server_cfg_t *server_cfg);
void get_msg(int *clientfd, char *tx_msg);
void *transmit(void *arg);
void *receive(void *arg);
void *process(void *arg);

#endif // SERVER_H