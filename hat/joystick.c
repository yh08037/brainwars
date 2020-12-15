#include "joystick.h"


void open_joystick() {
    joystick_data.fd = open(JOYSTICK_PATH, O_RDONLY);
    //is_thread_stop = false;
    if (joystick_read_thread() != 0){
        fprintf(stderr, "Failed to Matrix multiplication\n");
        exit(1);
    }
}

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
        if (joystick_data.ev.type != 0 && joystick_data.ev.value == 1){
            joystick_vaild_data.joystick_data = joystick_data;
            joystick_vaild_data.vaild = 1;
        }

        sleep(DELAY_SYNC / 1000.0);
    }
}
