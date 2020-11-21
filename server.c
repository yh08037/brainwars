#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>

#include <string.h>
#include <pthread.h>

#define SIZE_BUFFER 256

static void usage(void) {
    fprintf(stderr, "\n"
		"usage: server [-h] [options]"
		"\n"
		"options:\n"
		"    -h  show this message\n"
		"    -p  port number of server (default: 9734)\n"
		"\n"
	);
	exit(1);
}

typedef struct _rx_arg_t {
	char*				rx_msg;
	int					server_sockfd;
	struct sockaddr_in	server_address;
} rx_arg_t;

void *receive(void *arg) {
	char 	ch;
	int 	result, fd, nread, client_len;
	fd_set 	readfds, testfds;

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
						write(fd, rx_msg, SIZE_BUFFER);
					}
				}
			}
		}
	}
	
	close(client_sockfd);
}



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
	rx_arg_t	rx_arg;
    int 		res1;
    pthread_t	a_thread1;
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


	// initialize args for rx threads
	rx_arg.rx_msg = rx_msg;
	rx_arg.server_sockfd = server_sockfd;
	rx_arg.server_address = server_address;

	// create rx threads
	res1 = pthread_create(&a_thread1, NULL, receive, (void*)&rx_arg);

	if (res1 != 0) {
		perror("Thread creation failed");
		exit(EXIT_FAILURE);
	}

	res1 = pthread_join(a_thread1, &thread_result);

	close(server_sockfd);
	exit(0);
}
