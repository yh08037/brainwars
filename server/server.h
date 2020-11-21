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

#define SIZE_BUFFER 256

fd_set readfds;

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

typedef struct _tx_arg_t {
	char *tx_msg;
} tx_arg_t;

void *transmit(void *arg);

typedef struct _rx_arg_t {
	char*				rx_msg;
	int					server_sockfd;
	struct sockaddr_in	server_address;
} rx_arg_t;

void *receive(void *arg);

#endif // SERVER_H