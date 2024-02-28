#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include<time.h>

#define PORT 8888
#define MAX_CLIENTS 30

int main() {
    int opt = 1;
    int master_socket, addrlen, new_socket, client_socket[MAX_CLIENTS], max_clients = MAX_CLIENTS, activity, i, valread;
    struct sockaddr_in address;
    struct pollfd fds[MAX_CLIENTS + 1]; // array of structures,+1 for server,Additional space for the master socket

    char buffer[1025]; // data buffer of 1K

    // a message
    char *message = "ECHO Daemon v1.0 \r\n";

    // initialize all client_socket[] to 0 so not checked
    for (i = 0; i < max_clients; i++) {
        client_socket[i] = 0;//initially no client so all 0
    }

    // create a master socket-serversocket
    if ((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // set master socket to allow multiple connections,
    // this is just a good habit, it will work without this
    if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // type of socket created-server address structure
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // bind the socket to localhost port 8888
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Listener on port %d \n", PORT);

    // try to specify maximum of 3 pending connections for the master socket
    if (listen(master_socket, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // waiting for incoming connections 
    addrlen = sizeof(address);
    puts("Waiting for connections ...");

    // Initialize the pollfd structure for the master socket
    fds[0].fd = master_socket;//first index of the array will always the server
    fds[0].events = POLLIN;

    for (i = 1; i < max_clients + 1; i++) {
        fds[i].fd = 0;  // 0 means not used
    }

    while (1) {
        // Use poll to wait for an event
        activity = poll(fds, max_clients + 1, -1);/*if any client connect to server...poll will change the value of revents it will change the activity,<0=error,0=no connection,>0=ready to connect*/

        if ((activity < 0) && (errno != EINTR)) {
            printf("poll error");
        }

        // If there is a new incoming connection//revents ka value keep changing
        if (fds[0].revents & POLLIN) {
            if ((new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            // inform the user of socket number - used in send and receive commands
            printf("New connection, socket fd is %d, ip is : %s, port : %d\n",
                   new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

            // send new connection greeting message
            if (send(new_socket, message, strlen(message), 0) != strlen(message)) {
                perror("send");
            }

            puts("Welcome message sent successfully");

            // add new socket to the array of sockets//since 0 th index for server...start from 1
            for (i = 1; i < max_clients + 1; i++) {
                if (fds[i].fd == 0) {
                    fds[i].fd = new_socket;
                    fds[i].events = POLLIN;
                    break;
                }
            }
        }

        // else it's some IO operation on some other socket
        for (i = 1; i < max_clients + 1; i++) {
            if (fds[i].revents & POLLIN) {
                int sd = fds[i].fd;

                // check if it was for closing, and also read the incoming message
                if ((valread = read(sd, buffer, 1024)) == 0) {
                    // somebody disconnected, get their details and print
                    getpeername(sd, (struct sockaddr *)&address,
                                (socklen_t *)&addrlen);
                    printf("Host disconnected, ip %s, port %d \n",
                           inet_ntoa(address.sin_addr),
                           ntohs(address.sin_port));

                    // close the socket and mark as 0 in the list for reuse
                    close(sd);
                    fds[i].fd = 0;
                } else {
                    // echo back the message that came in to the sender with a timestamp
                    // and broadcast the message to all other connected clients
                    char timestamp[20];
                    time_t now = time(NULL);
                    strftime(timestamp, sizeof(timestamp), "[%Y-%m-%d %H:%M:%S]", localtime(&now));

                    // set the string terminating NULL byte on the end
                    // of the data read
                    buffer[valread] = '\0';

                    // echo back to the sender
                    send(sd, timestamp, strlen(timestamp), 0);
                    send(sd, buffer, strlen(buffer), 0);

                    // broadcast to other connected clients
                   /* for (int j = 1; j < max_clients + 1; j++) {
                        if (fds[j].fd != 0 && fds[j].fd != sd) {
                            send(fds[j].fd, timestamp, strlen(timestamp), 0);
                            send(fds[j].fd, buffer, strlen(buffer), 0);
                        }
                    }*/
                }
            }
        }
    }

    return 0;
}

