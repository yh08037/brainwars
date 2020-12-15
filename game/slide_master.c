#include "slide_master.h"


void slide_master_game(result_t *result, led_matrix_t *led_matrix){

    result->correct = -1;
    result->wrong = 0;
    double start_time, check_time;
    double elapsed_time;

    slide_master_t slide_master;

    slide_master.state_isCorrect = true;

    game_ready(led_matrix->map);

    start_time = wtime();

    while(true)
    {
        check_time = wtime();
        elapsed_time = check_time - start_time;
        
        ring(led_matrix->map, PLAY_TIME - elapsed_time, PLAY_TIME, RGB565_WEAKGREEN, false);

        if (elapsed_time > PLAY_TIME){
            break;
        }
        
        if (slide_master.state_isCorrect == true){
            memset(led_matrix->map, 0, FILESIZE);
            delay(100);
            random_arrow(led_matrix->map, &slide_master.direction, &slide_master.color);
            ring(led_matrix->map, PLAY_TIME - elapsed_time, PLAY_TIME, RGB565_WEAKGREEN, false);
            slide_master.state_isCorrect = false;
            result->correct++;
            continue;
        }

        if (joystick_data.ev.type != 0 && joystick_data.ev.value == 1){
            slide_master.state_isCorrect = check_slide_master(slide_master.direction, slide_master.color, joystick_data.ev.code);

            if (slide_master.state_isCorrect == false){
                for (int i = 0; i < 4; i++){
                    memset(led_matrix->map, 0, FILESIZE);
                    delay(100);
                    arrow(led_matrix->map, slide_master.direction, slide_master.color == 0 ? RGB565_RED : RGB565_BLUE);
                    ring(led_matrix->map, PLAY_TIME - elapsed_time, PLAY_TIME, RGB565_WEAKGREEN, false);
                    delay(100);
                
                }
                result->wrong++;
            }
            else{
                arrow(led_matrix->map, slide_master.direction, slide_master.color == 0 ? RGB565_RED : RGB565_BLUE);
                ring(led_matrix->map, PLAY_TIME - elapsed_time, PLAY_TIME, RGB565_WEAKGREEN, false);
            }    

            delay(DELAY_SYNC);
        }
    }

    int score = result->correct * CORRECT_POINT + result->wrong * WRONG_POINT;
    if (score < 0) score = 0;

    result->score = score;

    disp_nums(led_matrix->map, score, RGB565_WHITE);
    delay(5000);
}

void random_arrow(uint16_t *map, int *direction, int *color){ // map - input, direction color - output
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

    rotate_ccw90(map, arrow_shape[direction], color_code);
}

bool check_slide_master(int answer_direction, int answer_color, int direction){
    int answer_sheet[2][4] = {{106, 103, 108, 105}, {105, 108, 103, 106}};

    if (answer_sheet[answer_color][answer_direction] == direction) return true;
    else return false;

}
