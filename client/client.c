#include "client.h"


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
