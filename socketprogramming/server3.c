/*get sockaddr-ipv4 or 6
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
#include <time.h>
/*struct tm {
    int tm_sec;   // seconds after the minute [0, 61]
    int tm_min;   // minutes after the hour [0, 59]
    int tm_hour;  // hours since midnight [0, 23]
    int tm_mday;  // day of the month [1, 31]
    int tm_mon;   // months since January [0, 11]
    int tm_year;  // years since 1900
    // ... (additional fields)
};
*/

#define PORT 12345/* Port 12345 is simply a commonly chosen port for illustrative purposes and is not reserved or assigned to any specific service.

*/
#define MAX_BUFFER_SIZE 1024

void handle_client(int client_socket) {
    char buffer[MAX_BUFFER_SIZE];
    int bytes_received;
    time_t current_time;
    /*ssize_t recv(int sockfd, void *buf, size_t len, int flags);
Description: Receives data from a connected socket.
sockfd: The socket file descriptor.
buf: A pointer to the buffer where the received data will be stored.
len: The maximum number of bytes to receive.
flags: Additional flags (usually set to 0).*/


    while ((bytes_received = recv(client_socket, buffer, sizeof(buffer), 0)) > 0) {
        // Get current timestamp for receiving
        current_time = time(NULL);/*current_time = time(NULL);: This line uses the time function to get the current time from the system clock. The time function returns the current time in seconds since the Epoch (January 1, 1970). By passing NULL as an argument, it directly fills the result into the current_time variable.*/
        struct tm* time_info = localtime(&current_time);/*struct tm* time_info = localtime(&current_time);: The localtime function is then used to convert the raw time value (in seconds) into a structure (struct tm) representing the local time. It takes the address of current_time as an argument and returns a pointer to a struct tm that holds various components of the time, such as the hour, minute, second, etc.*/

        // Print the timestamp for receiving
        printf("time: [%02d:%02d:%02d] Received message from client: %.*s",
               time_info->tm_hour, time_info->tm_min, time_info->tm_sec,
               bytes_received, buffer);/*%.*s  it is a precission, that the formatspecifier s will get only bytes_received number of bytes*/
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
*/


    // Create socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Set up server address structure
    server_addr.sin_family = AF_INET;//set to ipv4
    server_addr.sin_port = htons(PORT);/*The htons function is used to convert the port number from host byte order to network byte order. */
    server_addr.sin_addr.s_addr = INADDR_ANY;
    /*server_addr.sin_addr.s_addr = INADDR_ANY;

This line sets the IP address for the server socket. INADDR_ANY is a constant that represents the server's willingness to accept connections on any of its network interfaces.*/

    // Bind socket to address
    /*int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
    Description: Associates a socket with a specific address (IP address and port number).
sockfd: The socket file descriptor.
addr: A pointer to a struct sockaddr containing the address information.
addrlen: The size of the address structure.*/

    // Bind socket to address
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
    if (listen(server_socket, 1) == -1) {
        perror("Error listening for connections");
        exit(EXIT_FAILURE);
    }

    printf("Echo server is listening on port %d\n", PORT);

    // Accept a connection
    /*int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
        Description: Accepts a connection on a listening socket.
sockfd: The socket file descriptor.
addr: A pointer to a struct sockaddr where the address of the connecting entity will be stored.
addrlen: A pointer to the size of the address structure, modified by the function to indicate the actual size.*/
while(1){
    if ((client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len)) == -1) {
        perror("Error accepting connection");
        exit(EXIT_FAILURE);
    }

    printf("Accepted connection from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
    /*inet_ntoa(client_addr.sin_addr): Converts the binary IPv4 address (client_addr.sin_addr) of the connecting client to a human-readable string. This string is then used in the printf statement to display the IP address of the connecting client.*/


    // Handle client communication
    handle_client(client_socket);

    // Close the server socket
    }
    close(server_socket);

    return 0;
}
/*In the log message "Accepted connection from 127.0.0.1:46048," the number "46048" is the port number associated with the client's end of the connection. In this specific case, it indicates the port number used by the client to establish the connection with the server.

When a client initiates a connection to a server, it chooses a random available port number on its side of the connection, known as an ephemeral port. The server then uses this port number to send the response back to the correct port on the client.

In the context of your echo server, "127.0.0.1" is the IP address of the client (localhost), and "46048" is the ephemeral port on the client's side that was dynamically assigned for the connection. The server logs this information when it accepts a connection to acknowledge where the incoming connection is originating from.*/

