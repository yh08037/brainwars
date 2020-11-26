#include "slide_master.h"

int main(void){

    led_matrix_t *led_matrix, led_matrix_v;
    led_matrix = &led_matrix_v;

    open_led_matrix(led_matrix);

    slide_master_game(led_matrix);

    close_led_matrix(led_matrix);
    return 0;
}

void slide_master_game(led_matrix_t* led_matrix){
    srand(time(NULL));
    
    int score = 0;
    double start_time, check_time;
    double elapsed_time;

    slide_master_t *slide_master, slide_master_v;
    slide_master = &slide_master_v;

    slide_master->state_isCorrect = true;
    slide_master->direction = &slide_master->direction_v;
    slide_master->color = &slide_master->color_v;

    slide_master_game_ready(led_matrix->map);

    joystick_data.fd = open(JOYSTICK_PATH, O_RDONLY);
    is_thread_stop = false;
    if (joystick_read_thread() != 0){
        fprintf(stderr, "Failed to Matrix multiplication\n");
        exit(1);
    }
    start_time = wtime();

    while(true)
    {
        check_time = wtime();
        elapsed_time = check_time - start_time;
        

        if (elapsed_time > PLAY_TIME){
            //printf("Game Finished~ Score : %d\n", score);
            is_thread_stop = true;
            break;
        }
        
        if (slide_master->state_isCorrect == true){
            memset(led_matrix->map, 0, FILESIZE);
            delay(100);
            random_arrow(led_matrix->map, slide_master->direction, slide_master->color);
            ring(led_matrix->map, PLAY_TIME - elapsed_time, PLAY_TIME, RGB565_WEAKGREEN, false);
            slide_master->state_isCorrect = false;
            score += CORRECT_POINT;
            continue;
        }

        if (joystick_data.ev.type != 0 && joystick_data.ev.value == 1){
            slide_master->state_isCorrect = check(*slide_master->direction, *slide_master->color, joystick_data.ev.code);

            if (slide_master->state_isCorrect == false){
                for (int i = 0; i < 4; i++){
                    memset(led_matrix->map, 0, FILESIZE);
                    delay(100);
                    arrow(led_matrix->map, *slide_master->direction, *slide_master->color == 0 ? RGB565_RED : RGB565_BLUE);
                    ring(led_matrix->map, PLAY_TIME - elapsed_time, PLAY_TIME, RGB565_WEAKGREEN, false);
                    delay(100);
                
                }
                if (score != 0) score += WRONG_POINT;
                }
        }
        else{
            arrow(led_matrix->map, *slide_master->direction, *slide_master->color == 0 ? RGB565_RED : RGB565_BLUE);
            ring(led_matrix->map, PLAY_TIME - elapsed_time, PLAY_TIME, RGB565_WEAKGREEN, false);
        }    

        delay(DELAY_SYNC);
    }

    disp_score(led_matrix->map, score);
    delay(5000);
}

void disp_score(uint16_t *map, int score){
    memset(map, 0, FILESIZE);

    if (score >= 100){
        printf("MAX score! score: %d", score);
        score = 99;
    }

    int ten_degit = score / 10;
    int one_degit = score - ten_degit * 10;

    if (ten_degit != 0)    disp_num(map, ten_degit, 1, 2);
    disp_num(map, one_degit, 5, 2);
}

void disp_num(uint16_t *map, int degit, int x_pos, int y_pos){
    int num_position_init[15] = {
        56, 48, 40,
        57, 49, 41,
        58, 50, 42,
        59, 51, 43,
        60, 52, 44,
    };

    int numbers[10][15] = {
        {
            1, 1, 1,
            1, 0, 1,
            1, 0, 1,
            1, 0, 1,
            1, 1, 1,
        },
        {
            0, 1, 1,
            0, 0, 1,
            0, 0, 1,
            0, 0, 1,
            0, 0, 1,
        },
        {
            1, 1, 1,
            0, 0, 1,
            1, 1, 1,
            1, 0, 0,
            1, 1, 1,
        },
        {
            1, 1, 1,
            0, 0, 1,
            1, 1, 1,
            0, 0, 1,
            1, 1, 1,
        },
        {
            1, 0, 1,
            1, 0, 1,
            1, 1, 1,
            0, 0, 1,
            0, 0, 1,
        },
        {
            1, 1, 1,
            1, 0, 0,
            1, 1, 1,
            0, 0, 1,
            1, 1, 1,
        },
        {
            1, 1, 1,
            1, 0, 0,
            1, 1, 1,
            1, 0, 1,
            1, 1, 1,
        },
        {
            1, 1, 1,
            1, 0, 1,
            0, 0, 1,
            0, 0, 1,
            0, 0, 1,
        },
        {
            1, 1, 1,
            1, 0, 1,
            1, 1, 1,
            1, 0, 1,
            1, 1, 1,
        },
        {
            1, 1, 1,
            1, 0, 1,
            1, 1, 1,
            0, 0, 1,
            0, 0, 1,
        }
    };

    int num_position[15] = { 0, };
    for (int i = 0; i < 15; i++){
        if (numbers[degit][i] == 1){
            num_position[i] = num_position_init[i] - 8*x_pos + y_pos;
            *(map+num_position[i]) = RGB565_WHITE;
        }
    }
}

