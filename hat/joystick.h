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

typedef struct {
    joystick_t joystick_data;
    int vaild;
} joystick_vaild_t;

void open_joystick();
int joystick_read_thread();
void *joystick_command(void *arg);

joystick_t joystick_data;
joystick_vaild_t joystick_vaild_data;

#endif