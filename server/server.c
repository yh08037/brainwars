#include "server.h"


void *transmit(void *arg) {
	int target_fd;
	char *tx_msg;

	tx_arg_t *tx_arg = (tx_arg_t*) arg;
	tx_msg = tx_arg->tx_msg;

	while(1) {
		scanf("%d\n", &target_fd);

		if (FD_ISSET(target_fd, &readfds)) {
			scanf("%s", tx_msg);
			write(target_fd, tx_msg, SIZE_BUFFER);
		}
		else {
			printf("fd %d didn't set\n", target_fd);
		}
	}
}

void *receive(void *arg) {
	char 	ch;
	int 	result, fd, nread, client_len;
	fd_set 	testfds;

	char*				rx_msg;
	int 				server_sockfd, client_sockfd;
	struct sockaddr_in	server_address, client_address;

	rx_arg_t *rx_arg = (rx_arg_t*) arg;

	rx_msg = rx_arg->rx_msg;
	server_sockfd = rx_arg->server_sockfd;
	server_address = rx_arg->server_address;

	
	FD_ZERO(&readfds);
	FD_SET(server_sockfd, &readfds);

	while(1) {

		testfds = readfds;

		printf("server waiting\n");
		result = select(FD_SETSIZE, &testfds, (fd_set *)0, (fd_set *)0, (struct timeval *)0);

		if (result < 1) {
			perror("error: select");
			exit(1);
		}

		for (fd = 0; fd < FD_SETSIZE; fd++) {
			if (FD_ISSET(fd, &testfds)) {
				if (fd == server_sockfd) {
					client_len = sizeof(client_address);
					client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_address, &client_len);
					FD_SET(client_sockfd, &readfds);
					printf("adding client on fd %d\n", client_sockfd);
				}
				else {
					ioctl(fd, FIONREAD, &nread);

					if (nread == 0) {
						close(fd);
						FD_CLR(fd, &readfds);
						printf("removing client on fd %d\n", fd);
					}
					else {
						read(fd, rx_msg, SIZE_BUFFER);
						printf("serving client on fd %d\n", fd);
						printf("msg from client on fd %d: %s\n", fd, rx_msg);
						// sleep(1);	// it will make server more secure

						// echo response - for test
						// write(fd, rx_msg, SIZE_BUFFER);
					}
				}
			}
		}

		printf("\n");
	}
	
	close(client_sockfd);
}
