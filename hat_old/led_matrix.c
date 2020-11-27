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

#include <time.h>

#define FILEPATH "/dev/fb0"
#define NUM_WORDS 64
#define FILESIZE (NUM_WORDS * sizeof(uint16_t))

#define RGB565_RED 0xF800
#define RGB565_GREEN 0x07E0
#define RGB565_BLUE 0x001F

typedef enum {
    North = 0,
    South,
    East,
    West
} Direction;

void delay(int);
void arrow(uint16_t *p, Direction, int);
int random_arrow(uint16_t *p, int*, int*);

int main(void)
{
    int i;
    int fbfd;
    uint16_t *map;
    uint16_t *p;
    struct fb_fix_screeninfo fix_info;

    srand(time(NULL));

    fbfd = open(FILEPATH, O_RDWR);
    if (fbfd == -1){
        perror("Error (call to 'open')");
        exit(EXIT_FAILURE);
    }

    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &fix_info) == -1){
        perror("Error (call to 'ioctl')");
        close(fbfd);
        exit(EXIT_FAILURE);
    }

    if (strcmp(fix_info.id, "RPi-Sense FB") != 0){
        printf("%s\n", "Error: RPi-Sense FB not fount");
        close(fbfd);
        exit(EXIT_FAILURE);
    }

    map = mmap(NULL, FILESIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    if (map == MAP_FAILED){
        close(fbfd);
        perror("Error mmapping the file");
        exit(EXIT_FAILURE);
    }

    p = map;

    memset(map, 0, FILESIZE);

    // init setup finished

    int direction_v = 0, color_v = 0;
    int *direction, *color;
    direction = &direction_v;
    color = &color_v;

    for (i = 0; i < 10; i++){
        random_arrow(p, direction, color);
        printf("direction: %d, color %d\n", *direction, *color);
        delay(700);
    }
    delay(700);
    
    // setup close

    memset(map, 0, FILESIZE);

    if (munmap(map, FILESIZE) == -1){
        perror("Error un-mmapping the file");
    }
    close(fbfd);
    
    return 0;

}

void delay(int t)
{
    usleep(t*1000);
}

int random_arrow(uint16_t *p, int *direction, int *color){ // p - input, direction color - output
    int direction_random = rand() % 4;
    int color_random = rand() % 3;

    *direction = direction_random;
    *color = color_random;

    if (color_random == 0) color_random = RGB565_RED;
    else if (color_random == 1) color_random = RGB565_GREEN;
    else color_random = RGB565_BLUE;

    arrow(p, direction_random, color_random);
}

void arrow(uint16_t *p, Direction direction, int color){
    memset(p, 0, FILESIZE);
    int arrow_shape[4][64] = { // North, South, East, West
    {
        0,0,0,1,1,0,0,0,
        0,0,1,1,1,1,0,0,
        0,1,1,1,1,1,1,0,
        0,0,0,1,1,0,0,0,
        0,0,0,1,1,0,0,0,
        0,0,0,1,1,0,0,0,
        0,0,0,1,1,0,0,0,
        0,0,0,1,1,0,0,0, },
    {
        0,0,0,1,1,0,0,0,
        0,0,0,1,1,0,0,0,
        0,0,0,1,1,0,0,0,
        0,0,0,1,1,0,0,0,
        0,0,0,1,1,0,0,0,
        0,1,1,1,1,1,1,0,
        0,0,1,1,1,1,0,0,
        0,0,0,1,1,0,0,0, },
    {
        0,0,0,0,0,0,0,0,
        0,0,0,0,0,1,0,0,
        0,0,0,0,0,1,1,0,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        0,0,0,0,0,1,1,0,
        0,0,0,0,0,1,0,0,
        0,0,0,0,0,0,0,0, },
    {
        0,0,0,0,0,0,0,0,
        0,0,1,0,0,0,0,0,
        0,1,1,0,0,0,0,0,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        0,1,1,0,0,0,0,0,
        0,0,1,0,0,0,0,0,
        0,0,0,0,0,0,0,0, },
    };

    for (int i = 0; i < NUM_WORDS; i++){
        if (arrow_shape[direction][i] == 1)    *(p+i) = color;
    }
}
