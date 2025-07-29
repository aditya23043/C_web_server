#include "server.h"
#include "../defines.h"

static Routes routes;
char *token;

void server_init(HTTP_Server *http_server) {

    /*  socket init */
    http_server->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (http_server->socket_fd == -1) {
        perror("Socket");
        exit(-1);
    }

    /*  socket config */
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(http_server->port);
    addr.sin_addr.s_addr = INADDR_ANY;

    /*  bind socket */
    if (bind(http_server->socket_fd, (const struct sockaddr *)&addr,
             sizeof(addr)) != 0) {
        perror("Bind");
        exit(-1);
    }

    /*  listen for request */
    // socket, backlog = number of connections that can be waiting until the
    // server starts rejecting the new connections
    if (listen(http_server->socket_fd, BACKLOG) != 0) {
        perror("Listen");
        exit(-1);
    }

    printf("[SERVER] Listening on port: %d...\n", http_server->port);

    routes.index = 0;
}

void server_add_route(char *route, char *file) {
    routes.arr[routes.index] = (struct node){route, file};
    routes.index++;
}

void server_run(HTTP_Server *http_server) {
    while (1) {

        int client_fd = accept(http_server->socket_fd, 0, 0);
        if (client_fd == -1) {
            perror("Client");
            continue;
        }
        printf("Client connected successfully!\n");

        pid_t pid = fork();
        if (pid < 0) {
            perror("Failed to fork");
            continue;
        } else if (pid == 0) {
            // child process
            char buffer[BUF_SIZE] = {0};
            recv(client_fd, buffer, BUF_SIZE, 0);
            /* printf("[SERVER] Received: %s\n", buffer); */

            /* GET or POST request */
            char method[BUF_SIZE];
            strncpy(method, buffer, BUF_SIZE);
            *strchr(method, ' ') = 0;

            if (strcmp(method, "GET") == 0) {
                handle_GET(client_fd, buffer);
            } else if (strcmp(method, "POST") == 0) {

                char *content_length_buf = strstr(buffer, "Content-Length: ");
                int content_length_num = 0;
                if (content_length_buf != NULL) {
                    sscanf(content_length_buf, "Content-Length: %d",
                           &content_length_num);

                } else {
                    printf("Empty POST request buffer!");
                    continue;
                }

                char *request_body = strstr(buffer, "\r\n\r\n");
                request_body += 4;
                request_body[content_length_num] = '\0';
                printf("[BODY]: %s\n", request_body);
            }

            close(client_fd);
            exit(0);
        } else {
            // Main process
            close(client_fd);
        }
    }

    close(http_server->socket_fd);
}

void handle_GET(int client_fd, char *buffer) {

    /* Extracts /file from the GET /file ... from the buffer */
    char *f_arg = buffer + 4;
    *strchr(f_arg, ' ') = 0;
    printf("Requested File: %s\n", f_arg);

    int file_served = 0;

    for (int i = 0; i < routes.index; i++) {
        if (strncmp(f_arg, routes.arr[i].arg, 256) == 0) {
            printf("F_ARG: %s, ROUTE: %s, FILE: %s\n", f_arg, routes.arr[i].arg,
                   routes.arr[i].file);
            int file_fd = open(routes.arr[i].file, O_RDONLY);
            if (file_fd < 0) {
                perror("Failed to open file");
            } else {
                off_t filesize = lseek(file_fd, 0L, SEEK_END);
                lseek(file_fd, 0, SEEK_SET);

                char *dot = strchr(routes.arr[i].file, '.');

                char *content_type = "text/plain";
                if (dot != NULL) {
                    if (strcmp(dot + 1, "html") == 0) {
                        content_type = "text/html";
                    } else if (strcmp(dot + 1, "css") == 0) {
                        content_type = "text/css";
                    } else if (strcmp(dot + 1, "js") == 0) {
                        content_type = "text/javascript";
                    }
                }
                /* printf("Content Type: %s\n", content_type); */

                char response[256];
                sprintf(response,
                        "HTTP/1.1 200 OK\r\n"
                        "Content-Type: %s\r\n"
                        "Content-Length: %lu\r\n"
                        "\r\n",
                        content_type, filesize);

                send(client_fd, response, strlen(response), 0);
                sendfile(client_fd, file_fd, 0, filesize);
                close(file_fd);
                file_served = 1;
            }
            break;
        }
    }

    // if the file mapped to the route does not exist or the route does not exist
    if (!file_served) {
        char err_msg[256] = {0};
        sprintf(err_msg, "<style>*{font-family: monospace"
                         "}html{background-color: #000;color: #dadada;}</style><h1 style=\"padding: 1rem;\">ERROR!<br>Route/File Not Found: %s</h1>", f_arg);
        char response[256] = {0};
        sprintf(response,
                "HTTP/1.1 404 NOT FOUND\r\n"
                "Content-Type: text/html\r\n"
                "Content-Length: %lu\r\n"
                "\r\n"
                "%s",
                strlen(err_msg), err_msg);
        send(client_fd, response, strlen(response), 0);
    }
}
