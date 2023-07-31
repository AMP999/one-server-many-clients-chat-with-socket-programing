
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

class Connection {
public:
    int createSocket();
    int bind();
    int listen();
};