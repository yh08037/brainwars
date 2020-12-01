#include "game_basis.h"

double wtime()
{
    static int sec = -1;
    struct timeval tv;
    gettimeofday(&tv, NULL);

    if (sec < 0) sec = tv.tv_sec;

    return (tv.tv_sec - sec) + 1.0e-6*tv.tv_usec;
}

void delay(int t)
{
    usleep(t * 1000);
}

void game_ready(uint16_t *map){
    memset(map, 0, FILESIZE);
    double start_time, check_time, elapsed_time;
    start_time = wtime();

    while(1){
        check_time = wtime();
        elapsed_time = check_time - start_time;

        if (elapsed_time > 3){
            return;
        }
        if (elapsed_time > 2){
            number_countdown(map, 1);
            ring(map, 3 - elapsed_time, 1, RGB565_GREEN, false);
        }
        else if (elapsed_time > 1){
            number_countdown(map, 2);
            ring(map, 2 - elapsed_time, 1, RGB565_GREEN, false);
        }
        else{
            number_countdown(map, 3);
            ring(map, 1 - elapsed_time, 1, RGB565_GREEN, false);
        }
        delay(10);
    }
    
}
