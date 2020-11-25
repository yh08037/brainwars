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

void get_msg_from_tx_buffer(char *tx_msg) {

	char *temp;

	while (1) {
		if (!queue_empty(&tx_buffer)) {
			temp = (char*)queue_front(&tx_buffer);
			strcpy(tx_msg, temp);
			queue_dequeue(&tx_buffer);
			break;
		}
		sleep(T_BUFFER_CHECK);
	}
}

void push_msg_to_tx_buffer(char *tx_msg) {

	char *temp = (char*)malloc(sizeof(char)*SIZE_BUFFER);

	strcpy(temp, tx_msg);
	queue_enqueue(&tx_buffer, temp);
}

void get_msg_from_rx_buffer(char *rx_msg) {

	char *temp = (char*)queue_front(&rx_buffer);

	strcpy(rx_msg, temp);
	queue_dequeue(&rx_buffer);
}

void push_msg_to_rx_buffer(char *rx_msg) {

	char *temp = (char*)malloc(sizeof(char)*SIZE_BUFFER);

	strcpy(temp, rx_msg);
	queue_enqueue(&rx_buffer, temp);
}

void *transmit(void *arg) {
	
	// tx message buffer
	char tx_msg[SIZE_BUFFER];

	client_arg_t *tx_arg = (client_arg_t*) arg;

	int sockfd = tx_arg->sockfd;

	while (1) {
		get_msg_from_tx_buffer(tx_msg);
		write(sockfd, tx_msg, SIZE_BUFFER);
		
		// move to command interpreter in future
		if (strcmp(tx_msg, "quit")==0) {
			printf("quit-tx_thread\n");
			break;
		}
	}
}

void *receive(void *arg) {

	// rx message buffer
	char rx_msg[SIZE_BUFFER];

	client_arg_t *rx_arg = (client_arg_t*) arg;

	int sockfd = rx_arg->sockfd;

	while (1) {
		read(sockfd, rx_msg, SIZE_BUFFER);
		// printf("msg from server: %s\n", rx_msg);
		push_msg_to_rx_buffer(rx_msg);

		// move to command interpreter in future
		if (strcmp(rx_msg, "quit")==0) {
			printf("quit-rx_thread\n");
			break;
		}
	}
}

void *process(void *arg) {
	
	char input[SIZE_BUFFER];

	while (1) {
		// check rx_buffer
		if (!queue_empty(&rx_buffer)) {
			get_msg_from_rx_buffer(input);
			printf("msg from server : %s\n", input);
		}
		
		// scanf("%s", input);
		// push_msg_to_tx_buffer(input);
		
		sleep(T_BUFFER_CHECK);
	}
}