#include "led_matrix.h"

// int main(void)
// {
//     int i;
//     int fbfd;
//     uint16_t *map;
//     uint16_t *p;
//     struct fb_fix_screeninfo fix_info;

//     /* open the led frame buffer device */
//     fbfd = open(LEDMATRIX_PATH, O_RDWR);
//     if (fbfd == -1) {
// 	perror("Error (call to 'open')");
// 	exit(EXIT_FAILURE);
//     }

//     /* read fixed screen info for the open device */
//     if (ioctl(fbfd, FBIOGET_FSCREENINFO, &fix_info) == -1) {
// 	perror("Error (call to 'ioctl')");
// 	close(fbfd);
// 	exit(EXIT_FAILURE);
//     }

//     /* now check the correct device has been found */
//     if (strcmp(fix_info.id, "RPi-Sense FB") != 0) {
// 	printf("%s\n", "Error: RPi-Sense FB not found");
// 	close(fbfd);
// 	exit(EXIT_FAILURE);
//     }

//     /* map the led frame buffer device into memory */
//     map = mmap(NULL, FILESIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
//     if (map == MAP_FAILED) {
// 	close(fbfd);
// 	perror("Error mmapping the file");
// 	exit(EXIT_FAILURE);
//     }

//     /* set a pointer to the start of the memory area */
//     p = map;

//     /* clear the led matrix */
//     memset(map, 0, FILESIZE);

//     /* light it up! */
//     for (i = 0; i < NUM_WORDS; i++) {
// 	*(p + i) = RGB565_RED;
// 	delay(25);
//     }

//     /* flash white */
//     for (i = 0; i < 3; i++) {
// 	delay(250);
// 	memset(map, 0xFF, FILESIZE);
// 	delay(250);
// 	memset(map, 0, FILESIZE);
//     }
//     delay(250);

//     /* clear the led matrix */
//     memset(map, 0, FILESIZE);

//     /* un-map and close */
//     if (munmap(map, FILESIZE) == -1) {
// 	perror("Error un-mmapping the file");
//     }
//     close(fbfd);

//     return 0;
// }


void open_led_matrix(led_matrix_t *led_matrix){
    
    led_matrix->fbfd = open(LEDMATRIX_PATH, O_RDWR);
    if (led_matrix->fbfd == -1){
        perror("Error (call to 'open')");
        exit(EXIT_FAILURE);
    }

    if (ioctl(led_matrix->fbfd, FBIOGET_FSCREENINFO, &led_matrix->fix_info) == -1){
        perror("Error (call to 'ioctl')");
        close(led_matrix->fbfd);
        exit(EXIT_FAILURE);
    }

    if (strcmp(led_matrix->fix_info.id, "RPi-Sense FB") != 0){
        printf("%s\n", "Error: RPi-Sense FB not fount");
        close(led_matrix->fbfd);
        exit(EXIT_FAILURE);
    }

    led_matrix->map = mmap(NULL, FILESIZE, PROT_READ | PROT_WRITE, MAP_SHARED, led_matrix->fbfd, 0);
    if (led_matrix->map == MAP_FAILED){
        close(led_matrix->fbfd);
        perror("Error mmapping the file");
        exit(EXIT_FAILURE);
    }
    memset(led_matrix->map, 0, FILESIZE);
}

void close_led_matrix(led_matrix_t *led_matrix){
    memset(led_matrix->map, 0, FILESIZE);

    if (munmap(led_matrix->map, FILESIZE) == -1){
        perror("Error un-mmapping the file");
    }
    close(led_matrix->fbfd);
}

void delay(int t)
{
    usleep(t * 1000);
}