void slide_master_game_ready(uint16_t *map){
    memset(map, 0, FILESIZE);
    double start_time, check_time, elapsed_time;
    start_time = wtime();

    while(1){
        check_time = wtime();
        elapsed_time = check_time - start_time;

        if (elapsed_time > 3){
            return;
        }
        if (elapsed_time > 2){
            number_countdown(map, 1);
            ring(map, 3 - elapsed_time, 1, RGB565_GREEN, false);
        }
        else if (elapsed_time > 1){
            number_countdown(map, 2);
            ring(map, 2 - elapsed_time, 1, RGB565_GREEN, false);
        }
        else{
            number_countdown(map, 3);
            ring(map, 1 - elapsed_time, 1, RGB565_GREEN, false);
        }
        delay(10);
    }
}

void number_countdown(uint16_t *map, int number){
    memset(map, 0, FILESIZE);
    int num_shape[3][64] = { // 1, 2, 3
    {
        0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,
        0,0,0,0,1,1,0,0,
        0,0,0,0,0,1,0,0,
        0,0,0,0,0,1,0,0,
        0,0,0,0,0,1,0,0,
        0,0,0,0,0,1,0,0,
        0,0,0,0,0,0,0,0, },
    {
        0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,
        0,0,0,1,1,1,0,0,
        0,0,0,0,0,1,0,0,
        0,0,0,1,1,1,0,0,
        0,0,0,1,0,0,0,0,
        0,0,0,1,1,1,0,0,
        0,0,0,0,0,0,0,0, },
    {
        0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,
        0,0,0,1,1,1,0,0,
        0,0,0,0,0,1,0,0,
        0,0,0,1,1,1,0,0,
        0,0,0,0,0,1,0,0,
        0,0,0,1,1,1,0,0,
        0,0,0,0,0,0,0,0, },

    };

    rotate_cww90(map, num_shape[number-1], RGB565_BLUE);
}

void ring(uint16_t *map, double left_time, double max_time, int color_code, bool is_addColor){
    int k, new_color_code;
    RGB565_t current_color_code, add_color_code;
    int ring_order[28] = { 32, 40, 48, 56, 57, 58, 59, 60, 61, 62, 63, 55, 47, 39, 31, 23, 15, 7, 6, 5, 4, 3, 2, 1, 0, 8, 16, 24};

    if (is_addColor){
        for (int i = 0; i < left_time / max_time * 28; i++){
            k = ring_order[27-i];
            current_color_code.R = (*(map+k) & 1111100000000000) >> 11;
            current_color_code.G = (*(map+k) & 11111100000) >> 5;
            current_color_code.B = (*(map+k) & 11111);

            add_color_code.R = (color_code & 1111100000000000) >> 11;
            add_color_code.G = (color_code & 11111100000) >> 5;
            add_color_code.B = (color_code & 11111);

            if (current_color_code.R + add_color_code.R < 31) current_color_code.R += add_color_code.R;
            else current_color_code.R = 31;

            if (current_color_code.G + add_color_code.G * 2 < 63) current_color_code.G += add_color_code.G * 2;
            else current_color_code.G = 63;

            if (current_color_code.B + add_color_code.B < 31) current_color_code.B += add_color_code.B;
            else current_color_code.B = 31;

            new_color_code = (current_color_code.R << 11) | (current_color_code.G << 5) | current_color_code.B;

            *(map+k) = new_color_code;
        }
    }
    else{
        for (int i = 0; i < left_time / max_time * 28; i++){
            k = ring_order[27-i];
            if (*(map+k) == 0) *(map+k) = color_code;
        }
    }

}

int joystick_read_thread(){
    int res;
    pthread_t joystick_thread;

    res = pthread_create(&joystick_thread, NULL, joystick_command, NULL);
    if (res != 0){
        perror("Thread creation failed");
        exit(EXIT_FAILURE);
    }

    return 0;
}

void *joystick_command(void *arg){
    while (1){
        read(joystick_data.fd, &joystick_data.ev, sizeof(struct input_event));

        delay(DELAY_SYNC);

        if (is_thread_stop){
            break;
        }
    }
}

void open_led_matrix(led_matrix_t *led_matrix){
    
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

void close_led_matrix(led_matrix_t *led_matrix){
    memset(led_matrix->map, 0, FILESIZE);

    if (munmap(led_matrix->map, FILESIZE) == -1){
        perror("Error un-mmapping the file");
    }
    close(led_matrix->fbfd);
}

int random_arrow(uint16_t *map, int *direction, int *color){ // map - input, direction color - output
    int direction_random = rand() % 4;
    int color_random = rand() % 2;

    *direction = direction_random;
    *color = color_random;

    if (color_random == 0) color_random = RGB565_RED;
    else  color_random = RGB565_BLUE;

    arrow(map, direction_random, color_random);
}

void arrow(uint16_t *map, int direction, int color_code){
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

    rotate_cww90(map, arrow_shape[direction], color_code);
}

bool check(int answer_direction, int answer_color, int direction){
    int answer_sheet[2][4] = {{106, 103, 108, 105}, {105, 108, 103, 106}};

    if (answer_sheet[answer_color][answer_direction] == direction) return true;
    else return false;

}

void rotate_cww90(uint16_t *map, int shape[], int color_code){
    memset(map, 0, FILESIZE);

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

    for (int i = 0; i < NUM_WORDS; i++){
        if (shape[rotate[i]] == 1)    *(map+i) = color_code;
    }

}

void delay(int t){
    usleep(t*1000);
}

double wtime()
{
    static int sec = -1;
    struct timeval tv;
    gettimeofday(&tv, NULL);

    if (sec < 0) sec = tv.tv_sec;

    return (tv.tv_sec - sec) + 1.0e-6*tv.tv_usec;
}
