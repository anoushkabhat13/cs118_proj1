#include "libsocket.h"
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <errno.h> // Get error number
#include <string.h>

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: server <port>\n");
        exit(1);
    }

    int port = atoi(argv[1]);

    // TODO: Create socket
    //AF_INET Specifies to use IPv4 and SOCK_DRGAM specifies to use UDP
    int sockfd = s_socket(AF_INET, SOCK_DGRAM, 0);

    // TODO: Set stdin and socket nonblocking (set later, once first messsage has been recieved from client)
    char stdin_buf[1024];
    
    // TODO: Construct server address
    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET; // use IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY; // accept all connections

    // Set receiving port
    int PORT = port;
    servaddr.sin_port = htons(PORT); // Big endian// Set receiving port
    
    // TODO: Bind socket with server address
    int did_bind = s_bind(sockfd, (struct sockaddr*) &servaddr, 
                        sizeof(servaddr));
    // Error if did_bind < 0 :(
    if (did_bind < 0) return errno;
    
    // TODO: Create sockaddr_in and socklen_t buffers to store client address
    struct sockaddr_in clientaddr = {0}; // Same information, but about client
    socklen_t clientsize = sizeof(clientaddr);

    char buffer[1024];
    int client_connected = 0;

    // Listen loop
    while (1) {
        // TODO: Receive from socket
        /* 5. Listen for data from clients */
        int bytes_recvd = s_recvfrom(sockfd, buffer, 1024, 
                                // socket  store data  how much
                                0, (struct sockaddr*) &clientaddr, 
                                &clientsize);
        // Execution will stop here until `BUF_SIZE` is read or termination/error
        // Error if bytes_recvd < 0 :(
        if (bytes_recvd < 0) return errno;
       
        // TODO: If no data and client not connected, continue
        if (bytes_recvd == 0) continue;

        // TODO: If data, client is connected and write to stdout
        // If client sent data, display the client's IP and port
        // Make non-blocking
        if (bytes_recvd > 0) {
            if (!client_connected) {
                client_connected = 1;
                fprintf(stdout,"Switching server to non-blocking mode.\n");
            }

            int flags = fcntl(sockfd, F_GETFL);
            flags |= O_NONBLOCK;
            fcntl(sockfd, F_SETFL, flags);

            fprintf(stdout,"Client connected! IP: %s, Port: %d\n");
            char* client_ip = inet_ntoa(clientaddr.sin_addr);
            int client_port = ntohs(clientaddr.sin_port);
            fprintf(stdout,"Received message from %s:%d\n", client_ip, client_port);
        }

        // TODO: Read from stdin
        if(client_connected){
           
            if (fgets(stdin_buf, sizeof(stdin_buf), stdin)) {
                // fgets reads a line from stdin and stores it in stdin_buf
                // Send stdin_buf to the client instead of "Hello world!"
                int did_send = s_sendto(sockfd, stdin_buf, strlen(stdin_buf), 
                                    0, (struct sockaddr*) &clientaddr, 
                                    sizeof(clientaddr));
                if (did_send < 0) return errno;
            }
        }
        
    }

    close(sockfd);

    return 0;
}
