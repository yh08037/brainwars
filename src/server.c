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
	
	// initialize tx_buffer / rx_buffer 
	queue_init(&tx_buffer);
	queue_init(&rx_buffer);	
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

void get_msg_from_buffer(txrx_t txrx, int *target_fd, msg_t *msg) {

	queue *buffer;
	labeled_msg_t *labeled_msg;

	switch (txrx) {
		case TX: buffer = &tx_buffer; break;
		case RX: buffer = &rx_buffer; break;
		default: exit(2);
	}

	while (1) {
		if (!queue_empty(buffer)) {
			labeled_msg = (labeled_msg_t*)queue_front(buffer);
			break;
		}
		sleep(T_BUFFER_CHECK);
	}
	*msg = labeled_msg->msg;
	*target_fd = labeled_msg->target_fd;
	queue_dequeue(buffer);

	if (txrx == RX) {
		printf("received from fd%d: ", *target_fd);
		print_msg(msg);
	}
}

void push_msg_to_buffer(txrx_t txrx, int target_fd, msg_t *msg) {

	queue *buffer;
	labeled_msg_t *labeled_msg;

	switch (txrx) {
		case TX: buffer = &tx_buffer; break;
		case RX: buffer = &rx_buffer; break;
		default: exit(2);
	}

	labeled_msg = (labeled_msg_t*)malloc(sizeof(labeled_msg_t));

	labeled_msg->msg = *msg;
	labeled_msg->target_fd = target_fd;

	queue_enqueue(buffer, labeled_msg);

	if (txrx == TX) {	
		printf("transmitted to fd%d: ", target_fd);
		print_msg(msg);
	}
}

void *transmit(void *arg) {
	
	// tx message buffer
	msg_t tx_msg;
	int target_fd = 4;

	while(1) {
		get_msg_from_buffer(TX, &target_fd, &tx_msg);
				
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
					num_client++;
				}
				else {
					ioctl(fd, FIONREAD, &nread);

					if (nread == 0) {
						close(fd);
						FD_CLR(fd, &readfds);
						printf("removing client on fd %d\n", fd);
						num_client--;
					}
					else {
						read(fd, &rx_msg, SIZE_BUFFER);
						push_msg_to_buffer(RX, fd, &rx_msg);
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
	server_state_t state = WF_USER;
	int game, score1, score2, result1, result2;
	int fd1 = 4, fd2 = 5;
	char temp;

	while (1) {
		switch (state)
		{
		case WF_USER:
			if (num_client < 2) {
				printf("waiting for user...\n");
				while (1) {
					if (num_client >= 2) {
						break;
					}
					sleep(1);
				}
			}
			state = IP_SELECT;
			break;

		case IP_SELECT:
			printf("waiting for select input...\n");
			scanf("%d", &game);
			getchar();

			tx_msg.type = MSG_SELECT;
			tx_msg.data = game;
			push_msg_to_buffer(TX, fd1, &tx_msg);
			sleep(1);
			push_msg_to_buffer(TX, fd2, &tx_msg);
			
			printf("Selected game %d\n", game);
			state = WF_READY;
			break;

		case WF_READY:
			printf("waiting for game ready...\n");
			
			// need to fix!!!!!!
			get_msg_from_buffer(RX, &fd1, &rx_msg);
			get_msg_from_buffer(RX, &fd2, &rx_msg);

			tx_msg.type = MSG_START;
			tx_msg.data = 0;
			push_msg_to_buffer(TX, fd1, &tx_msg);
			push_msg_to_buffer(TX, fd2, &tx_msg);

			state = IN_GAME;
			break;

		case IN_GAME:
			printf("now gaming...\n");

			// need to fix!!!!!!
			get_msg_from_buffer(RX, &fd1, &rx_msg);
			score1 = rx_msg.data;
			get_msg_from_buffer(RX, &fd2, &rx_msg);
			score2 = rx_msg.data;

			result1 = (score1>score2) ? 0 : ((score1<score2) ? 1 : 2);
			result2 = (score1<score2) ? 0 : ((score1>score2) ? 1 : 2);

			tx_msg.type = MSG_RESULT;
			tx_msg.data = result1;
			push_msg_to_buffer(TX, fd1, &tx_msg);
			tx_msg.data = result2;
			push_msg_to_buffer(TX, fd2, &tx_msg);

			state = DP_RESULT;
			break;

		case DP_RESULT:
			switch (result1) {
				case 0: printf("u1: lose   u2: win\n"); break;
				case 1: printf("u1: win    u2: lose\n"); break;
				case 2: printf("draw\n"); break;
				default: printf("???\n");
			}
			printf("press any key to restart\n");
			scanf("%c", &temp);
			state = WF_USER;

		default:
			break;
		}
		printf("\n");
		sleep(0.1);
	}
}