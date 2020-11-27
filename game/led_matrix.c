#include "led_matrix.h"

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

void disp_nums(uint16_t *map, int score, int color_code_){
    memset(map, 0, FILESIZE);

    int color_code = color_code_;

    if (score >= 100){
        printf("MAX score! score: %d\n", score);
        for (int i = 0; i < score - 99 ; i++){
            *(map+(56-8*i)) = RGB565_BLUE;
            color_code = RGB565_BLUE;
            if (i == 7){
                break;
            }
        }
        for (int i = 0; i < score - 107; i++){
            *(map+(56-8*i)) = RGB565_GREEN;
            color_code = RGB565_GREEN;
            if (i == 7){
                break;
            }
        }
        for (int i = 0; i < score - 115; i++){
            *(map+(56-8*i)) = RGB565_RED;
            color_code = RGB565_RED;
            if (i == 7){
                break;
            }
        }
        disp_num(map, 9, 1, 2, color_code);
        disp_num(map, 9, 5, 2, color_code);

        return;
    }

    int ten_degit = score / 10;
    int one_degit = score - ten_degit * 10;

    if (ten_degit != 0)    disp_num(map, ten_degit, 1, 2, color_code);
    disp_num(map, one_degit, 5, 2, color_code);
}

void disp_num(uint16_t *map, int degit, int x_pos, int y_pos, int color_code){
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
            *(map+num_position[i]) = color_code;
        }
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

    rotate_ccw90(map, num_shape[number-1], RGB565_BLUE);
}

void ring(uint16_t *map, double left_time, double max_time, int color_code, bool is_addColor){
    int i, k, new_color_code;
    RGB565_t current_color_code, add_color_code;
    //int ring_order[28] = { 32, 40, 48, 56, 57, 58, 59, 60, 61, 62, 63, 55, 47, 39, 31, 23, 15, 7, 6, 5, 4, 3, 2, 1, 0, 8, 16, 24};
    int ring_order[28] = { 24, 16, 8, 0, 1, 2, 3, 4, 5, 6, 7, 15, 23, 31, 39, 47, 55, 63, 62, 61, 60, 59, 58, 57, 56, 48, 40, 32};

    if (is_addColor){
        for (i = 0; i < left_time / max_time * 28; i++){
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
        for (i = 0; i < left_time / max_time * 28; i++){
            k = ring_order[i];
            if (*(map+k) == RGB565_OFF){
                *(map+k) = color_code;
            }
        }
        i++;
        if (i < 28){
            k = ring_order[i];
            if (*(map+k) == RGB565_WEAKGREEN) *(map+k) = RGB565_OFF;
        }
    }

}

void rotate_ccw90(uint16_t *map, int shape[], int color_code){
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
