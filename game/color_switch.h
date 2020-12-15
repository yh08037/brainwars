#ifndef COLOR_SWITCH_H
#define COLOR_SWITCH_H

#define NUM_NODES 50

#include "game_basis.h"

typedef struct {
    bool is_vaild;
    int line_no;
    double current_pos;
    double falling_speed;
    int color; // 0: blue 1: red
} color_switch_node_t;

typedef struct {
    int my_state[3];
    color_switch_node_t nodes[NUM_NODES];
    int len_node;
    double cool_time[3];
} color_switch_t;


void color_switch_game(result_t *result, led_matrix_t *led_matrix); // call this function to start slide_master
void disp_color_switch(uint16_t *map, color_switch_t rainfall);
void check_color_switch(color_switch_t *color_switch, result_t *result);
void random_generate_node(color_switch_t *color_switch);
void update_position(color_switch_t *color_switch);

#endif