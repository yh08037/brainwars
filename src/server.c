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
	
	// initialize tx_buffer / rx_buffer / serving_fd
	queue_init(&tx_buffer);
	queue_init(&rx_buffer);
	queue_init(&serving_fd);

	tx_semaphore = 1;
	game = 0;
	both_ready = 0;
	state = WF_USER;
}

void run_server(server_cfg_t *server_cfg) {
	
	int server_sockfd = server_cfg->server_sockfd;
	
	// vars for threads
	rx_arg_t	rx_arg;
    int 		res1, res2, res3, res4;
    pthread_t	tx_thread, rx_thread, process_thread, web_thread;
    void 		*thread_result;
	
	// initialize args for tx/rx threads
	rx_arg.server_sockfd = server_sockfd;

	// create rx threads
	res1 = pthread_create(&tx_thread, NULL, transmit, NULL);
	res2 = pthread_create(&rx_thread, NULL, receive, (void*)&rx_arg);
	res3 = pthread_create(&process_thread, NULL, process, NULL);
	res4 = pthread_create(&web_thread, NULL, serve_forever, (void*)"12913");

	if (res1 != 0 || res2 != 0 || res3 != 0 || res4 != 0 ) {
		perror("Thread creation failed");
		exit(EXIT_FAILURE);
	}

	res1 = pthread_join(tx_thread, &thread_result);
    res2 = pthread_join(rx_thread, &thread_result);
    res3 = pthread_join(process_thread, &thread_result);
    res4 = pthread_join(web_thread, &thread_result);
}

