#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

#include <string.h>

#define SIZE_BUFFER 256

int main()
{
	int sockfd, len, result;
	struct sockaddr_in address;
	char msg[SIZE_BUFFER];
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr("127.0.0.1");
	address.sin_port = htons(9734);

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
