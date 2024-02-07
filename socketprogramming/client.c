#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT "3490" // Port for server connection
#define MAXDATASIZE 100 // Maximum data size for message

// Function to get current timestamp
void get_timestamp(char *timestamp) {
    time_t now = time(NULL);
    strftime(timestamp, 20, "%Y-%m-%d %H:%M:%S", localtime(&now));
}

// Declaration of get_in_addr function
void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[]) {
    int sockfd, numbytes;
    char buf[MAXDATASIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];
    char timestamp[20]; // Buffer to hold timestamp

    if (argc != 2) {
        fprintf(stderr,"usage: client hostname\n");
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // Loop through all the results and connect to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
            s, sizeof s);
    printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo); // All done with this structure

    while(1) {
        // Prompt user to enter a message
        printf("Enter message to send to server (type 'quit' to exit): ");
        fgets(buf, sizeof(buf), stdin);
        
        // Remove newline character if present
        if ((strlen(buf) > 0) && (buf[strlen (buf) - 1] == '\n'))
            buf[strlen (buf) - 1] = '\0';

        // If user enters 'quit', exit loop and close socket
        if (strcmp(buf, "quit") == 0)
            break;

        // Get timestamp before sending message
        get_timestamp(timestamp);
        printf("client: sending '%s' at %s\n", buf, timestamp);

        // Send message to server
        if (send(sockfd, buf, strlen(buf), 0) == -1) {
            perror("send");
            exit(1);
        }

        // Receive echoed message from server
        if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
            perror("recv");
            exit(1);
        }

        // Null-terminate received message
        buf[numbytes] = '\0';

        // Get timestamp after receiving echoed message
        get_timestamp(timestamp);
        printf("client: received '%s' from server at %s\n", buf, timestamp);
    }

    close(sockfd);

    return 0;
}


