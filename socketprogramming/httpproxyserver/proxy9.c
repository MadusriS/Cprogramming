#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <poll.h>
#define maxclients 10
/*struct sockaddr_in {
    short sin_family;           // Address family, usually AF_INET
    unsigned short sin_port;    // Port number in network byte order
    struct in_addr sin_addr;    // IP address in network byte order
    char sin_zero[8];           // Padding to make the structure the same size as struct sockaddr
};
*/
/*struct addrinfo {
    int ai_flags;           // Input flags
    int ai_family;          // Address family (AF_INET for IPv4, AF_INET6 for IPv6, etc.)
    int ai_socktype;        // Socket type (e.g., SOCK_STREAM for TCP, SOCK_DGRAM for UDP)
    int ai_protocol;        // Protocol (usually 0, letting the system choose the appropriate protocol)
    socklen_t ai_addrlen;    // Length of socket address
    struct sockaddr* ai_addr; // Socket address for socket()
    char* ai_canonname;      // Canonical name for nodename
    struct addrinfo* ai_next; // Pointer to the next structure in the linked list
};
*/

#define MAX 65536
#define PORT 1234

int is_connect_request(const char *request) {
    return (strncmp(request, "CONNECT", 7) == 0);
}

void handle_connect_request(int socket_client, int socket_server) {
struct sockaddr_in address;
int new_socket,addrlen;
    const char* success_response = "HTTP/1.1 200 OK\r\n\r\n";/*The \r\n\r\n denotes the end of the HTTP headers and the beginning of the response body.*/
    send(socket_client, success_response, strlen(success_response), 0);/*
This line sends the HTTP success response (success_response) to the connected client using the send function. It specifies the socket (socket_client) through which the data will be sent, */

    // Additional handling for SSL/TLS connection setup
      printf("Connection established...\n");

    // Now you can start relaying data between socket_client and socket_server
    
        struct pollfd fds[2];
        fds[0].fd = socket_server;
        fds[0].events = POLLIN;
     for(int i=1;i<maxclients+1;i++){
        fds[i].fd = 0;
       // fds[i].events = POLLIN;}

       
while (1) {
        int ret = poll(fds, maxclients+1, -1);

        if (ret < 0) {
            perror("Poll error...\n");
            exit(EXIT_FAILURE);
        }
        /*The subsequent if statements check whether data is available for reading on either socket_client or socket_server.

If data is available on socket_client, it is read into a buffer using recv and then sent to socket_server using send.

If data is available on socket_server, it is read into a buffer using recv and then sent to socket_client using send.

*/

        if (fds[0].revents & POLLIN) {
         if ((new_socket = accept(socket_server, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
                perror("accept");
                exit(EXIT_FAILURE);
            }
printf("New connection, socket fd is %d, ip is : %s, port : %d\n",
                   new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));
        
            char buffer[MAX];
            ssize_t bytes_received = recv(socket_client, buffer, sizeof(buffer), 0);

            if (bytes_received <= 0) {
                break;
            }

            send(socket_client, buffer, bytes_received, 0);
        }
        
for (i = 1; i < maxclients + 1; i++) {
                if (fds[i].fd == 0) {
                    fds[i].fd = new_socket;
                    fds[i].events = POLLIN;
                    break;
                }
            }        
for(int i=1;i<maxclients+1;i++){
        if (fds[i].revents & POLLIN) {
        int sd = fds[i].fd;
            char buffer[MAX];
            ssize_t bytes_received = recv(socket_server, buffer, sizeof(buffer), 0);

            if (bytes_received <= 0) {
                break;
            }

            send(socket_server, buffer, bytes_received, 0);
             close(sd);
                    fds[i].fd = 0;
        }
        //close(sd);
                   // fds[i].fd = 0;
    }

    close(socket_client);}
    close(socket_server);
}}

void proxy_to_server(int socket_client) {
    char request[MAX];
    ssize_t r;

    if ((r = read(socket_client, request, MAX)) > 0) /* read function to read data from the socket_client into the request*/
    {
    printf("the request is %s\n" ,request);
        if (is_connect_request(request)) {
            printf("Received CONNECT request...\n");
            //CONNECT www.trivago.in:443 HTTP/1.1

            //syntax of char *strtok(char *str, const char *delimiters);

            char *host = strtok(request + 8, ":");//ignores "connect " takes www.trivago.in till ":"
            char *port_str = strtok(NULL, " ");//takes 443 till " "
            int port = atoi(port_str);

            printf("Connecting to %s:%d...\n", host, port);

            int socket_server = socket(AF_INET, SOCK_STREAM, 0);

            if (socket_server < 0) {
                perror("Failed to create server socket");
                close(socket_client);
                return;
            }

            struct sockaddr_in server_addr;/*  "sockaddr_in" stands for "socket address in the Internet."  This structure is commonly used when working with sockets, where it is used to specify the local or remote address to bind or connect to.*/
            memset(&server_addr, 0, sizeof(server_addr));
            server_addr.sin_family = AF_INET;//ipv4
            server_addr.sin_port = htons(port);

            struct addrinfo hints, *server_info;

            memset(&hints, 0, sizeof(hints));
            hints.ai_family = AF_INET;
            hints.ai_socktype = SOCK_STREAM;

            int getaddrinfo_result = getaddrinfo(host, port_str, &hints, &server_info);/* It resolves the domain name (hostname) and service name (port) and provides a list of address structures that can be used to create sockets.*/

            if (getaddrinfo_result != 0) {
                fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(getaddrinfo_result));
                close(socket_server);
                close(socket_client);
                return;
            }

            struct sockaddr_in *server_addr_in = (struct sockaddr_in *)server_info->ai_addr;/*The line is converting a generic socket address (struct sockaddr) to an IPv4-specific socket address (struct sockaddr_in). It assumes the socket address is IPv4 based on the context. After this, you can use the converted structure to access IPv4-specific information like IP address and port.*/
            memcpy(&server_addr.sin_addr.s_addr, &server_addr_in->sin_addr, sizeof(struct in_addr));/*This line is copying the IPv4 address from `server_addr_in` to `server_addr`. It's essentially copying the IP address from the resolved server information structure to the destination structure.*/

            freeaddrinfo(server_info);

            if (connect(socket_server, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) 
            /*This line establishes a connection from the client socket (socket_server) to the destination server using the connect system call. The server's address information is provided through the server_addr structure. */{
                perror("Failed to connect to destination server");
                close(socket_server);
                close(socket_client);
                return;
            }

            handle_connect_request(socket_client, socket_server);
        } else {
            printf("Ignoring non-CONNECT request:\n%s\n", request);
            close(socket_client);
        }
    }
}

int main() {
    int socket_server, socket_client;
    socklen_t cli_len;
    struct sockaddr_in server_address, client_address;

    socket_server = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_server == -1) {
        perror("Socket creation error...\n");
        exit(EXIT_FAILURE);
    }

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    printf("\nSocket created...\n");

    if (bind(socket_server, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) //binding the server socket to server address
    {
        perror("Bind error...\n");
        exit(EXIT_FAILURE);
    }

    printf("Bind is done...\n");

    if (listen(socket_server, 5) == -1) {
        perror("Listen error...\n");
        exit(EXIT_FAILURE);
    }

    printf("Proxy server listening at port %d...\n", PORT);
    cli_len = sizeof(client_address);

    while (1) {
        socket_client = accept(socket_server, (struct sockaddr*)&client_address, &cli_len);/*
The accept function in socket programming is used by a server to accept a connection request from a client. It creates a new socket for the accepted connection and returns a file descriptor representing that socket.*/

        if (socket_client == -1) {
            perror("Client connection error...\n");
            exit(EXIT_FAILURE);
        }

        printf("\nThe client is connected...\n");

        proxy_to_server(socket_client);
    }

    close(socket_server);

    return 0;
}

