#ifndef _HTTPD_H___
#define _HTTPD_H___

#include <string.h>
#include <stdio.h>

// global variables from server.h
typedef enum _server_state_t {
	WF_USER = 0,
	IP_SELECT,
	WF_READY,
	IN_GAME,
	DP_RESULT,
} server_state_t;

typedef enum _html_t {
        READY = 0,
        INGAME,
        RESULT,
} html_t;

typedef enum _game_t {
        SLIDE_MASTER = 0,
        HIGH_OR_LOW,
        RAINFALL,
        COLOR_SWITCH,
} game_t;


server_state_t state;
int game;
int both_ready;
char result_buffer[64];

//Server control functions

void *serve_forever(void *port);

// Client request

char    *method,    // "GET" or "POST"
        *uri,       // "/index.html" things before '?'
        *qs,        // "a=1&b=2"     things after  '?'
        *prot;      // "HTTP/1.1"

char    *payload;     // for POST
int      payload_size;

char *request_header(const char* name);

// user shall implement this function

void route();
void create_html(char *dst, game_t game, html_t html);

// some interesting macro for `route()`
#define ROUTE_START()       if (0) {
#define ROUTE(METHOD,URI)   } else if (strcmp(URI,uri)==0&&strcmp(METHOD,method)==0) {
#define ROUTE_GET(URI)      ROUTE("GET", URI) 
#define ROUTE_POST(URI)     ROUTE("POST", URI) 
#define ROUTE_END()         } else printf(\
                                "HTTP/1.1 500 Not Handled\r\n\r\n" \
                                "The server has no handler to the request.\r\n" \
                            );

#endif