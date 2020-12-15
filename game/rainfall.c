#include "rainfall.h"

#define CORRECT_POINT_RAINFALL 1
#define WRONG_POINT_RAINFALL -1

void rainfall_game(result_t *result, led_matrix_t *led_matrix)
{ // call this function to start slide_master
    result->correct = -1;
    result->wrong = 0;
    double start_time, check_time;
    double elapsed_time;

    rainfall_t rainfall;
    rainfall.state_isCorrect = true;
    rainfall.my_pos = 108;

    for (int i = 0; i < 5; i++){
        rainfall.object[i][0] = rand() % 2;
        rainfall.object[i][1] = rand() % 2;
    }
    rainfall.object_index = 0;

    game_ready(led_matrix->map);

    start_time = wtime();

    while(true){
        check_time = wtime();
        elapsed_time = check_time - start_time;

        ring(led_matrix->map, PLAY_TIME - elapsed_time, PLAY_TIME, RGB565_WEAKGREEN, false);

        if (elapsed_time > PLAY_TIME){
            break;
        }

        if (rainfall.state_isCorrect == true){
            memset(led_matrix->map, 0, FILESIZE);

            rainfall.object[rainfall.object_index][0] = rand() % 2;
            rainfall.object[rainfall.object_index][1] = rand() % 2;
            rainfall.object_index++;
            if (rainfall.object_index == 5){
                rainfall.object_index = 0;
            }

            disp_rainfall(led_matrix->map, rainfall);
            ring(led_matrix->map, PLAY_TIME - elapsed_time, PLAY_TIME, RGB565_WEAKGREEN, false);
            rainfall.state_isCorrect = false;
            result->correct++;
            continue;
        }

        if (joystick_data.ev.type != 0 && joystick_data.ev.value == 1){
            rainfall.state_isCorrect = check_rainfall(rainfall, joystick_data.ev.code);
            rainfall.my_pos = joystick_data.ev.code;

            if (rainfall.state_isCorrect == false){
                for (int i = 0; i < 4; i++){
                    memset(led_matrix->map, 0, FILESIZE);
                    delay(100);
                    disp_rainfall(led_matrix->map, rainfall);
                    ring(led_matrix->map, PLAY_TIME - elapsed_time, PLAY_TIME, RGB565_WEAKGREEN, false);
                    delay(100);
                
                }
                result->wrong++;
            }
            else{
                disp_rainfall(led_matrix->map, rainfall);
                ring(led_matrix->map, PLAY_TIME - elapsed_time, PLAY_TIME, RGB565_WEAKGREEN, false);
            }    
            delay(DELAY_SYNC);
        }
    }

    int score = result->correct * CORRECT_POINT_RAINFALL + result->wrong * WRONG_POINT_RAINFALL;
    if (score < 0) score = 0;

    result->score = score;

    disp_nums(led_matrix->map, score, RGB565_WHITE);
    delay(5000);
}

void disp_rainfall(uint16_t *map, rainfall_t rainfall){
    memset(map, 0, FILESIZE);

    int platform[2] = {56, 48};
    int new_platform[2] = { 0, };
    int color_code, j;

    for (int i = 0; i < 5; i++){
        j = (i + rainfall.object_index) % 5;
        if (rainfall.object[j][0] == 0){
            color_code = RGB565_BLUE;
        }
        else {
            color_code = RGB565_RED;
        }
        
        for (int k = 0; k < 2; k++){
            new_platform[k] = platform[k] - 8 * (2 + 3 * rainfall.object[j][1]) + (5-i);
            
            //printf("%d %d\n", rainfall.object[j][1], new_platform[k]);
            *(map+(new_platform[k])) = color_code;
        }
    }
    if (rainfall.my_pos == 108){
        *(map+46) = RGB565_GREEN;
        *(map+38) = RGB565_GREEN;
    }
    else {
        *(map+22) = RGB565_GREEN;
        *(map+14) = RGB565_GREEN;
    }
}

bool check_rainfall(rainfall_t rainfall, int direction){
    if (rainfall.object[rainfall.object_index][0] ^ rainfall.object[rainfall.object_index][1] == 0 &&
        direction == 108) {
            return true;
        }

    else if(rainfall.object[rainfall.object_index][0] ^ rainfall.object[rainfall.object_index][1] == 1 &&
        direction == 103) return true;
    
    return false;
}
