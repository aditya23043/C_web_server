#pragma once

#define MAX_ROUTES 10

typedef struct HTTP_Server {
    int socket_fd;
    int port;
} HTTP_Server;

struct node {
    char *arg;
    char *file;
};

typedef struct Routes {
    int index;
    struct node arr[MAX_ROUTES];
} Routes;

void server_init(HTTP_Server *http_server);
void server_run(HTTP_Server *http_server);
void server_add_route(char *route, char *file);
