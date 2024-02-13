#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8888

int main() {
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char message[1024];
    char buffer[1024] = {0};

    // create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    // set up server details
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }

    // connect to server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection Failed");
        exit(EXIT_FAILURE);
    }

    // receive the welcome message from the server
    valread = read(sock, buffer, 1024);
    printf("Server's response: %s\n", buffer);

    while (1) {
        // get user input
        printf("Enter a message to send to the server: ");
        fgets(message, sizeof(message), stdin);

        // send the message to the server
        send(sock, message, strlen(message), 0);

        // receive the echoed message from the server
        valread = read(sock, buffer, 1024);
        printf("Server's response: %s\n", buffer);
    }

    return 0;
}

