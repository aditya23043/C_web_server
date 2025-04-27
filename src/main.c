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
    } else if (pid == 0) {
      // child process
      char buffer[256] = {0};
      recv(client_fd, buffer, 256, 0);

      // gets the browser args from the GET /file request
      char* f_arg = buffer+5;
      *strchr(f_arg, ' ') = 0;
      printf("Requested File: %s\n", f_arg);

      int file_fd = open(f_arg, O_RDONLY);
      if (file_fd < 0) {
        perror("Failed to open file");
        char err_msg[256] = {0};
        sprintf(err_msg,"<style>*{font-family: \"Cascadia Code NF\"}</style><h1>File Not Found: %s</h1>", f_arg);
        char response[256] = {0};
        sprintf(response,
                "HTTP/1.1 404 NOT FOUND\r\n"
                "Content-Type: text/html\r\n"
                "Content-Length: %lu\r\n"
                "\r\n"
                "%s",
                strlen(err_msg), err_msg);
        send(client_fd, response, strlen(response), 0);
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
    } else {
      // Main process
      close(client_fd);
    }
  }

  close(server_fd);

  return 0;
}
