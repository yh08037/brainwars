#include "message.h"

void print_msg(msg_t *msg) {
	switch (msg->type)
	{
	case MSG_SELECT:
		printf("MSG_SELECT - game: %d\n", msg->data);
		break;
	case MSG_READY:
		printf("MSG_READY\n");
		break;
	case MSG_START:
		printf("MSG_START\n");
		break;
	case MSG_FINISH:
		printf("MSG_FINISH - score: %d\n", msg->data);
		break;
	case MSG_RESULT:
		switch (msg->data) {
			case 0: printf("MSG_RESULT - win\n"); break;
			case 1: printf("MSG_RESULT - lose\n"); break;
			case 2: printf("MSG_RESULT - draw\n"); break;
			default: printf("MSG_RESULT - ?\n");
		}
		break;
	default:
		printf("Unknown msg type\n");
	}
}