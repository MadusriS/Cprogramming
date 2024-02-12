/*#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>

#define PORT 12345
#define MAX_CLIENTS 10
#define MAX_BUFFER_SIZE 1024

void handle_client(int client_socket) {
    char buffer[MAX_BUFFER_SIZE];
    int bytes_received;

    while ((bytes_received = recv(client_socket, buffer, sizeof(buffer), 0)) > 0) {
        // Handle the received data (e.g., echo back to the client)
        send(client_socket, buffer, bytes_received, 0);
    }

    close(client_socket);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    fd_set master_fds, read_fds;
    int max_fd, activity;

    // Create socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind socket to address
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error binding socket");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, MAX_CLIENTS) == -1) {
        perror("Error listening for connections");
        exit(EXIT_FAILURE);
    }

    printf("Multi-client server is listening on port %d\n", PORT);

    FD_ZERO(&master_fds);
    FD_SET(server_socket, &master_fds);
    max_fd = server_socket;

    while (1) {
        read_fds = master_fds;
        activity = select(max_fd + 1, &read_fds, NULL, NULL, NULL);

        if (activity == -1) {
            perror("Error in select");
            exit(EXIT_FAILURE);
        }

        // Check for incoming connection
        if (FD_ISSET(server_socket, &read_fds)) {
            if ((client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len)) == -1) {
                perror("Error accepting connection");
                exit(EXIT_FAILURE);
            }

            printf("Accepted connection from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

            FD_SET(client_socket, &master_fds);
            if (client_socket > max_fd) {
                max_fd = client_socket;
            }
        }

        // Check for data from clients
        for (int i = 0; i <= max_fd; i++) {
            if (FD_ISSET(i, &read_fds) && i != server_socket) {
                handle_client(i);
                FD_CLR(i, &master_fds);
            }
        }
    }

    // Close the server socket
    close(server_socket);

    return 0;
}
*/
/*#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/poll.h>

#define PORT 12345
#define MAX_CLIENTS 10
#define MAX_BUFFER_SIZE 1024

void handle_client(int client_socket) {
    char buffer[MAX_BUFFER_SIZE];
    int bytes_received;

    while ((bytes_received = recv(client_socket, buffer, sizeof(buffer), 0)) > 0) {
        // Handle the received data (e.g., echo back to the client)
        send(client_socket, buffer, bytes_received, 0);
    }

    close(client_socket);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    struct pollfd fds[MAX_CLIENTS + 1]; // Additional slot for server socket
    int nfds = 1; // Number of file descriptors to monitor initially (server socket)

    // Create socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind socket to address
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error binding socket");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, MAX_CLIENTS) == -1) {
        perror("Error listening for connections");
        exit(EXIT_FAILURE);
    }

    printf("Multi-client server is listening on port %d\n", PORT);

    // Initialize the pollfd structure for the server socket
    fds[0].fd = server_socket;
    fds[0].events = POLLIN;

    while (1) {
        // Wait for events on multiple file descriptors using poll
        int ready = poll(fds, nfds, -1);

        if (ready == -1) {
            perror("Error in poll");
            exit(EXIT_FAILURE);
        }

        // Check for events on the server socket
        if (fds[0].revents & POLLIN) {
            // Accept a new connection
            if ((client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len)) == -1) {
                perror("Error accepting connection");
                exit(EXIT_FAILURE);
            }

            printf("Accepted connection from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

            // Add the new client socket to the list
            fds[nfds].fd = client_socket;
            fds[nfds].events = POLLIN;
            nfds++;
        }

        // Check for events on client sockets
        for (int i = 1; i < nfds; i++) {
            if (fds[i].revents & (POLLIN | POLLHUP)) {
                // Handle communication with the client
                handle_client(fds[i].fd);

                // Remove the closed client socket from the list
                close(fds[i].fd);
                fds[i] = fds[nfds - 1];
                nfds--;
                i--;
            }
        }
    }

    // Close the server socket
    close(server_socket);

    return 0;
}
*/
#include<stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <poll.h>
#include<time.h>

#define PORT 8080
#define MAX_EVENTS 10

