#ifndef HIGH_OR_LOW_H
#define HIGH_OR_LOW_H

#include "game_basis.h"

typedef struct {
    bool state_isCorrect;
    int now_number;
    int before_number;
} high_or_low_t;


void high_or_low_game(result_t *result, led_matrix_t *led_matrix); // call this function to start slide_master
void random_number(uint16_t *map, high_or_low_t *high_or_low);
bool check_high_or_low();
void show_0(uint16_t *map);

#endif
