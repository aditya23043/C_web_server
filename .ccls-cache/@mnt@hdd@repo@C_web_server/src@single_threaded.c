#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <string.h>

/*
 * socket
 * bind
 * sockaddr_in
 * listen
 * accept
 * recv
 */

int main(int argc, char **argv) {

  if (argc != 2) {
    fprintf(stderr, "Usage: ./a.out <PORT>\n");
    exit(-1);
  }

  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd == -1) {
    perror("Socket");
    exit(-1);
  }

  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(atoi(argv[1]));
  // port: 8080 -> hex = 0x1f90 -> reverse = 0x901f
  addr.sin_addr.s_addr = INADDR_ANY;

  if (bind(server_fd, (const struct sockaddr *)&addr, sizeof(addr)) != 0){
    perror("Bind");
    exit(-1);
  }

  if (listen(server_fd, 10) != 0) {
    perror("Listen");
    exit(-1);
  }
  printf("Listening on port: %s...\n", argv[1]);
  // socket, backlog = number of connections that can be waiting until the
  // server starts rejecting the new connections

  int client_fd = accept(server_fd, 0, 0);
  if (client_fd == -1) {
    perror("Client");
    exit(-1);
  }
  printf("Client connected successfully!\n");

  char buffer[256] = {0};
  recv(client_fd, buffer, 256, 0);

  // gets the browser args from the GET /file request
  char* f_arg = buffer+5;
  *strchr(f_arg, ' ') = 0;

  int file_fd = open(f_arg, O_RDONLY);

  const char *body = "<h1> Web Server in C</h1>";
  char response[256];
  sprintf(response,
      "HTTP/1.1 200 OK\r\n"
      "Content-Type: text/html\r\n"
      "Content-Length: %lu\r\n"
      "\r\n"
      "%s",
      strlen(body), body);

  send(client_fd, response, strlen(response), 0);

  close(file_fd);
  close(client_fd);
  close(server_fd);

  return 0;
}
