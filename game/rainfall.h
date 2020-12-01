#ifndef RAINFALL_H
#define RAINFALL_H

#include "game_basis.h"

typedef struct {
    bool state_isCorrect;
    int my_pos;
    int object[5][2];
    int object_index;
} rainfall_t;

void rainfall_game(result_t *result, led_matrix_t *led_matrix); // call this function to start slide_master
void disp_rainfall(uint16_t *map, rainfall_t rainfall);
bool check_rainfall(rainfall_t rainfall, int direction);


#endif