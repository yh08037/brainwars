// $ gcc -o slide_master slide_master.c
// $ ./slide_master

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

#define LEDMATRIX_PATH "/dev/fb0"
#define JOYSTICK_PATH "/dev/input/event0"
#define NUM_WORDS 64
#define FILESIZE (NUM_WORDS * sizeof(uint16_t))

#define RGB565_RED 0xF800
#define RGB565_GREEN 0x07E0
#define RGB565_BLUE 0x001F

typedef struct {
    int fbfd;
    uint16_t *map;
    struct fb_fix_screeninfo fix_info;
} Led_matrix;

typedef struct {
    int fd;
    struct input_event ev;
} Joystick;

typedef struct {
    bool state_isCorrect;
    int *direction, direction_v;
    int *color, color_v;
} Slide_master;

void slide_master_game(Led_matrix* led_matrix, Joystick *joystick, Slide_master *slide_master);
bool joystick_command(Joystick *joystick);
void init_slide_master(Slide_master* slide_master);
void open_led_matrix(Led_matrix *led_matrix);
void close_led_matrix(Led_matrix *led_matrix);
void open_joystick(Joystick *joystick);
int random_arrow(uint16_t *p, int *direction, int *color);
void arrow(uint16_t *p, int direction, int color_code);
bool check(int answer_direction, int answer_color, int direction);
void rotate_cww90(uint16_t *p, int shape[], int color_code);
void delay(int t);

int main(void){

    Led_matrix *led_matrix, led_matrix_v;
    led_matrix = &led_matrix_v;

    Joystick *joystick, joystick_v;
    joystick = &joystick_v;

    Slide_master *slide_master, slide_master_v;
    slide_master = &slide_master_v;

    srand(time(NULL));

    open_led_matrix(led_matrix);
    open_joystick(joystick);
    init_slide_master(slide_master);

    slide_master_game(led_matrix, joystick, slide_master);

    close_led_matrix(led_matrix);
    return 0;
}

void slide_master_game(Led_matrix* led_matrix, Joystick *joystick, Slide_master *slide_master){
    while(true)
    {
        if (slide_master->state_isCorrect == true){
            memset(led_matrix->map, 0, FILESIZE);
            delay(100);
            random_arrow(led_matrix->map, slide_master->direction, slide_master->color);
            slide_master->state_isCorrect = false;
            continue;
        }

        if (joystick_command(joystick)){
            slide_master->state_isCorrect = check(*slide_master->direction, *slide_master->color, joystick->ev.code);

            if (slide_master->state_isCorrect == false){
                for (int i = 0; i < 4; i++){
                    memset(led_matrix->map, 0, FILESIZE);
                    delay(100);
                    arrow(led_matrix->map, *slide_master->direction, *slide_master->color == 0 ? RGB565_RED : RGB565_BLUE);
                    delay(100);
                
                }
            }
        }        
    }
}

bool joystick_command(Joystick *joystick){
    if(read(joystick->fd, &joystick->ev, sizeof(struct input_event)) < 0) {
        if(errno == EINTR)
            return false;
 
        exit(1);
    }
 
    if(EV_KEY != joystick->ev.type || joystick->ev.value != 1)
        return false;
    return true;
}

void init_slide_master(Slide_master *slide_master){
    slide_master->state_isCorrect = true;
    slide_master->direction = &slide_master->direction_v;
    slide_master->color = &slide_master->color_v;
}

void open_led_matrix(Led_matrix *led_matrix){
    
    led_matrix->fbfd = open(LEDMATRIX_PATH, O_RDWR);
    if (led_matrix->fbfd == -1){
        perror("Error (call to 'open')");
        exit(EXIT_FAILURE);
    }

    if (ioctl(led_matrix->fbfd, FBIOGET_FSCREENINFO, &led_matrix->fix_info) == -1){
        perror("Error (call to 'ioctl')");
        close(led_matrix->fbfd);
        exit(EXIT_FAILURE);
    }

    if (strcmp(led_matrix->fix_info.id, "RPi-Sense FB") != 0){
        printf("%s\n", "Error: RPi-Sense FB not fount");
        close(led_matrix->fbfd);
        exit(EXIT_FAILURE);
    }

    led_matrix->map = mmap(NULL, FILESIZE, PROT_READ | PROT_WRITE, MAP_SHARED, led_matrix->fbfd, 0);
    if (led_matrix->map == MAP_FAILED){
        close(led_matrix->fbfd);
        perror("Error mmapping the file");
        exit(EXIT_FAILURE);
    }
    memset(led_matrix->map, 0, FILESIZE);
}

void close_led_matrix(Led_matrix *led_matrix){
    memset(led_matrix->map, 0, FILESIZE);

    if (munmap(led_matrix->map, FILESIZE) == -1){
        perror("Error un-mmapping the file");
    }
    close(led_matrix->fbfd);
}

void open_joystick(Joystick *joystick){
    joystick->fd = open(JOYSTICK_PATH, O_RDONLY);
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

void arrow(uint16_t *p, int direction, int color_code){
    // printf("%d, %d\n", direction, color);
    int arrow_shape[4][64] = { // Up, Left, Right, Down
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

    rotate_cww90(p, arrow_shape[direction], color_code);
}

bool check(int answer_direction, int answer_color, int direction){
    // printf("%d, %d, %d\n", answer_direction, answer_color, direction);
    int answer_sheet[2][4] = {{106, 103, 108, 105}, {105, 108, 103, 106}};

    if (answer_sheet[answer_color][answer_direction] == direction) return true;
    else return false;

}

void rotate_cww90(uint16_t *p, int shape[], int color_code){
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
        if (shape[rotate[i]] == 1)    *(p+i) = color_code;
    }

}

void delay(int t){
    usleep(t*1000);
}
