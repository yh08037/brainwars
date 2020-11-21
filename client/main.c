#include "client.h"


int main(int argc, char* argv[])
{
	int                     sockfd, opt, len, result;
	struct sockaddr_in      address;
	char                    tx_msg[SIZE_BUFFER];
	char                    rx_msg[SIZE_BUFFER];

	char*					ipv4_address = "155.230.107.136";
	int						port_number = 9734;

	// vars for threads
	client_arg_t	tx_arg, rx_arg;
    int 			res1, res2;
    pthread_t		a_thread1, a_thread2;
    void 			*thread_result;
	

    while ((opt = getopt(argc, argv, "hd:p:")) != -1) {
        switch (opt) {
        case 'h':
            usage();
            break;
        case 'd':
            ipv4_address = strdup(optarg);
			break;
		case 'p':
			port_number = atoi(strdup(optarg));
			break;
        default:
            break;
        }
    }	

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

    address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr(ipv4_address);
	address.sin_port = htons(port_number);

    len = sizeof(address);
	result = connect(sockfd, (struct sockaddr *)&address, len);

	if (result == -1) {
		perror("oops: client connect failed");
		exit(1);
	}

	// initialize args for tx/rx threads
	tx_arg.msg = tx_msg;
	tx_arg.sockfd = sockfd;

	rx_arg.msg = rx_msg;
	rx_arg.sockfd = sockfd;

	// create tx/rx threads
	// FIXME : these threads does not have terminate condition of infinite loop
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

	close(sockfd);
	exit(0);
}
