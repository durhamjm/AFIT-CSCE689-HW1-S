#include "TCPClient.h"
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <cstdio>
#include <iostream>
#include <ostream>
#include <fcntl.h>


/**********************************************************************************************
 * TCPClient (constructor) - Creates a Stdin file descriptor to simplify handling of user input. 
 *
 **********************************************************************************************/

TCPClient::TCPClient() {
}

/**********************************************************************************************
 * TCPClient (destructor) - No cleanup right now
 *
 **********************************************************************************************/

TCPClient::~TCPClient() {

}

/**********************************************************************************************
 * connectTo - Opens a File Descriptor socket to the IP address and port given in the
 *             parameters using a TCP connection.
 *
 *    Throws: socket_error exception if failed. socket_error is a child class of runtime_error
 **********************************************************************************************/

void TCPClient::connectTo(const char *ip_addr, unsigned short port) {

    if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Error creating socket\n");
    }
    noblock = fcntl(clientSocket, F_SETFL | O_NONBLOCK);
    clientaddr.sin_family = AF_INET;
    clientaddr.sin_port = htons(port);



    if (inet_pton(AF_INET, ip_addr, &clientaddr.sin_addr) <= 0) {
        printf("Invalid IP");
    }

     if (connect(clientSocket,(struct sockaddr *)&clientaddr, sizeof(clientaddr)) < 0) {
         printf("Unable to connect");
     }
    
}

/**********************************************************************************************
 * handleConnection - Performs a loop that checks if the connection is still open, then 
 *                    looks for user input and sends it if available. Finally, looks for data
 *                    on the socket and sends it.
 * 
 *    Throws: socket_error for recoverable errors, runtime_error for unrecoverable types
 **********************************************************************************************/

void TCPClient::handleConnection() {
   //while true, look for input and send, plus print output (if needed, server may do that automatically)
    int clientSize = sizeof(clientaddr);
    while (true) {
        fd_set read_fd;
        FD_ZERO(&read_fd);
        FD_SET(clientSocket, &read_fd);

        //output
        int n;
        bzero(buffer, sizeof(buffer));
        if ((n = read(clientSocket, buffer, sizeof(buffer)) != 0)) {
            printf("%s", buffer);
            std::cout << std::flush;
        }

        //input
        bzero(buffer, sizeof(buffer));
        fgets(buffer, sizeof(buffer)-1, stdin);
        n = write(clientSocket, buffer, strlen(buffer));
        bzero(buffer, sizeof(buffer));

        
    }


        

    
    //printf("%s\n",buffer);

}

/**********************************************************************************************
 * closeConnection - Your comments here
 *
 *    Throws: socket_error for recoverable errors, runtime_error for unrecoverable types
 **********************************************************************************************/

void TCPClient::closeConn() {
    close(clientSocket);
}


