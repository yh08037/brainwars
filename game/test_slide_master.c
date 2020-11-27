#include "slide_master.h"

/* execution
 * gcc -o test test_slide_master.c slide_master.c led_matrix.c joystick.c -lpthread
*/

int main(void){

    led_matrix_t *led_matrix, led_matrix_v;
    led_matrix = &led_matrix_v;

    open_led_matrix(led_matrix);

    result_t *result;
    result_t result_v;
    result = &result_v;

    slide_master_game(result, led_matrix); // call this function to start slide master.

    close_led_matrix(led_matrix);
    return 0;
}