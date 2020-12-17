#include "httpd.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h> 

#define CONNMAX 1000

static int listenfd, clients[CONNMAX];
static void error(char *);
static void startServer(const char *);
static void respond(int);

typedef struct { char *name, *value; } header_t;
static header_t reqhdr[17] = { {"\0", "\0"} };
static int clientfd;

static char *buf;

// global variables for route()
char temp[8096];
int fd;

void *serve_forever(void *port)
{
    struct sockaddr_in clientaddr;
    socklen_t addrlen;
    char c; 
    
    int slot=0;

    int fd_pipe[2];
    char buffer[32];
    pid_t pid;    
    
    const char *PORT = (char *) port;

    printf(
            "Server started %shttp://127.0.0.1:%s%s\n",
            "\033[92m",PORT,"\033[0m"
            );

    // Setting all elements to -1: signifies there is no client connected
    int i;
    for (i=0; i<CONNMAX; i++)
        clients[i]=-1;
    startServer(PORT);
    
    // Ignore SIGCHLD to avoid zombie threads
    signal(SIGCHLD,SIG_IGN);

    // ACCEPT connections
    while (1)
    {
        addrlen = sizeof(clientaddr);
        clients[slot] = accept (listenfd, (struct sockaddr *) &clientaddr, &addrlen);

        if (clients[slot]<0)
        {
            perror("accept() error");
        }
        else
        {   
            /* return value of fork()
                -1  : error
                0   : child process
                pid : parent process 
            */

            pipe(fd_pipe);
            pid = fork();

            if (pid == 0) { 
                // child process
                respond(slot);
                // printf("h3 - %d %p\n", game, &game);
                
                sprintf(buffer, "%d", game);
                write(fd_pipe[1], buffer, 32);

                exit(0);
            }
            else if (pid > 0) { 
                // process process
                read(fd_pipe[0], buffer, 32);
                game = atoi(buffer);
            }
        }

        while (clients[slot]!=-1) slot = (slot+1)%CONNMAX;
    }
}

//start server
void startServer(const char *port)
{
    struct addrinfo hints, *res, *p;

    // getaddrinfo for host
    memset (&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if (getaddrinfo( NULL, port, &hints, &res) != 0)
    {
        perror ("getaddrinfo() error");
        exit(1);
    }
    // socket and bind
    for (p = res; p!=NULL; p=p->ai_next)
    {
        int option = 1;
        listenfd = socket (p->ai_family, p->ai_socktype, 0);
        setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
        if (listenfd == -1) continue;
        if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0) break;
    }
    if (p==NULL)
    {
        perror ("socket() or bind()");
        exit(1);
    }

    freeaddrinfo(res);

    // listen for incoming connections
    if ( listen (listenfd, 1000000) != 0 )
    {
        perror("listen() error");
        exit(1);
    }
}


// get request header
char *request_header(const char* name)
{
    header_t *h = reqhdr;
    while(h->name) {
        if (strcmp(h->name, name) == 0) return h->value;
        h++;
    }
    return NULL;
}

//client connection
void respond(int n)
{
    int rcvd, fd, bytes_read;
    char *ptr;

    buf = malloc(65535);
    rcvd=recv(clients[n], buf, 65535, 0);

    if (rcvd<0)    // receive error
        fprintf(stderr,("recv() error\n"));
    else if (rcvd==0)    // receive socket closed
        fprintf(stderr,"Client disconnected upexpectedly.\n");
    else    // message received
    {
        buf[rcvd] = '\0';

        method = strtok(buf,  " \t\r\n");
        uri    = strtok(NULL, " \t");
        prot   = strtok(NULL, " \t\r\n"); 

        fprintf(stderr, "\x1b[32m + [%s] %s\x1b[0m\n", method, uri);
        
        if (qs = strchr(uri, '?'))
        {
            *qs++ = '\0'; //split URI
        } else {
            qs = uri - 1; //use an empty string
        }

        header_t *h = reqhdr;
        char *t, *t2;
        while(h < reqhdr+16) {
            char *k,*v,*t;
            k = strtok(NULL, "\r\n: \t"); if (!k) break;
            v = strtok(NULL, "\r\n");     while(*v && *v==' ') v++;
            h->name  = k;
            h->value = v;
            h++;
            fprintf(stderr, "[H] %s: %s\n", k, v);
            t = v + 1 + strlen(v);
            if (t[1] == '\r' && t[2] == '\n') break;
        }
        // t++; // now the *t shall be the beginning of user payload
        t = strtok(NULL, "\r\n");
        t2 = request_header("Content-Length"); // and the related header if there is  
        payload = t;
        payload_size = t2 ? atol(t2) : (rcvd-(t-buf));

        // bind clientfd to stdout, making it easier to write
        clientfd = clients[n];
        dup2(clientfd, STDOUT_FILENO);
        close(clientfd);

        // call router
        route();

        // tidy up
        fflush(stdout);
        shutdown(STDOUT_FILENO, SHUT_WR);
        close(STDOUT_FILENO);
    }

    //Closing SOCKET
    shutdown(clientfd, SHUT_RDWR);         //All further send and recieve operations are DISABLED...
    close(clientfd);
    clients[n]=-1;

    free(buf);
}


