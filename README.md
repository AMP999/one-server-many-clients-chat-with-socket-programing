#one server many clients chat with socket programing
in this project clients connect to the server and start messaging and server send back their message to them\
this program uses sockets to build connection between server and clients and each client will be precossed in seperate thread in the server.\
each client has it's own priority and only highest priorities can chat with server so if a client 
with higher priority connects to server every other clients with lower priority will be disconnected.\
this project runs on linux distros.\
\
for compiling:\
1.type g++ server.cpp -o server.out -lpthread\
2.type g++ client.cpp -o client.out\
for running thr project:\
1.run server by this rule: ./server.out "port" (put the port you want to use)\
2.run each client with this rule: ./client.out "ip" "port" "priority" (put server's ip and port and client's priority)\
\
note: \
1.run each client in separate terminal\
2.you can disconnect each client by pressing ctrl+c\