#include "TransmissionHandler.h"
#include <arpa/inet.h>
#include <errno.h>
#include <iostream>
#include <pthread.h>
#include <queue>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

#define MAX_CLI 5

using namespace std;

int main(int argc, char *argv[]) {

  int serverSocketDescriptor, cli_desc, port = 5001;
  struct sockaddr_in server, client;

  // This is my expectation from `main()`
  // TransmissionHandler transmissionHandler;
  // transmissionHandler.createSocket();
  // transmissionHandler.bind();
  // transmissionHandler.listen();

  if (argc == 2) {
    port = atoi(argv[1]);
  }

  serverSocketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
  if (serverSocketDescriptor == -1) {
    puts("could'nt create socket.");
    exit(0);
  }
  puts("Socket created");

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(port);
  if (bind(serverSocketDescriptor, (struct sockaddr *)&server, sizeof(server)) <
      0) {
    puts("bind faild.");
    exit(0);
  }
  puts("bind done.");

  listen(serverSocketDescriptor, 5);

  int socketLen = sizeof(struct sockaddr_in);

  puts("Waiting for connection ...");

  pthread_mutex_init(&authorization_lock, NULL);
  pthread_mutex_init(&thr_ending_waiter, NULL);
  pthread_mutex_init(&mutex1, NULL);
  pthread_cond_init(&thr_end_ctrler, NULL);

  while (1) {
    while (clients < MAX_CLI) {
      if (cli_desc = accept(serverSocketDescriptor, (struct sockaddr *)&client,
                            (socklen_t *)&socketLen)) {
        std::jthread thread(TransmissionHandler((void *)&cli_desc));
      }
    }
  }
}
