#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>


int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: ./client server-address\n");
        exit(1);
    }

    int sock;
    struct sockaddr_in serverAddr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket Error");
        exit(1);
    }

    memset((char *)&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(9999);

    struct hostent *server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr, "Error: No such host\n");
        terminate_with_error(sock);
    }

    memcpy((char *)&serverAddr.sin_addr.s_addr, (char *)server->h_addr_list[0], server->h_length);
    memset(&(serverAddr.sin_zero), 0, 8);

    if (connect(sock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
        terminate_with_error(sock);

    printf("Connected to the server. Type your messages below:\n");

    char buffer[1024];
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        printf("You: ");
        fgets(buffer, sizeof(buffer), stdin);

        if (send(sock, buffer, strlen(buffer), 0) == -1) {
            perror("Error Sending Message");
            break;
        }

        memset(buffer, 0, sizeof(buffer));
        if (recv(sock, buffer, sizeof(buffer), 0) > 0) {
            printf("Server: %s\n", buffer);
        } else {
            perror("Error Receiving Message");
            break;
        }
    }

    close(sock);
    return 0;
}
