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
		if (msg->data < 0)
			printf("MSG_RESULT - joint %d\n", msg->data * (-1));
		else
			printf("MSG_RESULT - %d\n", msg->data);
		break;
	default:
		printf("Unknown msg type\n");
	}
}