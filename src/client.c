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

	// initialize tx_buffer / rx_buffer 
	queue_init(&tx_buffer);
	queue_init(&rx_buffer);	

	// initialize led_matrix
    open_led_matrix(&led_matrix);

	// initialize joystick
	open_joystick();

	// init random seed`
    srand(time(NULL));
}

void run_client(client_cfg_t *client_cfg) {

	int sockfd = client_cfg->sockfd;

	// vars for threads
	client_arg_t	tx_arg, rx_arg;
    int 			res1, res2, res3;
    pthread_t		tx_thread, rx_thread, process_thread;
    void 			*thread_result;
	
	// initialize args for tx/rx threads
	tx_arg.sockfd = sockfd;
	rx_arg.sockfd = sockfd;

	// create tx/rx threads
	// FIXME : these threads does not have terminate condition of infinite loop
	res1 = pthread_create(&tx_thread, NULL, transmit, (void*)&tx_arg);
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

void get_msg_from_buffer(txrx_t txrx, msg_t *msg) {

	queue *buffer;
	msg_t *temp;

	switch (txrx) {
		case TX: buffer = &tx_buffer; break;
		case RX: buffer = &rx_buffer; break;
		default: exit(2);
	}

	while (1) {
		if (!queue_empty(buffer)) {
			temp = (msg_t*)queue_front(buffer);
			break;
		}
		sleep(T_BUFFER_CHECK);
	}
	*msg = *temp;
	queue_dequeue(buffer);

	if (txrx == RX) {
		printf("received: ");
		print_msg(msg);
	}
}

void push_msg_to_buffer(txrx_t txrx, msg_t *msg) {

	queue *buffer;
	msg_t *temp = (msg_t*)malloc(sizeof(msg_t));
	
	switch (txrx) {
		case TX: buffer = &tx_buffer; break;
		case RX: buffer = &rx_buffer; break;
		default: exit(2);
	}

	*temp = *msg;
	queue_enqueue(buffer, temp);

	if (txrx == TX) {	
		printf("transmitted: ");
		print_msg(msg);
	}
}

void *transmit(void *arg) {
	
	// tx message buffer
	msg_t tx_msg;

	client_arg_t *tx_arg = (client_arg_t*) arg;

	int sockfd = tx_arg->sockfd;

	while (1) {
		get_msg_from_buffer(TX, &tx_msg);
		write(sockfd, &tx_msg, SIZE_BUFFER);
	}
}

void *receive(void *arg) {

	// rx message buffer
	msg_t rx_msg;

	client_arg_t *rx_arg = (client_arg_t*) arg;

	int sockfd = rx_arg->sockfd;

	while (1) {
		read(sockfd, &rx_msg, SIZE_BUFFER);
		push_msg_to_buffer(RX, &rx_msg);
	}
}

void *process(void *arg) {
	
	msg_t rx_msg, tx_msg;
	client_state_t state = WF_SELECT;
	int game, score, result;
	// char temp;

	while (1) {
		switch (state)
		{
		case WF_SELECT:
			printf("waiting for game select...\n");
			get_msg_from_buffer(RX, &rx_msg);

			game = rx_msg.data;
			
			printf("Selected game %d\n", game);
			state = IP_READY;
			break;

		case IP_READY:
			printf("waiting for ready input...\n");
			
			read(joystick_data.fd, &joystick_data.ev, sizeof(struct input_event));

			tx_msg.type = MSG_READY;
			tx_msg.data = 0;
			push_msg_to_buffer(TX, &tx_msg);

			state = WF_START;
			break;

		case WF_START:
			printf("waiting for game start...\n");
			get_msg_from_buffer(RX, &rx_msg);

			printf("Start game %d!!!\n", game);
			state = IN_GAME;
			break;

		case IN_GAME:
    		slide_master_game(&game_result, &led_matrix);

			score = 3 * game_result.correct - 2 * game_result.wrong;
			printf("game score: %d\n", score);

			tx_msg.type = MSG_FINISH;
			tx_msg.data = score;
			push_msg_to_buffer(TX, &tx_msg);

			state = WF_RESULT;
			break;

		case WF_RESULT:
			printf("waiting for game result...\n");
			get_msg_from_buffer(RX, &rx_msg);
			
			result = rx_msg.data;

			state = DP_RESULT;
			break;

		case DP_RESULT:
			switch (result) {
				case 0: printf("win\n"); break;
				case 1: printf("lose\n"); break;
				case 2: printf("draw\n"); break;
				default: printf("???\n");
			}
			printf("press any key to restart\n");
			read(joystick_data.fd, &joystick_data.ev, sizeof(struct input_event));
			state = WF_SELECT;

		default:
			break;
		}
		printf("\n");
		sleep(0.1);
	}
}