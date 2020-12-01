#ifndef GAME_BASIS_H
#define GAME_BASIS_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdint.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>
#include <linux/input.h>
#include <time.h>
#include <stdbool.h>
#include <sys/time.h>
#include <pthread.h>

#define PLAY_TIME 20 //sec

#define CORRECT_POINT 3
#define WRONG_POINT -2

#include "led_matrix.h"
#include "joystick.h"

typedef struct _result_t {
    int correct;
    int wrong;
} result_t;

void delay(int);
double wtime();
void game_ready(uint16_t *map);

#endif