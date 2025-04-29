#include "defines.h"
#include "core/server.h"

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

  HTTP_Server http_server;
  http_server.port = atoi(argv[1]);

  server_init(&http_server);

  server_add_route("/", "html/index.html");
  server_add_route("/style.css", "html/style.css");
  server_add_route("/about", "html/about.html");
  server_add_route("/make", "Makefile");

  server_run(&http_server);

  return 0;
}
