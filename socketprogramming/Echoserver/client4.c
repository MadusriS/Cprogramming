#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>/*Including arpa/inet.h is essential when working with socket programming and networking in C, as it provides the necessary tools for handling IP addresses, converting byte order, and dealing with socket address structures.
contains hton() and ntoh() etc*/
#include <unistd.h>
/*The #include <unistd.h> directive is used to include the header file in C programs. This header file provides access to the POSIX (Portable Operating System Interface) operating system API, and it contains various function declarations and constants related to system calls, file operations, and process management.*/
#include <time.h>

#define SERVER_IP "127.0.0.1"/*The SERVER_IP is defined as the loopback address "127.0.0.1". The loopback address is a special IP address that always refers to the localhost or the machine on which the code is running. In IPv4, "127.0.0.1" is the loopback address, and any data sent to this address is looped back within the same device.*/
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
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);// You can change this to the server's IP address
    /*The inet_addr function in C is used to convert a string representation of an IPv4 address into its binary form, suitable for use in network-related structures. In the provided example, inet_addr("127.0.0.1") converts the string "127.0.0.1" into the binary representation of the IPv4 loopback address.


int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
sockfd: The socket file descriptor representing the client's socket, obtained from the socket() function.

addr: A pointer to a struct sockaddr containing the server's address information.

addrlen: The size of the addr structure.

The return value of connect() is an integer that indicates the success or failure of the connection attempt*/
int err;

    // Connect to the server
    if (err=connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error connecting to server");
        exit(EXIT_FAILURE);
    }

    printf("Connected to the echo server%d\n",err);

    while (1) {
        // Get current timestamp for sending
        current_time = time(NULL);
        struct tm* time_info = localtime(&current_time);

        printf("[%02d:%02d:%02d] Enter message (type 'exit' to quit): ",
               time_info->tm_hour, time_info->tm_min, time_info->tm_sec);

        fgets(buffer, sizeof(buffer), stdin);

        // Check for exit condition before sending the message
        if (strncmp(buffer, "exit", 4) == 0) {
            // Get current timestamp for sending
            current_time = time(NULL);
            time_info = localtime(&current_time);

            printf("[%02d:%02d:%02d] Exiting client.\n",
                   time_info->tm_hour, time_info->tm_min, time_info->tm_sec);
            break;
        }

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
        printf("[%02d:%02d:%02d] Server echoed: %s",
               time_info->tm_hour, time_info->tm_min, time_info->tm_sec, buffer);
    }

    // Close the socket
    close(client_socket);

    return 0;
}

