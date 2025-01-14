#include "libsocket.h"
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h> // Get error number



int main(int argc, char** argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: client <hostname> <port> \n");
        exit(1);
    }

    // Only supports localhost as a hostname, but that's all we'll test on
    const char* addr =
        strcmp(argv[1], "localhost") == 0 ? "127.0.0.1" : argv[1];
    int port = atoi(argv[2]);

    // TODO: Create socket
    int sockfd = s_socket(AF_INET, SOCK_DGRAM, 0);

    // TODO: Set stdin and socket nonblocking
    int flags = fcntl(sockfd, F_GETFL);
    flags |= O_NONBLOCK;
    fcntl(sockfd, F_SETFL, flags);

    // Setting stdin non-blocking 
    int stdin_flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, stdin_flags | O_NONBLOCK);

    
    // TODO: Construct server address
    /* 2. Construct our address */
    struct sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET; // use IPv4
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
   
    // Set receiving port
    int PORT = 8080;
    serveraddr.sin_port = htons(PORT); // Big endian

    char buffer[1024];
   
    /* 4. Create buffer to store incoming data */
    int BUF_SIZE = 1024;
    char server_buf[1024];
    socklen_t serversize = sizeof(struct sockaddr_in); // Temp buffer for recvfrom API

    // Listen loop
    while (1) {
        // TODO: Receive from socket
        int bytes_recvd = s_recvfrom(sockfd, server_buf, BUF_SIZE, 
                        // socket  store data  how much
                           0, (struct sockaddr*) &serveraddr, 
                           &serversize);
        // TODO: If data, write to stdout
        if (bytes_recvd >=0){
            write(1, server_buf, bytes_recvd);
        }
        // TODO: Read from stdin
        int bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer));
        // TODO: If data, send to socket
        if (bytes_read > 0) {
            // Send user input to the server
            int bytes_sent = s_sendto(sockfd, buffer, bytes_read, 0, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
            if (bytes_sent < 0) {
                perror("sendto");
            }
        }
    }   
    close(sockfd);

    return 0;
}