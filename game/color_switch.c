#include "color_switch.h"

void color_switch_game(result_t *result, led_matrix_t *led_matrix) // call this function to start slide_master
{
    result->correct = 0;
    result->wrong = 0;
    double start_time, check_time;
    double elapsed_time;

    color_switch_t color_switch;
    for (int i = 0; i < 3; i++){
        color_switch.my_state[i] = 0;
    }
    color_switch.len_node = 0;

    game_ready(led_matrix->map);
    memset(led_matrix->map, 0, FILESIZE);

    for (int i = 0; i < 3; i++){
        color_switch.cool_time[i] = wtime();
    }

    start_time = wtime();
    
    while(true){
        check_time = wtime();
        elapsed_time = check_time - start_time;

        ring(led_matrix->map, PLAY_TIME - elapsed_time, PLAY_TIME, RGB565_WEAKGREEN, false);

        if (elapsed_time > PLAY_TIME){
            break; 
        }

        if (joystick_vaild_data.vaild){
            if (joystick_vaild_data.joystick_data.ev.code == 108){
                color_switch.my_state[0] = color_switch.my_state[0]^1;
            }
            else if (joystick_vaild_data.joystick_data.ev.code == 28){
                color_switch.my_state[1] = color_switch.my_state[1]^1;
            }
            else if (joystick_vaild_data.joystick_data.ev.code == 103){
                color_switch.my_state[2] = color_switch.my_state[2]^1;
            }
            joystick_vaild_data.vaild = 0;
        }

        disp_color_switch(led_matrix->map, color_switch);
        check_color_switch(&color_switch, result);
        random_generate_node(&color_switch);
        update_position(&color_switch);

        delay(DELAY_SYNC);

    }
    int score = result->correct * CORRECT_POINT + result->wrong * WRONG_POINT;
    if (score < 0) score = 0;

    disp_nums(led_matrix->map, score, RGB565_WHITE);
    delay(5000);
}

void disp_color_switch(uint16_t *map, color_switch_t color_switch){
    int j;
    int new_platform = 0;
    int color_code;

    for (int i = 0; i < 3; i++){
        if (color_switch.my_state[i] == 0){  // color: blue
            //if (*(map+8*(5-2*i)+6) != RGB565_BLUE){
            if (1){
                *(map+8*(5-2*i)+6) = RGB565_BLUE;
                *(map+8*(5-2*i)+5) = RGB565_WEAKBLUE; // up

                if (i > 0 && color_switch.my_state[i-1] == 0 || i == 0){
                    *(map+8*(6-2*i)+6) = RGB565_WEAKBLUE; // left
                }
                else {
                    *(map+8*(6-2*i)+6) = RGB565_WEAKREDBLUE; // left
                }

                if (i < 2 && color_switch.my_state[i+1] == 0 || i == 2){
                    *(map+8*(4-2*i)+6) = RGB565_WEAKBLUE; // right
                }
                else {
                    *(map+8*(4-2*i)+6) = RGB565_WEAKREDBLUE; // left
                }
            }
        }
        else {  // color: red
            //if (*(map+8*(5-2*i)+6) != RGB565_RED){
            if (1){
                *(map+8*(5-2*i)+6) = RGB565_RED;
                *(map+8*(5-2*i)+5) = RGB565_WEAKRED; // up

                if (i > 0 && color_switch.my_state[i-1] == 1 || i == 0){
                    *(map+8*(6-2*i)+6) = RGB565_WEAKRED; // left
                }
                else {
                    *(map+8*(6-2*i)+6) = RGB565_WEAKREDBLUE; // left
                }

                if (i < 2 && color_switch.my_state[i+1] == 1 || i == 2){
                    *(map+8*(4-2*i)+6) = RGB565_WEAKRED; // right
                }
                else {
                    *(map+8*(4-2*i)+6) = RGB565_WEAKREDBLUE; // left
                }
            }
        }
    }
    
    for (int i = 0; i < color_switch.len_node; i++){ // Node draw

        if (color_switch.nodes[i].is_vaild){
            if (color_switch.nodes[i].color == 0) color_code = RGB565_BLUE;
            else color_code = RGB565_RED;
            
            //printf("%f\n", color_switch.nodes[i].current_pos);
            new_platform = 8 * (5 - 2 * color_switch.nodes[i].line_no) + (int)color_switch.nodes[i].current_pos;
            *(map+new_platform) = color_code;
            if ((int)color_switch.nodes[i].current_pos != 0){
                *(map+new_platform-1) = 0;
            }
        }
    }
}

void check_color_switch(color_switch_t *color_switch, result_t *result){
    for (int i = 0; i < color_switch->len_node; i++){
        if (color_switch->nodes[i].is_vaild){
            if (color_switch->nodes[i].current_pos > 6){
                if (color_switch->nodes[i].color == color_switch->my_state[color_switch->nodes[i].line_no]){
                    result->correct++;
                }
                else{
                    result->wrong++;
                }
                color_switch->nodes[i].is_vaild = false;
            }
        }
    }
}

void random_generate_node(color_switch_t *color_switch){
    color_switch_node_t new_node;
    new_node.line_no = rand() % 3;
    double check_time = wtime();
    double weight = check_time - color_switch->cool_time[new_node.line_no];
    
    if (weight < 0.6){
        return;
    }

    int div = 30 - 15 * weight;
    if (div <= 0) div = 1;

    if (rand() % div == 0){
        new_node.is_vaild = 1;
        new_node.current_pos = 0;
        new_node.falling_speed = rand() % 5;
        new_node.color = rand() % 2;
        color_switch->nodes[color_switch->len_node++] = new_node;
        color_switch->cool_time[new_node.line_no] = wtime();
    }
}

void update_position(color_switch_t *color_switch){
    for (int i = 0; i < color_switch->len_node; i++){
        if (color_switch->nodes[i].is_vaild){
            color_switch->nodes[i].current_pos += 0.20 + 0.03 * color_switch->nodes[i].falling_speed;
        }
    }
}
