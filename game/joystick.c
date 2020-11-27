#include "joystick.h"


int joystick_read_thread(){
    int res;
    pthread_t joystick_thread;

    res = pthread_create(&joystick_thread, NULL, joystick_command, NULL);
    if (res != 0){
        perror("Thread creation failed");
        exit(EXIT_FAILURE);
    }

    return 0;
}

void *joystick_command(void *arg){
    while (1){
        read(joystick_data.fd, &joystick_data.ev, sizeof(struct input_event));

        // delay(DELAY_SYNC);
        sleep(DELAY_SYNC / 1000.0);

        if (is_thread_stop){
            break;
        }
    }
}
