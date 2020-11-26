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

void get_msg_from_buffer(queue* buffer, int *target_fd, msg_t *msg) {

	labeled_msg_t *labeled_msg;

	labeled_msg = (labeled_msg_t*)queue_front(buffer);

	*msg = labeled_msg->msg;
	*target_fd = labeled_msg->target_fd;
	
	queue_dequeue(buffer);
}

void push_msg_to_buffer(queue* buffer, int target_fd, msg_t *msg) {

	labeled_msg_t *labeled_msg;

	labeled_msg = (labeled_msg_t*)malloc(sizeof(labeled_msg_t));

	labeled_msg->msg = *msg;
	labeled_msg->target_fd = target_fd;

	queue_enqueue(buffer, labeled_msg);
}

void *transmit(void *arg) {
	
	// tx message buffer
	msg_t tx_msg;
	int target_fd = 4;

	while(1) {
		while (1) {
			if (!queue_empty(&tx_buffer)) {
				get_msg_from_buffer(&tx_buffer, &target_fd, &tx_msg);
				break;
			}
			sleep(T_BUFFER_CHECK);
		}
		if (FD_ISSET(target_fd, &readfds)) {
			write(target_fd, &tx_msg, SIZE_BUFFER);
		}
		else {
			printf("fd %d didn't set\n", target_fd);
		}
	}
}

void *receive(void *arg) {
	
	// rx message buffer
	msg_t rx_msg;

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
						read(fd, &rx_msg, SIZE_BUFFER);
						push_msg_to_buffer(&rx_buffer, fd, &rx_msg);
						printf("serving client on fd %d\n", fd);
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
	
	msg_t rx_msg, tx_msg;
	int fd, temp_type;

	while (1) {
		// check rx_buffer
		if (!queue_empty(&rx_buffer)) {
			get_msg_from_buffer(&rx_buffer, &fd, &rx_msg);
			printf("msg from fd %d : %d %d\n", fd, rx_msg.type, rx_msg.data);
		}

		scanf("%d-%d-%d", &fd, &temp_type, &(tx_msg.data));
		tx_msg.type = (msg_type_t)temp_type;
		push_msg_to_buffer(&tx_buffer, fd, &tx_msg);

		sleep(T_BUFFER_CHECK);
	}
}