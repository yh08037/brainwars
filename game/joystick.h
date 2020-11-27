#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <linux/input.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define DELAY_SYNC 50 //ms
#define JOYSTICK_PATH "/dev/input/event0"

typedef struct {
    int fd;
    struct input_event ev;
} joystick_t;

int joystick_read_thread();
void *joystick_command(void *arg);

joystick_t joystick_data;
bool is_thread_stop;

#endif