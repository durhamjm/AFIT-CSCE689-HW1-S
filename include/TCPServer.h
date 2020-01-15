#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "Server.h"
#include <sys/socket.h>
#include <arpa/inet.h>

class TCPServer : public Server 
{
public:
   TCPServer();
   ~TCPServer();

   void bindSvr(const char *ip_addr, unsigned short port);
   void listenSvr();
   void shutdown();

private:
 
int svrSocketFD, noblock, newClient, clientSocket[30], socketdesc, maxsocketdesc, activity, svraddrlen;
int valread;
char buffer[1025];
char const *message = "Hello user!\r\n";
int maxConns = 25;
int currConns = 0;
sockaddr_in svraddr;
sockaddr_in client;

};


#endif
