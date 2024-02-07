#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>

#define SERVER_IP "127.0.0.1"
#define PORT 12345
#define MAX_BUFFER_SIZE 1024

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[MAX_BUFFER_SIZE];
    int bytes_received;
    time_t current_time;

    // Create socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Set up server address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // Connect to the server
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error connecting to server");
        exit(EXIT_FAILURE);
    }

    printf("Connected to the echo server\n");

    while (1) {
        // Get current timestamp for sending
        current_time = time(NULL);
        struct tm* time_info = localtime(&current_time);

        printf("time: [%02d:%02d:%02d] Enter message (type 'exit' to quit): ",
               time_info->tm_hour, time_info->tm_min, time_info->tm_sec);

        fgets(buffer, sizeof(buffer), stdin);

        // Send data to the server
        send(client_socket, buffer, strlen(buffer), 0);

        // Get current timestamp for receiving
        current_time = time(NULL);
        time_info = localtime(&current_time);

        // Receive echoed data from the server
        bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);

        if (bytes_received <= 0) {
            perror("Error receiving data from server");
            break;
        }

        // Null-terminate the received data before printing
        buffer[bytes_received] = '\0';

        // Print the timestamp for receiving and echoed message
        printf("time : [%02d:%02d:%02d] Server echoed: %s",
               time_info->tm_hour, time_info->tm_min, time_info->tm_sec, buffer);

        // Check for exit condition
        if (strncmp(buffer, "exit", 4) == 0)
            break;
    }

    // Close the socket
    close(client_socket);

    return 0;
}

