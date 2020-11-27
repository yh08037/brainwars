/*
 *  C code to demonstrate control of the LED matrix for the
 *  Raspberry Pi Sense HAT add-on board.
 *
 *  Uses the mmap method to map the led device into memory
 *
 *  Build with:  gcc -Wall -O2 led_matrix.c -o led_matrix
 *
 *  Tested with:  Raspbian GNU/Linux 10 (buster) / Raspberry Pi 3 B+
 *
 */

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


void open_led_matrix(led_matrix_t *led_matrix);
void close_led_matrix(led_matrix_t *led_matrix);
void delay(int);

#endif // LED_MATRIX_H