#include "httpd.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

char temp[8096];
int fd;

int main(int c, char** v)
{
    serve_forever("12913");
    return 0;
}

void route()
{

    ROUTE_START()

    ROUTE_GET("/") {
        printf("HTTP/1.1 200 OK\r\n\r\n");
        fd = open("index.html", O_RDONLY);
        read(fd, temp, 8096);
        printf("%s", temp);
    }
    
    /* ========================= READY ========================= */

    ROUTE_GET("/slide_master/ready.html") {
        printf("HTTP/1.1 200 OK\r\n\r\n");
        fd = open("slide_master/ready.html", O_RDONLY);
        read(fd, temp, 8096);
        printf("%s", temp);
    }

    ROUTE_GET("/high_or_low/ready.html") {
        printf("HTTP/1.1 200 OK\r\n\r\n");
        fd = open("high_or_low/ready.html", O_RDONLY);
        read(fd, temp, 8096);
        printf("%s", temp);
    }

    ROUTE_GET("/rainfall/ready.html") {
        printf("HTTP/1.1 200 OK\r\n\r\n");
        fd = open("rainfall/ready.html", O_RDONLY);
        read(fd, temp, 8096);
        printf("%s", temp);
    }

    ROUTE_GET("/color_switch/ready.html") {
        printf("HTTP/1.1 200 OK\r\n\r\n");
        fd = open("color_switch/ready.html", O_RDONLY);
        read(fd, temp, 8096);
        printf("%s", temp);
    }

    /* ========================= INGAME ========================= */

    ROUTE_GET("/slide_master/ingame.html") {
        printf("HTTP/1.1 200 OK\r\n\r\n");
        fd = open("slide_master/ingame.html", O_RDONLY);
        read(fd, temp, 8096);
        printf("%s", temp);
    }

    ROUTE_GET("/high_or_low/ingame.html") {
        printf("HTTP/1.1 200 OK\r\n\r\n");
        fd = open("high_or_low/ingame.html", O_RDONLY);
        read(fd, temp, 8096);
        printf("%s", temp);
    }

    ROUTE_GET("/rainfall/ingame.html") {
        printf("HTTP/1.1 200 OK\r\n\r\n");
        fd = open("rainfall/ingame.html", O_RDONLY);
        read(fd, temp, 8096);
        printf("%s", temp);
    }

    ROUTE_GET("/color_switch/ingame.html") {
        printf("HTTP/1.1 200 OK\r\n\r\n");
        fd = open("color_switch/ingame.html", O_RDONLY);
        read(fd, temp, 8096);
        printf("%s", temp);
    }

    /* ========================= RESULT ========================= */

    ROUTE_GET("/slide_master/result.html") {
        printf("HTTP/1.1 200 OK\r\n\r\n");
        fd = open("slide_master/result.html", O_RDONLY);
        read(fd, temp, 8096);
        printf("%s", temp);
    }

    ROUTE_GET("/high_or_low/result.html") {
        printf("HTTP/1.1 200 OK\r\n\r\n");
        fd = open("high_or_low/result.html", O_RDONLY);
        read(fd, temp, 8096);
        printf("%s", temp);
    }

    ROUTE_GET("/rainfall/result.html") {
        printf("HTTP/1.1 200 OK\r\n\r\n");
        fd = open("rainfall/result.html", O_RDONLY);
        read(fd, temp, 8096);
        printf("%s", temp);
    }

    ROUTE_GET("/color_switch/result.html") {
        printf("HTTP/1.1 200 OK\r\n\r\n");
        fd = open("color_switch/result.html", O_RDONLY);
        read(fd, temp, 8096);
        printf("%s", temp);
    }
    

    ROUTE_POST("/") {
        printf("HTTP/1.1 200 OK\r\n\r\n");
        printf("Wow, seems that you POSTed %d bytes. \r\n", payload_size);
        printf("Fetch the data using `payload` variable.");
    }
  
    ROUTE_END()
}
