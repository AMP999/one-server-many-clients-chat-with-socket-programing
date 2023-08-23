
#include <arpa/inet.h>
#include <errno.h>
#include <iostream>
#include <pthread.h>
#include <queue>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAX_CLI 5
#define R_BUF_SIZE 2000
#define W_BUF_SIZE 2000

using namespace std;

struct packet {
  int cli_desc;
  int prior;
};

// Global Variables
int curPrior = 0;
int clients = 0;

pthread_t thread1;
pthread_mutex_t authorization_lock, thr_ending_waiter, mutex1;
pthread_cond_t thr_end_ctrler;

int recognize(int cli_desc) {
  char cliMsg[10];
  bzero(cliMsg, 10);

  int prior = 0, cliMsgSize;

  if ((cliMsgSize = recv(cli_desc, cliMsg, 10, 0)) > 0) {
    cliMsg[cliMsgSize] = '\0';

    prior = atoi(cliMsg);
    return prior;
  }

  if (cliMsgSize == 0) {
    puts("Client disconnected");
    fflush(stdout);
    return -1;
  } else if (cliMsgSize == -1) {
    perror("recv failed");
    return -1;
  }
}

void *transmission_handler(void *cli_desc1) {

  int cli_desc = *(int *)cli_desc1;
  char *message;

  int newPrior = recognize(cli_desc);

  if (newPrior < 0) {
    cout << "Client didn't specified it's priority.";
    close(cli_desc);
    pthread_exit(NULL);
  }

  // Decide whether to connect client or not
  // mutex vase in mikhad ke age masalan 2 , 3 , 4 baham kanect bedan
  // momkene aval 2 bere bad 3 ham bere bad 4 beran tu ghesmate 1 va 3 , 4 bahan
  // kanect beshan

  pthread_mutex_lock(&authorization_lock);

  if (newPrior > curPrior || clients == 0) {

    curPrior = newPrior;
    if (clients != 0) {
      pthread_cond_wait(&thr_end_ctrler,
                        &thr_ending_waiter); //-------------------------
      // pthread_cond_broadcast(&thr_end_ctrler);//it let threads do the ending
      // process pthread_mutex_lock(&thr_ending_waiter);
    }
    clients++;
  }

  else if (newPrior == curPrior) {
    clients++;
  }

  else if (newPrior < curPrior) {
    message = "You don't have enough priority.";
    write(cli_desc, message, strlen(message));
    close(cli_desc);
    pthread_mutex_unlock(&authorization_lock);
    pthread_exit(NULL);
  }

  pthread_mutex_unlock(&authorization_lock);

  int prior = newPrior;

  char cliMsg[R_BUF_SIZE];
  int cliMsgSize = 0;
  memset(cliMsg, 0, R_BUF_SIZE);

  cout << "client " << cli_desc << "is connected successuly." << endl;

  message = "Greetings! you are connected to server successuly.\nNow type "
            "something and i shall repeat what you type.";
  write(cli_desc, message, strlen(message));
  while (prior == curPrior) {
    while ((cliMsgSize = recv(cli_desc, cliMsg, R_BUF_SIZE, MSG_DONTWAIT)) >
           0) {
      cliMsg[cliMsgSize] = '\0';
      cout << "client " << cli_desc << ": " << cliMsg << endl;
      write(cli_desc, cliMsg, strlen(cliMsg));
      memset(cliMsg, 0, R_BUF_SIZE);
    }

    if (cliMsgSize == 0) {
      cout << "client: " << cli_desc << " disconnected" << endl;
      close(cli_desc);
      clients--;
      fflush(stdout);
      return 0;
    } else if (errno != EAGAIN) {
      perror("recv failed");
      close(cli_desc);
      clients--;
      return 0;
    }
  }

  // Ending Process--------------------------
  pthread_mutex_lock(&mutex1); //----------------

  close(cli_desc);
  clients--;
  if (clients == 0) {
    pthread_cond_signal(&thr_end_ctrler); //------------------------
  }

  pthread_mutex_unlock(&mutex1); //--------------
  return 0;
}

int main(int argc, char *argv[]) {

  int serv_sock_desc, cli_desc, port = 5001;
  struct sockaddr_in server, client;

  if (argc == 2) {
    port = atoi(argv[1]);
  }

  serv_sock_desc = socket(AF_INET, SOCK_STREAM, 0);
  if (serv_sock_desc == -1) {
    puts("could'nt create socket.");
    exit(0);
  }
  puts("Socket created");

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(port);
  if (bind(serv_sock_desc, (struct sockaddr *)&server, sizeof(server)) < 0) {
    puts("bind faild.");
    exit(0);
  }
  puts("bind done.");

  listen(serv_sock_desc, 5);

  pthread_t thread1;
  int c = sizeof(struct sockaddr_in);

  puts("Waiting for connection ...");

  pthread_mutex_init(&authorization_lock, NULL);
  pthread_mutex_init(&thr_ending_waiter, NULL);
  pthread_mutex_init(&mutex1, NULL);
  pthread_cond_init(&thr_end_ctrler, NULL);

  while (1) {
    while (clients < MAX_CLI) {
      if (cli_desc = accept(serv_sock_desc, (struct sockaddr *)&client,
                            (socklen_t *)&c)) {
        if (pthread_create(&thread1, NULL, transmission_handler,
                           (void *)&cli_desc) < 0) {
          perror("could not create thread");
          return 1;
        }
      }
    }
  }
}
