#ifndef CLIENT_H
#define CLIENT_H

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

#include <string.h>
#include <pthread.h>

#define SIZE_BUFFER 256

static void usage(void) {
    fprintf(stderr, "\n"
		"usage: client [-h] [options]"
		"\n"
		"options:\n"
		"    -h  show this message\n"
		"    -d  ipv4 address of server (default: 155.230.107.136)\n"
		"    -p  port number of server (default: 9734)\n"
		"\n"
	);
	exit(1);
}

typedef struct _client_arg_t {
	char *msg;
	int sockfd;
} client_arg_t;

void *transmit(void *arg);
void *receive(void *arg);

#endif // CLIENT_H