#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 8192

void handle_client(int client_socket);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int server_port = atoi(argv[1]);

    // Create socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(server_port);

    // Bind the socket
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        perror("Error binding socket");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, 10) == -1) {
        perror("Error listening for connections");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Proxy server listening on port %d...\n", server_port);

    while (1) {
        // Accept incoming connection
        struct sockaddr_in client_address;
        socklen_t client_address_len = sizeof(client_address);
        int client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_len);

        if (client_socket == -1) {
            perror("Error accepting connection");
            continue;
        }

        printf("Accepted connection from %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

        // Handle client request in a new process
        if (fork() == 0) {
            close(server_socket);
            handle_client(client_socket);
            exit(EXIT_SUCCESS);
        } else {
            close(client_socket);
        }
    }

    close(server_socket);

    return 0;
}

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));

    // Read the client's request
    ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
    if (bytes_received == -1) {
        perror("Error receiving data");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    // Print the client's request
    printf("Received request:\n%s\n", buffer);

    // Forward the request to the target server (modify as needed)
    const char *target_host = "www.example.com";
    const int target_port = 80;

    // Create socket for the target server
    int target_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (target_socket == -1) {
        perror("Error creating target socket");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    // Configure target server address
    struct sockaddr_in target_address;
    memset(&target_address, 0, sizeof(target_address));
    target_address.sin_family = AF_INET;
    target_address.sin_addr.s_addr = inet_addr(target_host);
    target_address.sin_port = htons(target_port);

    // Connect to the target server
    if (connect(target_socket, (struct sockaddr *)&target_address, sizeof(target_address)) == -1) {
        perror("Error connecting to target server");
        close(client_socket);
        close(target_socket);
        exit(EXIT_FAILURE);
    }

    // Forward the client's request to the target server
    ssize_t bytes_sent = send(target_socket, buffer, bytes_received, 0);
    if (bytes_sent == -1) {
        perror("Error sending data to target server");
        close(client_socket);
        close(target_socket);
        exit(EXIT_FAILURE);
    }

    // Receive the response from the target server
    memset(buffer, 0, sizeof(buffer));
    bytes_received = recv(target_socket, buffer, sizeof(buffer), 0);
    if (bytes_received == -1) {
        perror("Error receiving data from target server");
        close(client_socket);
        close(target_socket);
        exit(EXIT_FAILURE);
    }

    // Print the target server's response
    printf("Received response from target server:\n%s\n", buffer);

    // Forward the target server's response to the client
    bytes_sent = send(client_socket, buffer, bytes_received, 0);
    if (bytes_sent == -1) {
        perror("Error sending data to client");
        close(client_socket);
        close(target_socket);
        exit(EXIT_FAILURE);
    }

    // Close sockets
    close(client_socket);
    close(target_socket);
}

