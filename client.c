#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

#include <string.h>

#define SIZE_BUFFER 256

static void usage(void) {
    fprintf(stderr, "\n"
		"usage: client [-h] [options]"
		"\n"
		"options:\n"
		"    -h  show this message\n"
		"    -d  ipv4 address of server (default: 155.230.107.136)\n"
		"    -p  port number of server (default: 9734)\n"
		"\n"
	);
	exit(1);
}

int main(int argc, char* argv[])
{
	int                     sockfd, opt, len, result;
	struct sockaddr_in      address;
	char                    msg[SIZE_BUFFER];

	char*					ipv4_address = "155.230.107.136";
	int						port_number = 9734;
	

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

	while (1) {
		scanf("%s", msg);
		//printf("%s\n", msg);
		write(sockfd, msg, SIZE_BUFFER);

		//read(sockfd, msg, SIZE_BUFFER);
		//printf("msg from server: %s\n", msg);
		
		if (strcmp(msg, "quit")==0) {
			printf("quit-client\n");
			break;
		}

	}

	close(sockfd);
	exit(0);
}
