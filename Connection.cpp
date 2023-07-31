#include "Connection.h"

int Connection::createSocket()
{
  int serv_sock_desc;
  int cli_desc;
  int port = 5001;
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
}

int Connection::bind()
{
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(port);
  if (bind(serv_sock_desc, (struct sockaddr *)&server, sizeof(server)) < 0) {
    puts("bind faild.");
    exit(0);
  }
  puts("bind done.");

}

int Connection::listen()
{
  listen(serv_sock_desc, 5);

  pthread_t thread1;
  int c = sizeof(struct sockaddr_in);

  puts("Waiting for connection ...");
}
