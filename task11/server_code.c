#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


int main(int argc, char **argv) {
    int sock;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t sin_size = sizeof(struct sockaddr_in);

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        perror("Socket error");
    
    int sock_opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (void *)&sock_opt, sizeof(sock_opt));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(9999);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(serverAddr.sin_zero), 8);

    if (bind(sock, (struct sockaddr *)&serverAddr, sizeof(struct sockaddr)) == -1)
        perror("Bind error");

    if (listen(sock, 10) == -1)
        perror("Listen error");

    printf("Server is listening on port 9999 \n");

    int newsock;
    while (1) {
        if ((newsock = accept(sock, (struct sockaddr *)&clientAddr, &sin_size)) == -1) {
            perror("Socket error");
            continue;
        }

        printf("Connection established with %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

        int maxBytes = 1024; 
        char buffer[maxBytes];

        while (1) {
            memset(buffer, 0, maxBytes);
            
            int bytes_read = recv(newsock, buffer, maxBytes, 0);

            if (bytes_read > 0) {
                printf("Received Message from %s:%d\n%s\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port), buffer);
                send(newsock, "Message received", 16, 0);
            } else if (bytes_read == 0) {
                printf("Client disconnected: %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
                break;
            } else {
                perror("Error Receiving Message");
                break;
            }
        }

        close(newsock);
    }

    close(sock);
    return 0;
}