void get_msg_from_buffer(txrx_t txrx, int *target_fd, msg_t *msg) {

	queue *buffer;
	labeled_msg_t *labeled_msg;
	int condition;

	switch (txrx) {
		case TX: buffer = &tx_buffer; break;
		case RX: buffer = &rx_buffer; break;
		default: exit(2);
	}

	while (1) {
		if (tx_semaphore && !queue_empty(buffer)) {
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

	int		*temp;
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
					
					create_player(client_sockfd);
					num_client++;
				}
				else {
					ioctl(fd, FIONREAD, &nread);

					if (nread == 0) {
						close(fd);
						FD_CLR(fd, &readfds);
						printf("removing client on fd %d\n", fd);
						
						delete_player(fd);
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
	int fd;
	char temp;
	player_info_t *player;

	while (1) {
		switch (state)
		{
		case WF_USER:
			if (num_client < NUM_PLAYER) {
				printf("waiting for user...\n");
				while (1) {
					if (num_client >= NUM_PLAYER) {
						break;
					}
					sleep(1);
				}
			}
			game = 0;
			both_ready = 0;
			state = IP_SELECT;
			break;

		case IP_SELECT:
			printf("waiting for select input...\n");

			// wait for browser input
			while (0 >= game) {
				// printf("s: %d\n", game);
				sleep(1);
 			}

			tx_msg.type = MSG_SELECT;
			tx_msg.data = game;

			broadcast(&tx_msg);
			
			printf("Selected game %d\n", game);
			state = WF_READY;
			break;

		case WF_READY:
			printf("waiting for game ready...\n");
			
			// need to fix!!!!!!
			for (int i = 1; i <= NUM_PLAYER; i++) {
				get_msg_from_buffer(RX, &fd, &rx_msg);
				if (i != NUM_PLAYER)
					sleep(1);
			}

			both_ready = 1;

			tx_msg.type = MSG_START;
			tx_msg.data = 0;
			
			broadcast(&tx_msg);

			state = IN_GAME;
			break;

		case IN_GAME:
			printf("now gaming...\n");

			// move to next page : ingame !!!

			// need to fix!!!!!!
			for (int i = 0; i < NUM_PLAYER; i++) {
				get_msg_from_buffer(RX, &fd, &rx_msg);
				player = search_player(fd);
				player->score = rx_msg.data;
			}

			eval_player();

			send_result_to_all_player();

			state = DP_RESULT;
			break;

		case DP_RESULT:
			print_result_player();
			
			// move to next page : result !!!

			printf("press any key to restart\n");
			scanf("%c", &temp);

			init_player();
			
			state = WF_USER;
			break;

		default:
			break;
		}
		printf("\n");
		sleep(0.1);
	}
}

void broadcast(msg_t *tx_msg) {
	
	int fd;
	node *curr; 
	player_info_t *player;

	tx_semaphore = 0;

	curr = serving_fd.front;
	while (curr != NULL) {
		player = (player_info_t*)(curr->data);
		fd = player->fd;
		push_msg_to_buffer(TX, fd, tx_msg);
		curr = curr->next;
	}

	tx_semaphore = 1;
}

void create_player(int client_sockfd) {
	
	player_info_t *player_info;
	player_info = (player_info_t*)calloc(1, sizeof(player_info_t));

	player_info->fd = client_sockfd;
	
	queue_enqueue(&serving_fd, player_info);
}

void delete_player(int client_sockfd) {

	player_info_t temp_player;

	temp_player.fd = client_sockfd;

	queue_delete(&serving_fd, &temp_player, cmp_player);
}

player_info_t *search_player(int client_sockfd) {
	
	void *result;
	player_info_t temp_player;

	temp_player.fd = client_sockfd;

	result = queue_search(&serving_fd, &temp_player, cmp_player);

	return (player_info_t*)result;
}

int cmp_player(void *a, void *b) {
	// compare by fd value
    
    int val_a = ((player_info_t*)a)->fd;
    int val_b = ((player_info_t*)b)->fd;
    int result = 0;

    if (val_a == val_b) result = 0;
    else if (val_a > val_b) result = 1;
    else result = -1;

    return result;   
}

void eval_player() {
	
	int max, temp, flag;
	node *curr;
	player_info_t *player, *first;

	max = 1000;
	for (int i = 1; i <= NUM_PLAYER; i++) {
		temp = -1000;
		curr = serving_fd.front;
		while (curr != NULL) {
			player = (player_info_t*)(curr->data);
			if (player->score < max) {
				player->result++;
				if (player->score > temp)
					temp = player->score;
			}
			curr = curr->next;
		}
		max = temp;
	}

	for (int i = 1; i <= NUM_PLAYER; i++) {
		first = NULL;
		flag = 0;
		curr = serving_fd.front;
		while (curr != NULL) {
			player = (player_info_t*)(curr->data);
			if (player->result == i) {
				if (first == NULL) {
					first = player;
				}
				else {
					player->result *= -1;
					flag = 1;
				}
			}
			curr = curr->next;
		}
		if (flag)
			first->result *= -1;
	}
}

void send_result_to_all_player() {
	
	msg_t tx_msg; 						
	node *curr;
	player_info_t *player;
	int fd;

	tx_msg.type = MSG_RESULT;
	curr = serving_fd.front;
	while (curr != NULL) {
		player = (player_info_t*)(curr->data);
		tx_msg.data = player->result;
		fd = player->fd;
		push_msg_to_buffer(TX, fd, &tx_msg);
		curr = curr->next;
	}
}

void init_player() {

	node *curr;
	player_info_t *player;

	curr = serving_fd.front;
	while (curr != NULL) {
		player = (player_info_t*)(curr->data);
		player->score = 0;
		player->result = 0;
		curr = curr->next;
	}
}

void print_result_player() {

	node *curr = serving_fd.front;
	player_info_t *player;
	int fd, result, score, n;
	char temp[32];

	printf("Result :\n");
	result_buffer[0] = '\0';

	n = 1;

	while (curr != NULL) {
		player = (player_info_t*)(curr->data);
		fd = player->fd;
		result = player->result;
		score = player->score;
		
		if (result == -1)
			sprintf(temp, "User %d Draw - %d<br>", n, score);
		else if (result == 1)
			sprintf(temp, "User %d Win - %d<br>", n, score);
		else if (result == 2)
			sprintf(temp, "User %d Lose - %d<br>", n, score);
		curr = curr->next;
		strcat(result_buffer, temp);

		n++;
	}
	strcat(result_buffer, "\n");
	printf("%s\n", result_buffer);
}

void print_player() {

	node *curr = serving_fd.front;
	player_info_t *player;
	int fd, score, result;

	while (curr != NULL) {
		player = (player_info_t*)(curr->data);
		fd = player->fd;
		score = player->score;
		result = player->result;
		printf("fd%d %d %d\n", fd, score, result);

		curr = curr->next;
	}
	printf("\n");
}