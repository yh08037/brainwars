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

void *transmit(void *arg) {
	client_arg_t *tx_arg = (client_arg_t*) arg;

	char *tx_msg = tx_arg->msg;
	int sockfd = tx_arg->sockfd;

	while (1) {
		scanf("%s", tx_msg);
		write(sockfd, tx_msg, SIZE_BUFFER);
		
		// move to command interpreter in future
		if (strcmp(tx_msg, "quit")==0) {
			printf("quit-tx_thread\n");
			break;
		}
	}
}

void *receive(void *arg) {
	client_arg_t *rx_arg = (client_arg_t*) arg;

	char *rx_msg = rx_arg->msg;
	int sockfd = rx_arg->sockfd;

	while (1) {
		read(sockfd, rx_msg, SIZE_BUFFER);
		printf("msg from server: %s\n", rx_msg);

		// move to command interpreter in future
		if (strcmp(rx_msg, "quit")==0) {
			printf("quit-rx_thread\n");
			break;
		}
	}
}


int main(int argc, char* argv[])
{
	int                     sockfd, opt, len, result;
	struct sockaddr_in      address;
	char                    tx_msg[SIZE_BUFFER];
	char                    rx_msg[SIZE_BUFFER];

	char*					ipv4_address = "155.230.107.136";
	int						port_number = 9734;

	// vars for threads
	client_arg_t	tx_arg, rx_arg;
    int 			res1, res2;
    pthread_t		a_thread1, a_thread2;
    void 			*thread_result;
	

    while ((opt = getopt(argc, argv, "hd:p:")) != -1) {
        switch (opt) {
        case 'h':
            usage();
            break;
        case 'd':
            ipv4_address = strdup(optarg);
			break;
		case 'p':
			port_number = atoi(strdup(optarg));
			break;
        default:
            break;
        }
    }	

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

    address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr(ipv4_address);
	address.sin_port = htons(port_number);

    len = sizeof(address);
	result = connect(sockfd, (struct sockaddr *)&address, len);

	if (result == -1) {
		perror("oops: client connect failed");
		exit(1);
	}

	// initialize args for tx/rx threads
	tx_arg.msg = tx_msg;
	tx_arg.sockfd = sockfd;

	rx_arg.msg = rx_msg;
	rx_arg.sockfd = sockfd;

	// create tx/rx threads
	// FIXME : these threads does not have terminate condition of infinite loop
	res1 = pthread_create(&a_thread1, NULL, transmit, (void*)&tx_arg);
	res2 = pthread_create(&a_thread2, NULL, receive, (void*)&rx_arg);

	if (res1 != 0) {
		perror("Thread creation failed");
		exit(EXIT_FAILURE);
	}
	if (res2 != 0) {
		perror("Thread creation failed");
		exit(EXIT_FAILURE);
	}

	res1 = pthread_join(a_thread1, &thread_result);
    res2 = pthread_join(a_thread2, &thread_result);

	close(sockfd);
	exit(0);
}
