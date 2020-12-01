#ifndef LED_MATRIX_H
#define LED_MATRIX_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdint.h>
#include <string.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <stdbool.h>

#define LEDMATRIX_PATH "/dev/fb0"
#define NUM_WORDS 64
#define FILESIZE (NUM_WORDS * sizeof(uint16_t))


#define RGB565_RED 0xF800
#define RGB565_GREEN 0x07E0
#define RGB565_BLUE 0x001F
#define RGB565_WEAKWHITE 0x7BEF
#define RGB565_WEAKGREEN 0x01E0
#define RGB565_WHITE 0xFFFF
#define RGB565_OFF 0x0000

typedef struct {
    int fbfd;
    uint16_t *map;
    struct fb_fix_screeninfo fix_info;
} led_matrix_t;

typedef struct {
    unsigned int B : 5;
    unsigned int G : 6;
    unsigned int R : 5;
} RGB565_t;


void open_led_matrix(led_matrix_t *led_matrix);
void close_led_matrix(led_matrix_t *led_matrix);
void disp_nums(uint16_t *map, int score, int color_code);
void disp_num(uint16_t *map, int degit, int x_pos, int y_pos, int color_code);
void number_countdown(uint16_t *map, int number);
void ring(uint16_t *map, double left_time, double max_time, int color_code, bool is_overwrite);
void rotate_ccw90(uint16_t *map, int shape[], int color_code);


// void delay(int);

#endif // LED_MATRIX_H