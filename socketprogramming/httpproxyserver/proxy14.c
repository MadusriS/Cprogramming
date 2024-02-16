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
/*struct addrinfo {
    int ai_flags;           // AI_PASSIVE, AI_CANONNAME, etc.
    int ai_family;          // AF_UNSPEC, AF_INET, AF_INET6, etc.
    int ai_socktype;        // SOCK_STREAM, SOCK_DGRAM, etc.
    int ai_protocol;        // IPPROTO_TCP, IPPROTO_UDP, etc.
    socklen_t ai_addrlen;   // Length of ai_addr field
    struct sockaddr *ai_addr; // Socket address for socket()
    char *ai_canonname;     // Canonical name for hostname
    struct addrinfo *ai_next; // Pointer to next in the linked list
};

#include <netinet/in.h>

struct sockaddr_in {
    short            sin_family;   // AF_INET, address family (IPv4)
    unsigned short   sin_port;     // Port number in network byte order
    struct in_addr   sin_addr;     // Internet address structure
    char             sin_zero[8];  // Padding to make structure the same size as struct sockaddr
};

*/


#define MAX 65536
#define PORT 1234
#define MAX_CLIENTS 100

int is_connect_request(const char *request) {
    return (strncmp(request, "CONNECT", 7) == 0);
}

void handle_connect_request(int socket_client, int socket_server) {
    const char *success_response = "HTTP/1.1 200 OK\r\n\r\n";/*The line you provided defines a string success_response containing an HTTP response header. This string is commonly used in HTTP server programming to send a simple successful response to a client. */
    send(socket_client, success_response, strlen(success_response), 0);//response from server to client

    printf("Connection established...\n");

    while (1) {
        struct pollfd fds[2];

        fds[0].fd = socket_client;
        fds[0].events = POLLIN;

        fds[1].fd = socket_server;
        fds[1].events = POLLIN;

        int ret = poll(fds, 2, -1);

        if (ret < 0) {
            perror("Poll error...\n");
            exit(EXIT_FAILURE);
        }

        for (int fd = 0; fd < 2; fd++) {
            if (fds[fd].revents & POLLIN) {
                char buffer[MAX];
                ssize_t bytes_received = recv((fd == 0) ? socket_client : socket_server, buffer, sizeof(buffer), 0);

                if (bytes_received <= 0) {
                    break;
                }

                send((fd == 0) ? socket_server : socket_client, buffer, bytes_received, 0);
            }
        }
    }

    close(socket_client);
    close(socket_server);
}

void proxy_to_server(int socket_client) {

//global server -server ;proixy- client;
/*steps in client:
Create a socket: socket_server = socket(AF_INET, SOCK_STREAM, 0);

Initialize the server address structure (struct sockaddr_in), including the server's IP address and port number.

Use connect to establish a connection to the server.*/
    char request[MAX];
    ssize_t r;

    if ((r = read(socket_client, request, MAX)) > 0) {
        printf("the request is %s\n", request);

        if (is_connect_request(request)) {
            printf("Received CONNECT request...\n");

            char *host = strtok(request + 8, ":");
            char *port_str = strtok(NULL, " ");
            int port = atoi(port_str);

            printf("Connecting to %s:%d...\n", host, port);

            int socket_server = socket(AF_INET, SOCK_STREAM, 0);

            if (socket_server < 0) {
                perror("Failed to create server socket");
                close(socket_client);
                return;
            }

            struct sockaddr_in server_addr;
            memset(&server_addr, 0, sizeof(server_addr));
            server_addr.sin_family = AF_INET;
            server_addr.sin_port = htons(port);

            struct addrinfo hints, *server_info;

            memset(&hints, 0, sizeof(hints));
            hints.ai_family = AF_INET;
            hints.ai_socktype = SOCK_STREAM;

            int getaddrinfo_result = getaddrinfo(host, port_str, &hints, &server_info);

            if (getaddrinfo_result != 0) {
                fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(getaddrinfo_result));
                close(socket_server);
                close(socket_client);
                return;
            }

            struct sockaddr_in *server_addr_in = (struct sockaddr_in *)server_info->ai_addr;
            memcpy(&server_addr.sin_addr.s_addr, &server_addr_in->sin_addr, sizeof(struct in_addr));

            freeaddrinfo(server_info);

            if (connect(socket_server, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
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

    // Set SO_REUSEADDR option to avoid "address already in use" error
    int optval = 1;
    if (setsockopt(socket_server, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
        perror("Set SO_REUSEADDR failed...\n");
        exit(EXIT_FAILURE);
    }

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    printf("\nSocket created...\n");

    if (bind(socket_server, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
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

    for (int i = 0; i < MAX_CLIENTS; i++) {
        socket_client = accept(socket_server, (struct sockaddr *)&client_address, &cli_len);

        if (socket_client == -1) {
            perror("Client connection error...\n");
            continue;
        }

        printf("\nThe client is connected...\n");

        pid_t pid = fork();

        if (pid == -1) {
            perror("Fork error...\n");
            close(socket_client);
            continue;
        } else if (pid == 0) {
            // Child process
            close(socket_server);//child closes server socket to avoid conflict
            proxy_to_server(socket_client);
            exit(EXIT_SUCCESS);
        } else {
            // Parent process
            close(socket_client);//parent process handles server socket ,only chie handles client socket so close.
        }
    }

    close(socket_server);

    return 0;
}

/*This code appears to be part of a simple server program that uses fork() to create a child process for handling each incoming client connection. Let's break down the code:

1. `pid_t pid = fork();`: This line creates a new process by duplicating the existing process. After the fork, there are two identical processes running concurrently, known as the parent and the child. The `pid` variable will contain the process ID (PID) of the child in the parent process and 0 in the child process.

2. `if (pid == -1) { ... }`: This block of code is executed in the parent process if fork() fails, indicated by returning -1. In this case, an error message is printed using perror(), and the socket connected to the client is closed. The program then continues to the next iteration of the loop, presumably waiting for the next client connection.

3. `else if (pid == 0) { ... }`: This block of code is executed in the child process. The child process first closes the socket connected to the server (presumably to avoid conflicts with the parent process), then calls a function `proxy_to_server()` to handle communication between the client and the server. Finally, the child process exits with EXIT_SUCCESS.

4. `else { ... }`: This block of code is executed in the parent process. After forking, the parent process closes the socket connected to the client. This is because the child process is responsible for handling communication with the client, and the parent doesn't need the client socket.

In summary, the code is creating a new process for each incoming client connection. The child process is responsible for proxying communication between the client and the server, while the parent process quickly closes the client socket and continues to wait for the next connection. If there's an error during the fork, the parent process prints an error message and closes the client socket before moving on to the next iteration of the loop.*/

