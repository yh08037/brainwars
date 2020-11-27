#include "high_or_low.h"


void high_or_low_game(result_t *result, led_matrix_t *led_matrix){

    result->correct = -1;
    result->wrong = 0;
    double start_time, check_time;
    double elapsed_time;

    high_or_low_t high_or_low;

    high_or_low.state_isCorrect = true;
    high_or_low.before_number = 0;
    high_or_low.now_number = 0;

    game_ready(led_matrix->map);
    show_0(led_matrix->map);

    // joystick_data.fd = open(JOYSTICK_PATH, O_RDONLY);
    // is_thread_stop = false;
    // if (joystick_read_thread() != 0){
    //     fprintf(stderr, "Failed to Matrix multiplication\n");
    //     exit(1);
    // }
    start_time = wtime();

    while(true)
    {
        check_time = wtime();
        elapsed_time = check_time - start_time;
        
        //arrow(led_matrix->map, *slide_master->direction, *slide_master->color == 0 ? RGB565_RED : RGB565_BLUE);
        ring(led_matrix->map, PLAY_TIME - elapsed_time, PLAY_TIME, RGB565_WEAKGREEN, false);

        if (elapsed_time > PLAY_TIME){
            //printf("Game Finished~ Score : %d\n", score);
            //is_thread_stop = true;
            break;
        }
        
        if (high_or_low.state_isCorrect == true){
            memset(led_matrix->map, 0, FILESIZE);
            delay(100);
            random_number(led_matrix->map, &high_or_low);
            ring(led_matrix->map, PLAY_TIME - elapsed_time, PLAY_TIME, RGB565_WEAKGREEN, false);
            high_or_low.state_isCorrect = false;
            result->correct++;
            continue;
        }

        if (joystick_data.ev.type != 0 && joystick_data.ev.value == 1){
            high_or_low.state_isCorrect = check_high_or_low(high_or_low, joystick_data.ev.code);

            if (high_or_low.state_isCorrect == false){
                for (int i = 0; i < 4; i++){
                    memset(led_matrix->map, 0, FILESIZE);
                    delay(100);
                    disp_nums(led_matrix->map, high_or_low.now_number, RGB565_WEAKWHITE);
                    ring(led_matrix->map, PLAY_TIME - elapsed_time, PLAY_TIME, RGB565_WEAKGREEN, false);
                    delay(100);
                
                }
                result->wrong++;
        }
        else{
            disp_nums(led_matrix->map, high_or_low.now_number, RGB565_WEAKWHITE);
            ring(led_matrix->map, PLAY_TIME - elapsed_time, PLAY_TIME, RGB565_WEAKGREEN, false);
        }    

        delay(DELAY_SYNC);
        }
    }

    int score = result->correct * CORRECT_POINT + result->wrong * WRONG_POINT;
    if (score < 0) score = 0;


    disp_nums(led_matrix->map, score, RGB565_WHITE);
    delay(5000);
}

void random_number(uint16_t *map, high_or_low_t *high_or_low){

    high_or_low->before_number = high_or_low->now_number;
    do{
        high_or_low->now_number = rand() % 100;
    } while(high_or_low->before_number == high_or_low->now_number);

    disp_nums(map, high_or_low->now_number, RGB565_WEAKWHITE);
}

bool check_high_or_low(high_or_low_t high_or_low, int direction){
    if (high_or_low.now_number > high_or_low.before_number && direction == 105 ||
        high_or_low.now_number < high_or_low.before_number && direction == 106) {
        return true;
    }
    return false;
}

void show_0(uint16_t *map){
    memset(map, 0, FILESIZE);
    disp_num(map, 0, 5, 2, RGB565_WEAKWHITE);
    delay(1000);

}