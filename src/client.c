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

void get_msg_from_tx_buffer(msg_t *tx_msg) {

	msg_t *temp;

	while (1) {
		if (!queue_empty(&tx_buffer)) {
			temp = (msg_t*)queue_front(&tx_buffer);
			*tx_msg = *temp;
			queue_dequeue(&tx_buffer);
			break;
		}
		sleep(T_BUFFER_CHECK);
	}
}

void push_msg_to_tx_buffer(msg_t *tx_msg) {

	msg_t *temp = (msg_t*)malloc(sizeof(msg_t));
	
	*temp = *tx_msg;
	queue_enqueue(&tx_buffer, temp);
}

void get_msg_from_rx_buffer(msg_t *rx_msg) {

	msg_t *temp = (msg_t*)queue_front(&rx_buffer);

	*rx_msg = *temp;
	queue_dequeue(&rx_buffer);
}

void push_msg_to_rx_buffer(msg_t *rx_msg) {

	msg_t *temp = (msg_t*)malloc(sizeof(msg_t));

	*temp = *rx_msg;
	queue_enqueue(&rx_buffer, temp);
}

void *transmit(void *arg) {
	
	// tx message buffer
	msg_t tx_msg;

	client_arg_t *tx_arg = (client_arg_t*) arg;

	int sockfd = tx_arg->sockfd;

	while (1) {
		get_msg_from_tx_buffer(&tx_msg);
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
		push_msg_to_rx_buffer(&rx_msg);
	}
}

void *process(void *arg) {
	
	msg_t rx_msg, tx_msg;
	int temp_type;

	while (1) {
		// check rx_buffer
		if (!queue_empty(&rx_buffer)) {
			get_msg_from_rx_buffer(&rx_msg);
			printf("msg from server : %d %d\n", rx_msg.type, rx_msg.data);
		}
		
		scanf("%d-%d", &temp_type, &(tx_msg.data));
		tx_msg.type = (msg_type_t)temp_type;
		push_msg_to_tx_buffer(&tx_msg);
		
		sleep(T_BUFFER_CHECK);
	}
}