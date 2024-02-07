#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 12345
#define MAX_BUFFER_SIZE 1024

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[MAX_BUFFER_SIZE];
    int bytes_received;

    // Create socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Set up server address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  // You can change this to the server's IP address
    /*The inet_addr function in C is used to convert a string representation of an IPv4 address into its binary form, suitable for use in network-related structures. In the provided example, inet_addr("127.0.0.1") converts the string "127.0.0.1" into the binary representation of the IPv4 loopback address.


int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
sockfd: The socket file descriptor representing the client's socket, obtained from the socket() function.

addr: A pointer to a struct sockaddr containing the server's address information.

addrlen: The size of the addr structure.

The return value of connect() is an integer that indicates the success or failure of the connection attempt*/

    // Connect to the server
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error connecting to server");
        exit(EXIT_FAILURE);
    }

    printf("Connected to the echo server\n");

    while (1) {
        printf("Enter message (type 'exit' to quit): ");
        fgets(buffer, sizeof(buffer), stdin);

        // Send data to the server
        send(client_socket, buffer, strlen(buffer), 0);

        // Receive echoed data from the server
        bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);

        if (bytes_received <= 0) {
            perror("Error receiving data from server");
            break;
        }

        // Null-terminate the received data before printing
        buffer[bytes_received] = '\0';
        printf("Server echoed: %s", buffer);

        // Check for exit condition
        if (strncmp(buffer, "exit", 4) == 0)
            break;
    }

    // Close the socket
    close(client_socket);

    return 0;
}

