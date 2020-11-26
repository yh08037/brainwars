#include "server.h"
#include <string.h>


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
	int	opt;
	int	port_number = 9734;

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

	server_cfg_t server_cfg;

	init_server(&server_cfg, port_number);
	run_server(&server_cfg);

	close(server_cfg.server_sockfd);
	exit(0);
}
