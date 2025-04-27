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

  server_init(atoi(argv[1]));
  server_run();

  return 0;
}
