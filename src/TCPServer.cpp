#include "TCPServer.h"
#include "buffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>

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

//Adapted from youtube.com/watch?v=cNdlrbZSkyQ
void TCPServer::bindSvr(const char *ip_addr, short unsigned int port) {
    svrSocketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (svrSocketFD <= -1) {
        std::cerr << "Error creating socket\n";
        return;
    }

    noblock = fcntl(svrSocketFD, F_SETFL, fcntl(svrSocketFD, F_GETFL, 0) | O_NONBLOCK);
   
    svraddr.sin_family = AF_INET;
    svraddr.sin_port = htons(port);
    inet_pton(AF_INET, ip_addr, &svraddr.sin_addr); //127.0.0.1, may need to change to &ip_addr?

    if (bind(svrSocketFD, (sockaddr*)&svraddr, sizeof(svraddr)) == -1) {
        std::cerr << "Can't bind to IP/port\n";
        return;
    }

 
    return;
}

/**********************************************************************************************
 * listenSvr - Performs a loop to look for connections and create TCPConn objects to handle
 *             them. Also loops through the list of connections and handles data received and
 *             sending of data. 
 *
 *    Throws: socket_error for recoverable errors, runtime_error for unrecoverable types
 **********************************************************************************************/

//Portions adapted from www.geeksforgeeks.org/socket-programming-in-cc-handling-multiple-clients-on-server-without-multi-threading/
void TCPServer::listenSvr() {
   if (listen(svrSocketFD, SOMAXCONN) == -1) {
        std::cerr << "Can't listen\n";
        return;
    }

    client.sin_addr = svraddr.sin_addr;
    client.sin_port = svraddr.sin_port;
    socklen_t clientSize = sizeof(client);

    struct timeval tv = {1, 0};

    //Initialize client sockets to avoid fake connections
    int i;
    for (i = 0; i <= maxConns; i++) {
        clientSocket[i] = 0;
    }
    svraddrlen = sizeof(svraddr);
        while(true) {
            //Preps the socket for use with select
            fd_set read_fd;
            FD_ZERO(&read_fd);
            FD_SET(svrSocketFD, &read_fd);

            for (i = 0; i <= maxConns; i++) {
                socketdesc = clientSocket[i];
                if (socketdesc > 0) FD_SET(socketdesc, &read_fd);
                if (socketdesc > maxsocketdesc) { 
                    maxsocketdesc = socketdesc;
                }
            }

            activity = select(maxsocketdesc+1, &read_fd, NULL, NULL, &tv) > 0;
            if ((activity < 0) && (errno!=EINTR)) { 
                std::cerr << "Error on select"; 
            }

            if (FD_ISSET(svrSocketFD, &read_fd)) {
                if ((newClient = accept(svrSocketFD, (struct sockaddr *)&svraddr, (socklen_t*)&svraddrlen)) < 0) {
                    std::cerr << "Error accepting client";
                    return;
                }
                //Try to get host name and port
                printf("Connection established with %s on port %d over socket %d.\n", inet_ntoa(svraddr.sin_addr), ntohs(svraddr.sin_port), newClient);
                
                if (send(newClient, message, strlen(message),0) != strlen(message)) {
                    perror("Error sending message");
                }

                puts("Welcome message sent successfully");

                for (i = 0; i < maxConns; i++) {
                    if (clientSocket[i] == 0) {
                        clientSocket[i] = newClient;
                        printf("Adding client to list of sockets as #%d.\n", newClient); //testing
                        currConns++;
                        break;
                    }
                }
            }

            //Add clients to array, check for input
            for (i = 0; i < maxConns; i++) {
                socketdesc = clientSocket[i];

                if (FD_ISSET(socketdesc, &read_fd)) {
                    //testing for removal of this part
                    if ((valread = read(socketdesc, buffer, 1024)) == 0) {
                        //closing, needs moved
                        getpeername(socketdesc, (struct sockaddr*)&client, (socklen_t*)&client);
                        printf("Client at %s:%d disconnected.\n",inet_ntoa(svraddr.sin_addr), ntohs(svraddr.sin_port));

                        close(socketdesc);
                        clientSocket[i] = 0;
                    } 
                    //handling input
                    else {
                        buffer[strcspn(buffer, "\n")] = '\0';
                        buffer[strcspn(buffer, "\r")] = '\0';
                        printf("Client at %s:%d sent: %s\n",inet_ntoa(svraddr.sin_addr), ntohs(svraddr.sin_port),buffer);
                        if (!strcmp(buffer, "hello")) {
                            send(socketdesc, message, strlen(message), 0);
                        } 
                        else if (!strcmp(buffer, "menu")) {
                            response = "hello - Displays a greeting.\nmenu - Displays this menu.\nexit - Closes the connection.\npasswd - Change your password.\n1 - Displays your IP.\n2 - Displays your port.\n3 - Displays your socket number.\n4 - Displays number of current connections.\n5 - Displays number of maximum connections.\n";
                            send(socketdesc, response, strlen(response), 0);
                        } 
                        else if (!strcmp(buffer, "1")) {
                            sprintf(ctemp1, "Your IP is: %s.\n", inet_ntoa(svraddr.sin_addr));
                            send(socketdesc, ctemp1, strlen(ctemp1), 0);
                        }
                        else if (!strcmp(buffer, "2")) {
                            sprintf(ctemp1, "Your port is: %d.\n", ntohs(svraddr.sin_port));
                            send(socketdesc, ctemp1, strlen(ctemp1), 0);
                        }
                        else if (!strcmp(buffer, "3")) {
                            sprintf(ctemp1, "Your socket number is: %d.\n", clientSocket[i]);
                            send(socketdesc, ctemp1, strlen(ctemp1), 0);
                        }
                        else if (!strcmp(buffer, "4")) {
                            sprintf(ctemp1, "There are %d current connections.\n", currConns);
                            send(socketdesc, ctemp1, strlen(ctemp1), 0);
                        }
                        else if (!strcmp(buffer, "5")) {
                            sprintf(ctemp1, "There can be up to %d simultaneous connections.\n", maxConns);
                            send(socketdesc, ctemp1, strlen(ctemp1), 0);
                        }
                        else if (!strcmp(buffer, "passwd")) {
                            sprintf(ctemp1, "This feature is not yet implemented.\n");
                            send(socketdesc, ctemp1, strlen(ctemp1), 0);
                        }
                        else if (!strcmp(buffer, "exit")) {
                        //closing, needs moved
                        getpeername(socketdesc, (struct sockaddr*)&client, (socklen_t*)&client);
                        printf("Client at %s:%d disconnected.\n",inet_ntoa(svraddr.sin_addr), ntohs(svraddr.sin_port));
                        currConns--;
                        close(socketdesc);
                        clientSocket[i] = 0;
                        }
                        
                        //Catch-all response
                        else {
                            response = "Invalid command. Type menu for help.\n";
                            send(socketdesc, response, strlen(response), 0);
                            // buffer[valread] = '\n';
                            // buffer[valread+1] ='\0'; 
                            // send(socketdesc, buffer, strlen(buffer),0);

                        }


                    }
                }
            }

            }

            return;
        }  
    


/**********************************************************************************************
 * shutdown - Cleanly closes the socket FD.
 *
 *    Throws: socket_error for recoverable errors, runtime_error for unrecoverable types
 **********************************************************************************************/

void TCPServer::shutdown() {
    close(svrSocketFD);

}
