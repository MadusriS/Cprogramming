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
#include <sys/wait.h>
#include <signal.h>
#include <time.h>

#define PORT "3490"
#define BACKLOG 10  // Define the maximum length of the pending connections queue


void sigchld_handler(int s) {
    while(waitpid(-1, NULL, WNOHANG) > 0);
}

// Declaration of get_in_addr function
//get sockaddr-ipv4 or 6
/*struct sockaddr_in {
    short int          sin_family;  // Address family (AF_INET)
    unsigned short int sin_port;    // Port number
    struct in_addr     sin_addr;    // IPv4 address
    unsigned char      sin_zero[8]; // Padding to make the structure the same size as struct sockaddr
};
*/
void *get_in_addr(struct sockaddr *sa) {
//check if the address is ipv4
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    //check if the address is ipv6
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void get_timestamp(char *timestamp) {
    time_t now = time(NULL);
    strftime(timestamp, 20, "%Y-%m-%d %H:%M:%S", localtime(&now));//strftime change time to string
}
/*/*addrinfo and sockaddr structures are part of the networking libraries provided by the operating system, typically found in the <sys/socket.h> and <netdb.h> header files.
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

int main(void) {
    int sockfd, new_fd;//server fd ,newfd 
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr;/*// connector's address information/*Declares a structure their_addr to store information about the connected client's address.*/

    socklen_t sin_size;/*In this example, sin_size is set to the size of the server_addr structure using the sizeof operator. This ensures that the bind() function receives the correct size of the structure, allowing it to properly bind the socket to the specified address and port.
    It's worth noting that socklen_t is a datatype used for representing sizes of socket-related structures. I*/
    struct sigaction sa;/*Declares a structure sa of type struct sigaction, which will be used for handling signals.
*/

    int yes = 1;
    char s[INET6_ADDRSTRLEN];
    char timestamp[20];

    memset(&hints, 0, sizeof hints);/*In the line memset(&hints, 0, sizeof hints);, hints is a structure of type struct addrinfo that is used to provide hints or preferences to the getaddrinfo() function, which is used for address resolution (converting a hostname and service name into a set of usable socket addresses).
    The 0 passed as the third argument to memset() signifies that we want to fill the memory with zeros. This effectively initializes all fields of the hints structure to zero.*/
    hints.ai_family = AF_UNSPEC;/*Sets the address family in hints to AF_UNSPEC, indicating that the program can work with both IPv4 and IPv6 addresses.*/

    hints.ai_socktype = SOCK_STREAM;//for tcp connection
    hints.ai_flags = AI_PASSIVE;// use my IP 
    /*getaddrinfo(NULL, PORT, &hints, &servinfo): This function call is invoking the getaddrinfo() function, which is used to perform DNS (Domain Name System) resolution and retrieve address information that can be used to establish network connections. The parameters passed to getaddrinfo() are:

NULL: This indicates that we're not specifying a particular hostname. When set to NULL, getaddrinfo() will use the local host's address.
PORT: This is a macro or variable representing the port number we want to connect to. It specifies the service port to be used for address resolution.
&hints: This is a pointer to a struct addrinfo object named hints. This structure contains hints or preferences for the address resolution process, guiding getaddrinfo() on how to perform the resolution. It provides information about the desired type of socket addresses and protocol.
&servinfo: This is a pointer to a pointer of type struct addrinfo, where the address information retrieved by getaddrinfo() will be stored.
*/

    if (getaddrinfo(NULL, PORT, &hints, &servinfo) != 0) {
        perror("getaddrinfo");
        return 1;
    }

    for(p = servinfo; p != NULL; p = p->ai_next) {
    /*creates new socket*/
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1)/*p->ai_protocol usually 0*/ {
            perror("server: socket");
            continue;
        }
 // Set socket options to allow reusing the address and port
    /*It sets the socket option SO_REUSEADDR using setsockopt(). This allows the socket to be reused even if it is in the TIME_WAIT state, preventing "address already in use" errors.*/

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }
        /*For example, in the line perror("setsockopt");, if the setsockopt() function fails, perror() will print an error message to stderr in the following format:
setsockopt: <error message corresponding to the current value of errno>
*/
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo);

    if (p == NULL) {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    printf("server: waiting for connections...\n");

    while(1) {
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);/*their addr is connected clients adres*/
        if (new_fd == -1) {
            perror("accept");
            continue;
        }

        get_timestamp(timestamp);
        /*inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s): This function call converts the binary IP address stored in the their_addr structure to a human-readable string representation and stores it in the character array s. 

their_addr.ss_family: The address family (e.g., AF_INET for IPv4 or AF_INET6 for IPv6) stored in the ss_family member of the their_addr structure.
get_in_addr((struct sockaddr *)&their_addr): This function call retrieves the IP address from the their_addr structure and returns it as a pointer to a struct in_addr or struct in6_addr (depending on the address family).
s: A character array where the resulting string representation of the IP address will be stored.
s
printf("server: got connection from %s at %s\n", s, timestamp);: This line prints the information about the connection, including the client's IP address (stored in s) and the timestamp when the connection was established (stored in the timestamp variable).

In summary, the inet_ntop() function converts the binary IP address to a human-readable string, which is then printed along with the timestamp of the connection.*/
        printf("server: got connection from %s at %s\n",
                inet_ntop(their_addr.ss_family,
                get_in_addr((struct sockaddr *)&their_addr),
                s, sizeof s), timestamp);

        if (!fork()) {
            close(sockfd);

            char buf[1024];
            int numbytes;

            while ((numbytes = recv(new_fd, buf, sizeof(buf), 0)) > 0)/*when input string is given*/ {
                buf[numbytes] = '\0';
                printf("server: received '%s'\n", buf);

                get_timestamp(timestamp);
                printf("server: received at %s\n", timestamp);

                if (send(new_fd, buf, numbytes, 0) == -1) {
                    perror("send");
                    exit(1);
                }

                get_timestamp(timestamp);
                printf("server: echoed back at %s\n", timestamp);
            }

            close(new_fd);
            exit(0);
        }

        close(new_fd);
    }

    return 0;
}

