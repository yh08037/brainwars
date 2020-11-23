#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
 
int main(int argc, char** argv) {
    uint8_t keys[128];
    int fd;
 
    fd = open(argv[1], O_RDONLY);
 
    while(1)
    {
        struct input_event ev;

        //printf("hello\n");
 
        if(read(fd, &ev, sizeof(struct input_event)) < 0)
        {
            if(errno == EINTR)
                continue;
 
            break;
        }
 
        if(EV_KEY != ev.type)
            continue;
 

        printf("%d, %d\n", ev.code, ev.value);
    }
 
    close(fd);
 
    return 0;
}