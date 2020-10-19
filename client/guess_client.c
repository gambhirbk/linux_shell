#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdbool.h>
#define MAXLINE 80

char resp[1000];

int do_connect(char *host, char *port) {
  int s;
  int sock_fd = socket(AF_INET, SOCK_STREAM, 0);

  struct addrinfo hints, *result;

  // Allows "global"
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_INET; /* IPv4 only */

  hints.ai_socktype = SOCK_STREAM; /* TCP */

  s = getaddrinfo(host, port, &hints, &result);

  // If I can't get the address, write an error.
  if (s != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
    exit(1);
  }
  // Try to connect; if I can't, write an error.
  if (connect(sock_fd, result->ai_addr, result->ai_addrlen) == -1) {
    perror("connect");
    exit(2);
  } else {
    printf("Connection is good!\n");
  }

  freeaddrinfo(result);

  return sock_fd;
}

void send_message(char *msg, int sock_fd) {
  printf("SENDING: %s", msg);
  printf("===\n");
  write(sock_fd, msg, sizeof(msg));
}

int read_response(int sock_fd) {
  // Response
  char resp[1000];
  char *end = "You guessed my secret number!";
  int len = read(sock_fd, resp, 999);
  resp[len] = '\0';
  printf("RECEIVED: %s\n", resp);
  if (strncmp(resp, end, sizeof(strlen(end))) == 0) {
      return 0;
    } else {
      return 1;
  }
}

char *Fgets(char *prt, int n, FILE *stream) {
  char *rptr;
  if (((rptr = fgets(prt, n, stream)) == NULL) && ferror(stream)) {
    fprintf(stderr, "%s\n", "fgets error");
    exit(0);
  } else {
    return rptr;
  }
}

int main(int argc, char **argv) {
  char cmdline[MAXLINE];
  char *command;

  int sock_fd = do_connect("localhost", "7533");

  printf("socket fd (client): %d\n", sock_fd);

  int receive_msg = read_response(sock_fd);

  while (receive_msg != 0) {
    // read_response(sock_fd);

    printf("client: ");
    Fgets(cmdline, MAXLINE, stdin);  // reads command

    // send message after reading the command
    char guess[sizeof(cmdline)];

    // char guess = (char*) malloc(sizeof(cmdline));

    strncpy(guess, cmdline, sizeof(cmdline));

    send_message(cmdline, sock_fd);

    // read back message coming from the server
    receive_msg = read_response(sock_fd);
  }
  close(sock_fd);
}
