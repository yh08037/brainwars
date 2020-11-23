#include "client.h"


void init_client(client_cfg_t *client_cfg, char *ipv4_address, int port_number) {

	int result;
	struct sockaddr_in address;

	client_cfg->sockfd = socket(AF_INET, SOCK_STREAM, 0);

    address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr(ipv4_address);
	address.sin_port = htons(port_number);

	result = connect(client_cfg->sockfd, 
					 (struct sockaddr *)&address, 
					 sizeof(address)
					 );

	if (result == -1) {
		perror("oops: client connect failed");
		exit(1);
	}
}

void run_client(client_cfg_t *client_cfg) {

	int sockfd = client_cfg->sockfd;

	// tx/rx message buffer
	char rx_msg[SIZE_BUFFER];
	char tx_msg[SIZE_BUFFER];

	// vars for threads
	client_arg_t	tx_arg, rx_arg;
    int 			res1, res2;
    pthread_t		tx_thread, rx_thread;
    void 			*thread_result;
	
	// initialize args for tx/rx threads
	tx_arg.msg = tx_msg;
	tx_arg.sockfd = sockfd;

	rx_arg.msg = rx_msg;
	rx_arg.sockfd = sockfd;

	// create tx/rx threads
	// FIXME : these threads does not have terminate condition of infinite loop
	res1 = pthread_create(&tx_thread, NULL, transmit, (void*)&tx_arg);
	res2 = pthread_create(&rx_thread, NULL, receive, (void*)&rx_arg);

	if (res1 != 0) {
		perror("Thread creation failed");
		exit(EXIT_FAILURE);
	}
	if (res2 != 0) {
		perror("Thread creation failed");
		exit(EXIT_FAILURE);
	}

	res1 = pthread_join(tx_thread, &thread_result);
    res2 = pthread_join(rx_thread, &thread_result);
}

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