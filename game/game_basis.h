#define PLAY_TIME 20 //sec

#define CORRECT_POINT 3
#define WRONG_POINT -2

#include "led_matrix.h"
#include "joystick.h"

typedef struct _result_t {
    int correct;
    int wrong;
} result_t;

void delay(int);
double wtime();
