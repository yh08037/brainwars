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
	int                     server_sockfd, client_sockfd;
	int                     server_len, client_len;
	struct sockaddr_in      server_address;
	struct sockaddr_in      client_address;
	
	char                    msg[SIZE_BUFFER];

	unlink("server_socket");
	server_sockfd = socket(AF_INET, SOCK_STREAM, 0);

	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = htons(9734);
	server_len = sizeof(server_address);
	bind(server_sockfd, (struct sockaddr *)&server_address, server_len);

	listen(server_sockfd, 5);

	client_len = sizeof(client_address);
	client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_address, &client_len);
	
	while(1) {
		printf("server waiting\n");

		read(client_sockfd, msg, SIZE_BUFFER);
		
		if (strcmp(msg, "quit")==0) {
			printf("quit-server\n");
			break;
		}

		printf("msg from client: %s\n", msg);

		//write(client_sockfd, msg, SIZE_BUFFER);
	}

	close(client_sockfd);
}
