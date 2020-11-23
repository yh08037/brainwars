#include "server.h"


void init_server(server_cfg_t *server_cfg, int port_number) {
	
	struct sockaddr_in server_address;

	server_cfg->server_sockfd = socket(AF_INET, SOCK_STREAM, 0);

	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = htons(port_number);
	bind(server_cfg->server_sockfd, 
		 (struct sockaddr *) &server_address, 
		 sizeof(server_address)
	);

	listen(server_cfg->server_sockfd, 5);
}

void run_server(server_cfg_t *server_cfg) {
	
	int server_sockfd = server_cfg->server_sockfd;
	
	// vars for threads
	rx_arg_t	rx_arg;
    int 		res1, res2, res3;
    pthread_t	tx_thread, rx_thread, process_thread;
    void 		*thread_result;
	
	// initialize args for tx/rx threads
	rx_arg.server_sockfd = server_sockfd;

	// create rx threads
	res1 = pthread_create(&tx_thread, NULL, transmit, NULL);
	res2 = pthread_create(&rx_thread, NULL, receive, (void*)&rx_arg);
	res3 = pthread_create(&process_thread, NULL, process, NULL);

	if (res1 != 0 || res2 != 0 || res3 != 0 ) {
		perror("Thread creation failed");
		exit(EXIT_FAILURE);
	}

	res1 = pthread_join(tx_thread, &thread_result);
    res2 = pthread_join(rx_thread, &thread_result);
    res3 = pthread_join(process_thread, &thread_result);
}

void get_msg_from_tx_buffer(int *target_fd, char *tx_msg) {

	labeled_msg_t *labeled_msg;

	while (1) {
		if (!queue_empty(&tx_buffer)) {
			labeled_msg = (labeled_msg_t*)queue_front(&tx_buffer);

			strcpy(tx_msg, labeled_msg->tx_msg);
			*target_fd = labeled_msg->target_fd;
			
			queue_dequeue(&tx_buffer);
			break;
		}
		sleep(T_BUFFER_CHECK);
	}
}

void push_msg_to_tx_buffer(int target_fd, char *tx_msg) {

	labeled_msg_t *labeled_msg;

	labeled_msg = (labeled_msg_t*)malloc(sizeof(labeled_msg_t));

	strcpy(labeled_msg->tx_msg, tx_msg);
	labeled_msg->target_fd = target_fd;

	queue_enqueue(&tx_buffer, labeled_msg);
}

void *transmit(void *arg) {
	
	// tx message buffer
	char tx_msg[SIZE_BUFFER];

	int target_fd = 4;

	while(1) {
		get_msg_from_tx_buffer(&target_fd, tx_msg);
		if (FD_ISSET(target_fd, &readfds)) {
			write(target_fd, tx_msg, SIZE_BUFFER);
		}
		else {
			printf("fd %d didn't set\n", target_fd);
		}
	}
}

void *receive(void *arg) {
	
	// rx message buffer
	char rx_msg[SIZE_BUFFER];

	int 	result, fd, nread, client_len;
	fd_set 	testfds;

	int 				server_sockfd, client_sockfd;
	struct sockaddr_in	client_address;
	
	rx_arg_t *rx_arg = (rx_arg_t*) arg;

	server_sockfd = rx_arg->server_sockfd;

	
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

void *process(void *arg) {
	
	char input[SIZE_BUFFER];
	int fd;

	while (1) {
		scanf("%d-%s", &fd, input);
		push_msg_to_tx_buffer(fd, input);	
	}
}