#ifndef SLIDE_MASTER_H
#define SLIDE_MASTER_H

#include "game_basis.h"

typedef struct {
    bool state_isCorrect;
    int direction;
    int color;
} slide_master_t;

void slide_master_game(result_t *result, led_matrix_t *led_matrix); // call this function to start slide_master
void init_slide_master(slide_master_t *slide_master);
void random_arrow(uint16_t *map, int *direction, int *color);
void arrow(uint16_t *map, int direction, int color_code);
bool check_slide_master(int answer_direction, int answer_color, int direction);

#endif
