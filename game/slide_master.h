// $ gcc -o slide_master slide_master.c -lpthread
// $ ./slide_master

#ifndef SLIDE_MASTER_H
#define SLIDE_MASTER_H

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

#define LEDMATRIX_PATH "/dev/fb0"
#define JOYSTICK_PATH "/dev/input/event0"
#define NUM_WORDS 64
#define FILESIZE (NUM_WORDS * sizeof(uint16_t))

#define RGB565_RED 0xF800
#define RGB565_GREEN 0x07E0
#define RGB565_BLUE 0x001F
#define RGB565_WEAKWHITE 0x7BEF
#define RGB565_WEAKGREEN 0x01E0
#define RGB565_WHITE 0xFFFF

#define PLAY_TIME 20 //sec
#define DELAY_SYNC 50 //ms

#define CORRECT_POINT 3
#define WRONG_POINT -2

typedef struct {
    int fbfd;
    uint16_t *map;
    struct fb_fix_screeninfo fix_info;
} led_matrix_t;

typedef struct {
    int fd;
    struct input_event ev;
} joystick_t;

typedef struct {
    bool state_isCorrect;
    int *direction, direction_v;
    int *color, color_v;
} slide_master_t;

typedef struct {
    unsigned int B : 5;
    unsigned int G : 6;
    unsigned int R : 5;
} RGB565_t;

void slide_master_game(led_matrix_t *led_matrix); // call this function to start slide_master
void disp_score(uint16_t *map, int score);
void disp_num(uint16_t *map, int degit, int x_pos, int y_pos);
void slide_master_game_ready(uint16_t *map);
void number_countdown(uint16_t *map, int number);
void ring(uint16_t *map, double left_time, double max_time, int color_code, bool is_overwrite);
int joystick_read_thread();
void *joystick_command(void *arg);
void init_slide_master(slide_master_t *slide_master);
void open_led_matrix(led_matrix_t *led_matrix);
void close_led_matrix(led_matrix_t *led_matrix);
void open_joystick(joystick_t *joystick);
int random_arrow(uint16_t *map, int *direction, int *color);
void arrow(uint16_t *map, int direction, int color_code);
bool check(int answer_direction, int answer_color, int direction);
void rotate_cww90(uint16_t *map, int shape[], int color_code);
void delay(int t);
double wtime();

joystick_t joystick_data;
bool is_thread_stop;

#endif
