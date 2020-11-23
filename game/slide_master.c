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
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <time.h>
#include <stdbool.h>

#define FILEPATH "/dev/fb0"
#define JOYSTICK_PATH "/dev/input/event0"
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
void arrow(uint16_t *, Direction, int);
int random_arrow(uint16_t *, int*, int*);
bool check(int, int, int);
void rotate_cww90(uint16_t *, int[], int);

int main(void){
    int fd;
    fd = open(JOYSTICK_PATH, O_RDONLY);
    struct input_event ev;

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

    bool state_isCorrect = true;
    int direction_v = 0, color_v = 0;
    int *direction, *color;
    direction = &direction_v;
    color = &color_v;
    
    while(1)
    {
        if (state_isCorrect == true){
            random_arrow(p, direction, color);
            state_isCorrect = false;
            continue;
        }
        if(read(fd, &ev, sizeof(struct input_event)) < 0)
        {
            if(errno == EINTR)
                continue;
 
            break;
        }
 
        if(EV_KEY != ev.type && ev.value != 1)
            continue;

        
        state_isCorrect = check(*direction, *color, ev.code);
    }


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
    int color_random = rand() % 2;

    *direction = direction_random;
    *color = color_random;

    if (color_random == 0) color_random = RGB565_RED;
    else  color_random = RGB565_BLUE;

    arrow(p, direction_random, color_random);
}

void arrow(uint16_t *p, Direction direction, int color){
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
        0,0,0,0,0,0,0,0,
        0,0,1,0,0,0,0,0,
        0,1,1,0,0,0,0,0,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        0,1,1,0,0,0,0,0,
        0,0,1,0,0,0,0,0,
        0,0,0,0,0,0,0,0, },
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
        0,0,0,1,1,0,0,0,
        0,0,0,1,1,0,0,0,
        0,0,0,1,1,0,0,0,
        0,0,0,1,1,0,0,0,
        0,0,0,1,1,0,0,0,
        0,1,1,1,1,1,1,0,
        0,0,1,1,1,1,0,0,
        0,0,0,1,1,0,0,0, },

    };

    rotate_cww90(p, arrow_shape[direction], color);
}

bool check(int answer_direction, int answer_color, int direction){
    printf("%d, %d, %d\n", answer_direction, answer_color, direction);
    int answer_sheet[2][4] = {{106, 103, 108, 105}, {105, 108, 103, 106}};

    if (answer_sheet[answer_color][answer_direction] == direction) return true;
    else return false;

}

void rotate_cww90(uint16_t *p, int shape[], int color){
    memset(p, 0, FILESIZE);

    // for (int i = 0; i < 64; i++){
    //     if (i % 8 == 0){
    //         printf("\n");
    //     }
    //     printf("%d ", shape[i]);
    // }

    int rotate[64] ={
        7, 15, 23, 31, 39, 47, 55, 63, 
        6, 14, 22, 30, 38, 46, 54, 62, 
        5, 13, 21, 29, 37, 45, 53, 61, 
        4, 12, 20, 28, 36, 44, 52, 60, 
        3, 11, 19, 27, 35, 43, 51, 59, 
        2, 10, 18, 26, 34, 42, 50, 58, 
        1, 9, 17, 25, 33, 41, 49, 57, 
        0, 8, 16, 24, 32, 40, 48, 56, 
    };


    // printf("\n");
    // for (int i = 0; i < 64; i++){
    //     if (i % 8 == 0){
    //         printf("\n");
    //     }
    //     printf("%d ", shape[rotate[i]]);
    // }
    // printf("\n");

    for (int i = 0; i < NUM_WORDS; i++){
        if (shape[rotate[i]] == 1)    *(p+i) = color;
    }

}