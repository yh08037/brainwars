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

int main(int argc, char* argv[])
{
	int                     server_sockfd, client_sockfd;
	int                     server_len, client_len;
	struct sockaddr_in      server_address;
	struct sockaddr_in      client_address;
	
	int						opt;
	int						port_number = 9734;

	int 					result;
	fd_set					readfds, testfds;

	char                    msg[SIZE_BUFFER];


	while ((opt = getopt(argc, argv, "hd:p:")) != -1) {
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

	FD_ZERO(&readfds);
	FD_SET(server_sockfd, &readfds);

	while(1) {
		char ch;
		int fd, nread;

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
						read(fd, msg, SIZE_BUFFER);
						printf("serving client on fd %d\n", fd);
						printf("msg from client on fd %d: %s\n", fd, msg);
						// sleep(1);	// it will make server more secure
					}
				}
			}
		}
	}

	close(client_sockfd);
}
