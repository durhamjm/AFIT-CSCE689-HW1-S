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
char const *message = "Welcome user! Please use these commands: hello, passwd, exit. For help, type menu.\r\n";
char const *response;
char ctemp1[256];
int itemp2;
int maxConns = 25;
int currConns = 0;
sockaddr_in svraddr;
sockaddr_in client;

};


#endif
