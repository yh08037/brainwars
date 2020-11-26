#include "client.h"
#include <string.h>


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
	int		opt;
	char*	ipv4_address = "155.230.107.136";
	int		port_number = 9734;

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

	client_cfg_t client_cfg;
	
	init_client(&client_cfg, ipv4_address, port_number);
	run_client(&client_cfg);
	
	close(client_cfg.sockfd);
	exit(0);
}