int main()
{
    int server_fd, new_socket, valread;
    struct sockaddr_in server;
    char buffer[1024] = {0};

    // Create a socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(0);
    }

    // Set socket options to allow reusing the address
    int yes = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &yes, sizeof(int)))
    {
        perror("setsockopt");
        exit(0);
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    // Bind the socket to port
    if (bind(server_fd, (struct sockaddr *)&server, sizeof(server))<0)
    {
        perror("bind failed");
        exit(0);
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0)
    {
        perror("error while listening");
        exit(0);
    }
    /*struct pollfd {
    int   fd;        // file descriptor //
    short events;    // requested events//
    short revents;   //returned events //
};
*/

    struct pollfd fds[MAX_EVENTS];
    // Number of file descriptors being monitored
    int des = 1;
    memset(fds, 0, sizeof(fds));
    /*In the context of a server, when you set fds[0].events = POLLIN, it means the server is interested in detecting whether there is incoming data (readable) on the file descriptor associated with the server socket. This is typically used to check if there are new client connections waiting to be accepted.

Here's a brief overview of some commonly used events in the events field:

POLLIN: There is data to read.
POLLOUT: Writing is possible without blocking.
POLLERR: There is an error on the file descriptor.
POLLHUP: The file descriptor is no longer connected (hang up).
POLLNVAL: The file descriptor is not open.
*/

    // Add server socket to the pollfd structure
    fds[0].fd = server_fd;
    fds[0].events = POLLIN;
    printf("connections from client...\n");

    socklen_t addrlen = sizeof(server);
    while(1)
    {
        // Call poll() to wait for events
        /*int poll(struct pollfd *fds, nfds_t nfds, int timeout);
        int cnt = poll(fds, des, -1);
        fds: It is an array of struct pollfd structures, where each structure describes a file descriptor that the caller is interested in monitoring for events.

nfds: It is the number of elements in the fds array. It represents the highest file descriptor value in the array plus 1.

timeout: It is the maximum time, in milliseconds, that poll should block waiting for events. The value -1 indicates an infinite timeout, meaning the function will block indefinitely until an event occurs on one of the monitored file descriptors.*/


        int cnt = poll(fds, des, -1);
        if (cnt < 0)
        {
            perror("error in poll");
            exit(0);
        }

        // Check for events on each file descriptor
        for (int i = 0; i < des; i++)
        {
        /* if (fds[i].revents & POLLIN) checks if the POLLIN event occurred on the file descriptor at index i in the fds array. If the condition is true, it means that there is data available to read on that file descriptor.*/
            if (fds[i].revents & POLLIN)
            {
                // New incoming connection
                /*if (fds[i].fd == server_fd) checks if the file descriptor at index i in the fds array is the same as the server_fd. If the condition is true, it means that the event is associated with the server socket.*/
                if (fds[i].fd == server_fd)
                {
                    if ((new_socket = accept(server_fd, (struct sockaddr *)&server, &addrlen))<0)
                    {
                        perror("error while accepting...");
                        exit(0);
                    }

                    printf("New connection, socket fd is %d, ip is: %s,\n",new_socket, inet_ntoa(server.sin_addr));

                    // Adding new connection to the pollfd structure
                    fds[des].fd = new_socket;
                    fds[des].events = POLLIN;
                    des++;
                }
                // Data available to read on an existing connection
                else
                {
                    int client_socket = fds[i].fd;
                    valread = read(client_socket, buffer, 1024);
                    // Client disconnected
                    if (valread == 0)
                    {
                        printf("Client disconnected\n");
                        close(client_socket);
                        // removing file descriptor
                        fds[i].fd = -1;
                    }
                    else
                    {
                        time_t cur_time;
                        time(&cur_time);
                        /*printf("time: [%02d:%02d:%02d] Received message from client: %.*s",
               time_info->tm_hour, time_info->tm_min, time_info->tm_sec,
               bytes_received, buffer);*/
                        printf("time:%s\n Received message from client:%s",ctime(&cur_time),buffer);
                        strcat(buffer," - ");
                        strcat(buffer,ctime(&cur_time));
                        send(fds[i].fd,buffer,valread,0);
                        memset(&buffer,'\0',sizeof(buffer));
                    }
                }
            }
        }
    }
    return 0;
}
