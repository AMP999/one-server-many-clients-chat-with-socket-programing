
#ifndef TransmissionHandler_H
#define TransmissionHandler_H

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
#include <thread>

#define R_BUF_SIZE 2000
#define W_BUF_SIZE 2000

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



struct TransmissionHandler {
  void *cli_desc_;
  int recognize(int cli_desc);
  TransmissionHandler(void *cli_desc) : cli_desc_(cli_desc) {}
  void *operator()();
};

int TransmissionHandler::recognize(int cli_desc) {
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

void *TransmissionHandler::operator()() {

  int cli_desc = *(int *)cli_desc_;
  char *message;

  int newPrior = recognize(cli_desc);

  if (newPrior < 0) {
    std::cout << "Client didn't specified it's priority.";
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

  std::cout << "client " << cli_desc << "is connected successuly." << std::endl;

  message = "Greetings! you are connected to server successuly.\nNow type "
            "something and i shall repeat what you type.";
  write(cli_desc, message, strlen(message));
  while (prior == curPrior) {
    while ((cliMsgSize = recv(cli_desc, cliMsg, R_BUF_SIZE, MSG_DONTWAIT)) >
           0) {
      cliMsg[cliMsgSize] = '\0';
      std::cout << "client " << cli_desc << ": " << cliMsg << std::endl;
      write(cli_desc, cliMsg, strlen(cliMsg));
      memset(cliMsg, 0, R_BUF_SIZE);
    }

    if (cliMsgSize == 0) {
      std::cout << "client: " << cli_desc << " disconnected" << std::endl;
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

#endif