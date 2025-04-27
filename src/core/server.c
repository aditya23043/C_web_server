#include "../defines.h"
#include "server.h"

typedef struct ROUTE {
  char path[128];
  void (*func)(int);
} ROUTE;

static ROUTE routes[MAX_ROUTES];
static int route_index = 0;
static int server_fd;

int server_init(int port) {

  /*  socket init */
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd == -1) {
    perror("Socket");
    exit(-1);
  }

  /*  socket config */
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = INADDR_ANY;

  /*  bind socket */
  if (bind(server_fd, (const struct sockaddr *)&addr, sizeof(addr)) != 0){
    perror("Bind");
    exit(-1);
  }

  /*  listen for request */
  // socket, backlog = number of connections that can be waiting until the
  // server starts rejecting the new connections
  if (listen(server_fd, BACKLOG) != 0) {
    perror("Listen");
    exit(-1);
  }

  printf("[SERVER] Listening on port: %d...\n", port);

  return server_fd;
}

void server_add_route(char* route, void (*func)(int)) {

  if (route_index >= MAX_ROUTES) {
    fprintf(stderr, "Max Routes Exceeded!");
    return;
  }

  strncpy(routes[route_index].path, route, sizeof(routes[route_index].path)-1);
  routes[route_index].func = func;
  route_index++;
}

void not_found(int client_fd) {
  char err_msg[256] = {0};
  sprintf(err_msg,"<style>*{font-family: \"Cascadia Code NF\"}</style><h1>File Not Found!</h1>");
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

void server_run(int server_fd) {
  while (1) {

    int client_fd = accept(server_fd, 0, 0);
    if (client_fd == -1) {
      perror("Client");
      continue;
    }
    printf("Client connected successfully!\n");

    pid_t pid = fork();
    if (pid < 0) {
      perror("Failed to fork");
      continue;
    }
    else if (pid == 0) {
      // child process
      char buffer[512] = {0};
      recv(client_fd, buffer, 512, 0);
      printf("[SERVER] Received: %s\n", buffer);

      // gets the browser args from the GET /file request
      char* f_arg = buffer+4;
      *strchr(f_arg, ' ') = 0;
      printf("Requested File: %s\n", f_arg);

      int file_fd = open(f_arg, O_RDONLY);
      if (file_fd < 0) {
        perror("Failed to open file");
      } else {

        off_t filesize = lseek(file_fd, 0L, SEEK_END);
        lseek(file_fd, 0, SEEK_SET);

        const char *body = "<style>*{font-family: \"Cascadia Code NF\"}</style><h1> Web Server in C</h1>";
        char response[256];
        sprintf(response,
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: %lu\r\n"
            "\r\n"
            "%s<plaintext>",
            strlen(body)+filesize+11, body);

        send(client_fd, response, strlen(response), 0);
        sendfile(client_fd, file_fd, 0, filesize);
        close(file_fd);
      }
      close(client_fd);
      exit(0);
    }
    else {
      // Main process
      close(client_fd);
    }
  }

  close(server_fd);

}

void handle_crucial(int expr, const char *message) {
  if (expr == -1) {
    perror(message);
    exit(-1);
  }
}
