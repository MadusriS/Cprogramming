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

#define MAX 65536
#define PORT 8060
#define MAX_CLIENTS 100

int is_connect_request(const char *request) {
    return (strncmp(request, "CONNECT", 7) == 0);
}

void handle_connect_request(int socket_client, int socket_server) {
    const char *success_response = "HTTP/1.1 200 OK\r\n\r\n";
    send(socket_client, success_response, strlen(success_response), 0);

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

        pid_t pid = fork();/*In C programming, pid_t is a data type used to represent process IDs (PIDs). The pid_t data type is typically used to store the process ID of a running process. The pid_t type is defined in the <sys/types.h> header file.*/

        if (pid == -1) {
            perror("Fork error...\n");
            close(socket_client);
            continue;
        } else if (pid == 0) {
            // Child process
            /*If fork() returns 0, it means the process is the child process.
In the child process, the parent's socket to the server is closed (socket_server) since it's not needed. Then, the proxy_to_server() function is called to handle communication between the client and the destination server.*/
            close(socket_server);
            proxy_to_server(socket_client);
            exit(EXIT_SUCCESS);
        } else {
            // Parent process
            /*In the parent process, the client socket (socket_client) is closed since it's no longer needed after forking a child process.*/
            close(socket_client);
        }
    }

    close(socket_server);

    return 0;
}

