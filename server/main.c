#include "server.h"


int main(int argc, char* argv[])
{
	int                     server_sockfd;
	int                     server_len;
	struct sockaddr_in      server_address;
	
	int						opt;
	int						port_number = 9734;

	char                    rx_msg[SIZE_BUFFER];
	char                    tx_msg[SIZE_BUFFER];
	
	// vars for threads
	tx_arg_t	tx_arg;
	rx_arg_t	rx_arg;
    int 		res1, res2;
    pthread_t	a_thread1, a_thread2;
    void 		*thread_result;
	

	while ((opt = getopt(argc, argv, "hp:")) != -1) {
        switch (opt) {
        case 'h':
            usage();
            break;
		case 'p':
			port_number = atoi(strdup(optarg));
			break;
        default:
            break;
        }
    }	

	server_sockfd = socket(AF_INET, SOCK_STREAM, 0);

	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = htons(port_number);
	server_len = sizeof(server_address);
	bind(server_sockfd, (struct sockaddr *)&server_address, server_len);

	listen(server_sockfd, 5);


	// initialize args for tx/rx threads
	tx_arg.tx_msg = tx_msg;
	rx_arg.rx_msg = rx_msg;
	rx_arg.server_sockfd = server_sockfd;
	rx_arg.server_address = server_address;

	// create rx threads
	res1 = pthread_create(&a_thread1, NULL, transmit, (void*)&tx_arg);
	res2 = pthread_create(&a_thread2, NULL, receive, (void*)&rx_arg);

	if (res1 != 0) {
		perror("Thread creation failed");
		exit(EXIT_FAILURE);
	}
	if (res2 != 0) {
		perror("Thread creation failed");
		exit(EXIT_FAILURE);
	}

	res1 = pthread_join(a_thread1, &thread_result);
	res2 = pthread_join(a_thread2, &thread_result);

	close(server_sockfd);
	exit(0);
}
