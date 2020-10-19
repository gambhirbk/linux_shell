#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <arpa/inet.h>

const int LOWER_RANGE  = 1;
const int UPPER_RANGE = 50;

int do_open() {
  int s;

  // Step 1: Get Address stuff
  struct addrinfo hints, *result;

  // Setting up the hints struct...
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  s = getaddrinfo("localhost", "7533", &hints, &result);
  if (s != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
    exit(1);
  }

  // Step 2: Create the socket
  int sock_fd = socket(AF_INET, SOCK_STREAM, 0);

  // Step 3: Bind the socket
  if (bind(sock_fd, result->ai_addr, result->ai_addrlen) != 0) {
    perror("bind()");
    exit(1);
  }

  // Step 4: Listen
  if (listen(sock_fd, 10) != 0) {
    perror("listen()");
    exit(1);
  }

  struct sockaddr_in *result_addr = (struct sockaddr_in *) result->ai_addr;
  printf("Listening on file descriptor %d, port %d\n",
       sock_fd, ntohs(result_addr->sin_port));

  // Step 5: Accept a connection
  printf("Waiting for connection...\n");
  int client_fd = accept(sock_fd, NULL, NULL);
  printf("Connection made: client_fd=%d\n", client_fd);
  freeaddrinfo(result);
  return client_fd;
}

int recieve_message(int client_fd) {
  int guess = 0;
  char buffer[1000];
  int len = read(client_fd, buffer, sizeof(buffer) - 1);
  buffer[len] = '\0';
  printf("SERVER RECEIVED: %s \n", buffer);
  guess = atoi(buffer);
  return guess;
}

void send_message(char *msg, int sock_fd) {
  printf("SERVER SENDING: %s", msg);
  printf("===\n");
  write(sock_fd, msg, strlen(msg));
}

// random number generator
int generate_random_number(int lower_range, int upper_range) {
  srand(time(0));
  return (rand() % (upper_range - lower_range + 1)) + lower_range;
}

int main() {
  int num_of_guesses = 0;
  int invalid_input_counter = 0;
  int client_socket_id = do_open();

  // generate a random number
  bool found = false;
  int random_number = generate_random_number(LOWER_RANGE, UPPER_RANGE);

send_message("Welcome! I have guessed a number between 1 and 50. Guess it!",
       client_socket_id);

  int guess = recieve_message(client_socket_id);

  while (guess != random_number) {
    num_of_guesses++;
    if (guess == 0) {
      send_message("I don't recognize that number. ", client_socket_id);
      send_message("Please guess a number between 1 and 50.\n",
       client_socket_id);
      invalid_input_counter++;
    } else if (guess > random_number) {
      send_message("The secret number is smaller.\n", client_socket_id);
    } else if (guess < random_number) {
      send_message("The secret number is higher\n", client_socket_id);
    }
    // Step 6: Read and write
    guess = recieve_message(client_socket_id);
  }

  num_of_guesses++;

  char win_buff[100];
  int win = sprintf(win_buff, "It only took you %d guesses ", num_of_guesses);
  send_message("You guessed my secret number! ", client_socket_id);
  send_message(win_buff, client_socket_id);
  if (invalid_input_counter > 0) {
    send_message("plus something I didn't understand. ", client_socket_id);
  }
  // Step 7: Close
  close(client_socket_id);
  return 0;
}

