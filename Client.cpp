#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <strings.h>

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <pthread.h>

using namespace std;

#define MAX_SIZE 2000
#define PORT 4001

int main(int argc, char *argv[]) {
  if (argc < 4) {
    fprintf(stderr, "use like this : %s priority hostname port\n", argv[0]);
    exit(0);
  }

  struct hostent *server;
  int portNo = PORT;

  server = gethostbyname(argv[2]);

  if (server == NULL) {
    fprintf(stderr, "ERROR, no such host\n");
    exit(0);
  }

  portNo = atoi(argv[3]);
  int priority = atoi(argv[1]);

  int sock_desc;
  struct sockaddr_in serv_addr;
  char sbuff[MAX_SIZE], rbuff[MAX_SIZE];

  if ((sock_desc = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    printf("Failed creating socket\n");

  bzero((char *)&serv_addr, sizeof(serv_addr));

  serv_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr,
        server->h_length);
  serv_addr.sin_port = htons(portNo);

  if (connect(sock_desc, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) <
      0) {
    printf("Failed to connect to server\n");
    return -1;
  }

  // Send Priority
  send(sock_desc, argv[1], strlen(argv[1]), 0);

  if (recv(sock_desc, rbuff, MAX_SIZE, 0) == 0) {
    perror("Server Disconnected");
    close(sock_desc);
    exit(0);
  } else
    cout << "server: " << rbuff << endl;

  bzero(rbuff, MAX_SIZE);

  if (recv(sock_desc, rbuff, MAX_SIZE, MSG_DONTWAIT) != 0) {
    cout << "You: ";
    while (fgets(sbuff, MAX_SIZE, stdin) != NULL) {

      send(sock_desc, sbuff, strlen(sbuff), 0);
      if (recv(sock_desc, rbuff, MAX_SIZE, 0) == 0) {
        perror("Server Disconnected");
        close(sock_desc);
        exit(0);
      } else
        cout << "server: " << rbuff << endl;

      bzero(rbuff, MAX_SIZE); // clean buffer
      cout << "You: ";
    }
    close(sock_desc);
    return 0;
  }
}