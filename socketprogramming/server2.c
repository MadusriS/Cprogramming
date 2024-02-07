/*//get sockaddr-ipv4 or 6
/*struct sockaddr_in {
    short int          sin_family;  // Address family (AF_INET)
    unsigned short int sin_port;    // Port number
    struct in_addr     sin_addr;    // IPv4 address
    unsigned char      sin_zero[8]; // Padding to make the structure the same size as struct sockaddr
};
*/
/*addrinfo and sockaddr structures are part of the networking libraries provided by the operating system, typically found in the <sys/socket.h> and <netdb.h> header files.
struct addrinfo {

    int              ai_flags;
    int              ai_family;
    int              ai_socktype;
    int              ai_protocol;
    socklen_t        ai_addrlen;
    struct sockaddr *ai_addr;
    char            *ai_canonname;
    struct addrinfo *ai_next;
    };
// ai_flags: Additional options specified when generating the address (e.g., AI_PASSIVE)
    // ai_family: Address family (e.g., AF_INET for IPv4, AF_INET6 for IPv6)
    // ai_socktype: Socket type (e.g., SOCK_STREAM for TCP, SOCK_DGRAM for UDP)

// ai_protocol: Protocol (usually 0, but can specify a protocol if known)
    // ai_addrlen: Length of the socket address structure
    // ai_addr: Pointer to a socket address structure (contains the actual address)
    // ai_protocol: Protocol (usually 0, but can specify a protocol if known)
    // ai_addrlen: Length of the socket address structure
    // ai_addr: Pointer to a socket address structure (contains the actual address)*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 12345
#define MAX_BUFFER_SIZE 1024

void handle_client(int client_socket) {
    char buffer[MAX_BUFFER_SIZE];
    int bytes_received;
    /*ssize_t recv(int sockfd, void *buf, size_t len, int flags);
Description: Receives data from a connected socket.
sockfd: The socket file descriptor.
buf: A pointer to the buffer where the received data will be stored.
len: The maximum number of bytes to receive.
flags: Additional flags (usually set to 0).*/

    while ((bytes_received = recv(client_socket, buffer, sizeof(buffer), 0)) > 0) {
        // Print the received message
        printf("Received message from client: %.*s", bytes_received, buffer);
        /*ssize_t send(int sockfd, const void *buf, size_t len, int flags);
Description: Sends data to a connected socket.
sockfd: The socket file descriptor.
buf: A pointer to the data to be sent.
len: The number of bytes to send.
flags: Additional flags (usually set to 0).*/

        // Echo back to the client
        send(client_socket, buffer, bytes_received, 0);
    }

    close(client_socket);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
/*int socket(int domain, int type, int protocol);
Description: Creates a new socket.
domain: The communication domain (e.g., AF_INET for IPv4, AF_INET6 for IPv6).
type: The socket type (e.g., SOCK_STREAM for TCP, SOCK_DGRAM for UDP).
protocol: The protocol to be used (typically 0 for default).
The socket() function in C returns a socket descriptor, which is an integer representing the endpoint of a communication channel.*/

    // Create socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Set up server address structure
    server_addr.sin_family = AF_INET;//set to IPV4
    server_addr.sin_port = htons(PORT);/*The htons function is used to convert the port number from host byte order to network byte order. */
    server_addr.sin_addr.s_addr = INADDR_ANY;/*server_addr.sin_addr.s_addr = INADDR_ANY;

This line sets the IP address for the server socket. INADDR_ANY is a constant that represents the server's willingness to accept connections on any of its network interfaces.
struct in_addr {
    in_addr_t s_addr;  // IPv4 address (32 bits)
};
s_addr: A 32-bit unsigned integer representing the IPv4 address in network byte order.*/

    // Bind socket to address
    /*int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
    Description: Associates a socket with a specific address (IP address and port number).
sockfd: The socket file descriptor.
addr: A pointer to a struct sockaddr containing the address information.
addrlen: The size of the address structure.*/

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error binding socket");
        exit(EXIT_FAILURE);
    }
    /*int listen(int sockfd, int backlog);
    
The listen function is used to set up a socket for listening and accepting incoming connections. Here is the syntax for the listen function in C:


sockfd: The socket file descriptor returned by the socket function.
backlog: The maximum length to which the queue of pending connections for the socket may grow. It is the maximum number of connection requests that can be queued up before the system starts rejecting new connection requests.
*/


    // Listen for incoming connections
    if (listen(server_socket, 5) == -1) {
        perror("Error listening for connections");
        exit(EXIT_FAILURE);
    }

    printf("Echo server is listening on port %d\n", PORT);

    while (1) {
        // Accept a connection
        /*int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
        Description: Accepts a connection on a listening socket.
sockfd: The socket file descriptor.
addr: A pointer to a struct sockaddr where the address of the connecting entity will be stored.
addrlen: A pointer to the size of the address structure, modified by the function to indicate the actual size.*/
/*This creates a new socket specifically for the communication with the connected client.*//*The accept() function in C returns a new socket descriptor representing the accepted connection. */
        if ((client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len)) == -1) {
            perror("Error accepting connection");
            continue;
        }

        printf("Accepted connection from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
/*inet_ntoa(client_addr.sin_addr): Converts the binary IPv4 address (client_addr.sin_addr) of the connecting client to a human-readable string. This string is then used in the printf statement to display the IP address of the connecting client.*/
        // Handle client communication in a separate function
        handle_client(client_socket);
    }

    close(server_socket);

    return 0;
}


