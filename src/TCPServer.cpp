#include "TCPServer.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>

TCPServer::TCPServer() {

}


TCPServer::~TCPServer() {

}

/**********************************************************************************************
 * bindSvr - Creates a network socket and sets it nonblocking so we can loop through looking for
 *           data. Then binds it to the ip address and port
 *
 *    Throws: socket_error for recoverable errors, runtime_error for unrecoverable types
 **********************************************************************************************/

int TCPServer::bindSvr(const char *ip_addr, short unsigned int port) {
    int socket1 = socket(AF_INET, SOCK_STREAM, 0);
    if (socket1 <= -1) {
        std::cerr << "Error creating socket";
        return -1;
    }

    int noblock = fcntl(socket1, F_SETFL, fcntl(socket1, F_GETFL, 0) | O_NONBLOCK);
   
    sockaddr_in svraddr;
    svraddr.sin_family = AF_INET;
    svraddr.sin_port = htons(port);
    inet_pton(AF_INET, "0.0.0.0", &svraddr.sin_addr); //127.0.0.1, may need to change to &ip_addr?

    if (bind(socket1, (sockaddr*)&svraddr, sizeof(svraddr)) == -1) {
        std::cerr << "Can't bind to IP/port";
        return -2;
    }

    if (listen(socket1, SOMAXCONN) == -1) {
        std::cerr << "Can't listen";
        return -3;
    }

    sockaddr_in client;
    socklen_t clientSize = sizeof(client);
    char host[NI_MAXHOST];
    char svc[NI_MAXSERV];

    int clientSocket = accept4(socket1, (sockaddr*)&client, &clientSize, SOCK_NONBLOCK);

    if (clientSocket == -1) {
        std::cerr << "Client cannot connect";
        return -4;
    }

    //Initialize memory to remove garbage
    memset(host, 0, NI_MAXHOST);
    memset(svc, 0, NI_MAXSERV);

    int nameConnect = getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, svc, NI_MAXSERV, 0);

    //Try to get host name, IP, and port
    if (nameConnect == 0) {
        std::cout << host << " connect on " << svc << std::endl;
    } else {
        inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
        std::cout << host << " connected on " << ntohs(client.sin_port) << std::endl;
    }

    return 0;
}

/**********************************************************************************************
 * listenSvr - Performs a loop to look for connections and create TCPConn objects to handle
 *             them. Also loops through the list of connections and handles data received and
 *             sending of data. 
 *
 *    Throws: socket_error for recoverable errors, runtime_error for unrecoverable types
 **********************************************************************************************/

void TCPServer::listenSvr() {


}

/**********************************************************************************************
 * shutdown - Cleanly closes the socket FD.
 *
 *    Throws: socket_error for recoverable errors, runtime_error for unrecoverable types
 **********************************************************************************************/

void TCPServer::shutdown() {
}