void route()
{

    ROUTE_START()

    ROUTE_GET("/") {
        printf("HTTP/1.1 200 OK\r\n\r\n");
        fd = open("html/index.html", O_RDONLY);
        read(fd, temp, 8096);
        printf("%s", temp);
    }
    
    /* ========================= READY ========================= */

    ROUTE_GET("/slide_master/ready.html") {
        game = 1;
        printf("HTTP/1.1 200 OK\r\n\r\n");
        create_html(temp, SLIDE_MASTER, READY);
        printf("%s", temp);
    }

    ROUTE_GET("/high_or_low/ready.html") {
        game = 2;
        printf("HTTP/1.1 200 OK\r\n\r\n");
        create_html(temp, HIGH_OR_LOW, READY);
        printf("%s", temp);
    }

    ROUTE_GET("/rainfall/ready.html") {
        game = 3;
        printf("HTTP/1.1 200 OK\r\n\r\n");
        create_html(temp, RAINFALL, READY);
        printf("%s", temp);
    }

    ROUTE_GET("/color_switch/ready.html") {
        game = 4;
        printf("HTTP/1.1 200 OK\r\n\r\n");
        create_html(temp, COLOR_SWITCH, READY);
        printf("%s", temp);
    }

    /* ========================= INGAME ========================= */

    ROUTE_GET("/slide_master/ingame.html") {
        if (both_ready){
            printf("HTTP/1.1 200 OK\r\n\r\n");
            create_html(temp, SLIDE_MASTER, INGAME);
            printf("%s", temp);
        }
    }

    ROUTE_GET("/high_or_low/ingame.html") {
        if (both_ready){
            printf("HTTP/1.1 200 OK\r\n\r\n");
            create_html(temp, HIGH_OR_LOW, INGAME);
            printf("%s", temp);
        }
    }

    ROUTE_GET("/rainfall/ingame.html") {
        if (both_ready){
            printf("HTTP/1.1 200 OK\r\n\r\n");
            create_html(temp, RAINFALL, INGAME);
            printf("%s", temp);
        }
    }

    ROUTE_GET("/color_switch/ingame.html") {
        if (both_ready){
            printf("HTTP/1.1 200 OK\r\n\r\n");
            create_html(temp, COLOR_SWITCH, INGAME);
            printf("%s", temp);
        }
    }

    /* ========================= RESULT ========================= */

    ROUTE_GET("/slide_master/result.html") {
        printf("HTTP/1.1 200 OK\r\n\r\n");
        create_html(temp, SLIDE_MASTER, RESULT);
        printf("%s", temp);
    }

    ROUTE_GET("/high_or_low/result.html") {
        printf("HTTP/1.1 200 OK\r\n\r\n");
        create_html(temp, HIGH_OR_LOW, RESULT);
        printf("%s", temp);
    }

    ROUTE_GET("/rainfall/result.html") {
        printf("HTTP/1.1 200 OK\r\n\r\n");
        create_html(temp, RAINFALL, RESULT);
        printf("%s", temp);
    }

    ROUTE_GET("/color_switch/result.html") {
        printf("HTTP/1.1 200 OK\r\n\r\n");
        create_html(temp, COLOR_SWITCH, RESULT);
        printf("%s", temp);
    }
    

    ROUTE_POST("/") {
        printf("HTTP/1.1 200 OK\r\n\r\n");
        printf("Wow, seems that you POSTed %d bytes. \r\n", payload_size);
        printf("Fetch the data using `payload` variable.");
    }
  
    ROUTE_END()
}

void create_html(char *dst, game_t game, html_t html) {

    char* name[4] = {"Slide master", "High or low", "Rainfall" ,"Color switch"};
    char* desc[3] = {"Please ready...", "now in game...", "Game result"};
    char* page[3] = {"ingame.html", "result.html", "../"};

    char *a = "<!DOCTYPE html><html><head><title>";
    char *b = "</title><meta charset=\"UTF-8\"></head><body><h1>";
    char *c = "</h1><h2>";
    char *d = "</h2>";
    char *e = "<input type=\"button\" value=\"next\" onclick=\"location.href='";
    char *f = "'\"></body></html>";

    strcpy(dst, a);
    strcat(dst, name[game]);
    strcat(dst, b);
    strcat(dst, name[game]);
    strcat(dst, c);
    strcat(dst, desc[html]);
    strcat(dst, d);

    if (html == RESULT) {
        strcat(dst, result_buffer);
    }

    strcat(dst, e);
    strcat(dst, page[html]);
    strcat(dst, f);
}