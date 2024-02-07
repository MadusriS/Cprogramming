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
#define PORT "3490"  // Define the port number as a string constant
#define BACKLOG 10   // Define the maximum length of the pending connections queue

void sigchld_handler(int s)
{
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;/*int saved_errno = errno;: This line saves the value of errno, the global variable that stores the last error code encountered in the program, to saved_errno. This is done to ensure that errno is not inadvertently modified by the waitpid() function call*/
    while(waitpid(-1, NULL, WNOHANG) > 0);/*while(waitpid(-1, NULL, WNOHANG) > 0);: This line repeatedly calls waitpid() with -1 as the process ID, which means it waits for any child process to change state. The WNOHANG flag specifies that waitpid() should return immediately if there are no child processes that have exited. The loop continues until waitpid() returns <= 0, indicating that there are no more exited child processes to reap.*/
    errno = saved_errno;/*errno = saved_errno;: This line restores the value of errno to what it was before the waitpid() calls. This is important because waitpid() might modify errno, and we want to ensure that any error code from previous operations is preserved.*/
}
/*his sigchld_handler function is responsible for reaping zombie processes (child processes that have terminated but whose exit status has not yet been collected by the parent process). It does this by repeatedly calling waitpid() with the WNOHANG option, ensuring that the parent process can continue its execution without blocking to wait for child processes to exit.*/

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    // Check if the address family is IPv4
    if (sa->sa_family == AF_INET) {
        // If it's IPv4, return a pointer to the IPv4 address
        return &(((struct sockaddr_in*)sa)->sin_addr);/* If the address family is IPv4, this line casts the struct sockaddr pointer sa to a pointer to struct sockaddr_in (which is specific to IPv4), then accesses the sin_addr member, which contains the IPv4 address.*/
    }
    // If it's not IPv4 (assumed to be IPv6), return a pointer to the IPv6 address
    return &(((struct sockaddr_in6*)sa)->sin6_addr);/*If the address family is not IPv4 (assumed to be IPv6), this line casts the struct sockaddr pointer sa to a pointer to struct sockaddr_in6 (which is specific to IPv6), then accesses the sin6_addr member, which contains the IPv6 address.*/
}
/*In summary, this function get_in_addr determines whether the given socket address is an IPv4 or IPv6 address and returns a pointer to the appropriate type of address (in_addr for IPv4 or in6_addr for IPv6). It's a utility function commonly used in network programming to extract the IP address from a generic socket address structure (struct sockaddr).*/

int main(void)
{
int sockfd, new_fd; // listen on sock_fd, new connection on new_fd/*Declares two integer variables sockfd and new_fd to represent the listening socket and the socket for a new connection, respectively.*/
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
    // ai_canonname: Canonical name of the host
    // ai_next: Pointer to the next addrinfo structure in the linked list (if any)
    
    
struct sockaddr {
    sa_family_t sa_family;
    char        sa_data[14];
};

*/
struct addrinfo hints, *servinfo, *p;/*Declares three variables:

hints: A structure containing hints about the type of socket address structures that the program supports.
servinfo: A pointer to the linked list of addrinfo structures returned by the getaddrinfo function.
p: A pointer used to iterate through the list of addresses.*/

struct sockaddr_storage their_addr; // connector's address information/*Declares a structure their_addr to store information about the connected client's address.*/

socklen_t sin_size;/*Declares a variable sin_size of type socklen_t, which is used to store the size of the socket address structure.
*/

struct sigaction sa;/*Declares a structure sa of type struct sigaction, which will be used for handling signals.
*/

int yes=1;/*Declares an integer variable yes and initializes it to 1. This is later used for setting socket options.*/

char s[INET6_ADDRSTRLEN];/*Declares a character array s to store the IP address in string form. The size is set to INET6_ADDRSTRLEN, which is typically large enough to accommodate IPv6 addresses.*/

int rv;/*Declares an integer variable rv to store the return value of various functions, primarily used for error checking.*/

memset(&hints, 0, sizeof hints);/*Initializes the hints structure to zero using memset. This is a common practice to ensure that all fields are properly initialized before setting specific values.
*/

hints.ai_family = AF_UNSPEC;/*Sets the address family in hints to AF_UNSPEC, indicating that the program can work with both IPv4 and IPv6 addresses.*/

hints.ai_socktype = SOCK_STREAM;/*Sets the socket type in hints to SOCK_STREAM, indicating a TCP socket.*/

hints.ai_flags = AI_PASSIVE; // use my IP    /*Sets the AI_PASSIVE flag in hints, indicating that the returned socket addresses should be suitable for binding a socket that will accept connections. The IP address will be the host's IP.*/

if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    return 1;
}

/*This section uses getaddrinfo() to retrieve address information that matches certain criteria specified by hints (not shown in this snippet). 
NULL is passed as the first parameter, indicating that the local address should be used. 
PORT is expected to be a string representing the port number or service name (e.g., "8080").
If getaddrinfo() fails, it returns a non-zero value, and the error is printed to the standard error stream using fprintf(). 
The gai_strerror() function is used to convert the return value into a human-readable string.
If an error occurs, the program returns with an exit code of 1.
*/

// Iterate through the linked list of addrinfo structures returned by getaddrinfo()
for(p = servinfo; p != NULL; p = p->ai_next) {
    // Create a socket using the address family, socket type, and protocol specified in the current addrinfo structure (p)
    if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
        perror("server: socket"); // Print an error message if socket creation fails
        continue; // Skip to the next addrinfo structure in the linked list
    }
    
    // Set socket options to allow reusing the address and port
    /*It sets the socket option SO_REUSEADDR using setsockopt(). This allows the socket to be reused even if it is in the TIME_WAIT state, preventing "address already in use" errors.*/
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        perror("setsockopt"); // Print an error message if setting socket options fails
        exit(1); // Terminate the program
    }
    
    // Bind the socket to the address and port specified in the current addrinfo structure (p)
    if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
        close(sockfd); // Close the socket
        perror("server: bind"); // Print an error message if binding fails
        continue; // Skip to the next addrinfo structure in the linked list
    }
    
    // If the socket is successfully bound, break out of the loop
    break;
}

freeaddrinfo(servinfo); // Free the memory allocated for the addrinfo structure (servinfo) as it's no longer needed

if (p == NULL) { // Check if the pointer p is NULL, indicating failure to bind
    fprintf(stderr, "server: failed to bind\n"); // Print an error message
    exit(1); // Exit the program with an error code
}

if (listen(sockfd, BACKLOG) == -1) { // Start listening on the socket for incoming connections
    perror("listen"); // Print an error message if listen fails
    exit(1); // Exit the program with an error code
}
/*struct sigaction {
    void (*sa_handler)(int);    // Pointer to the signal handler function or SIG_IGN/SIG_DFL
    sigset_t sa_mask;           // Additional set of signals to be blocked during execution of the signal handler
    int sa_flags;               // Flags modifying the behavior of signal handling
    void (*sa_sigaction)(int, siginfo_t *, void *); // Alternate signal handler function with more information
};
sa_flags: This member contains flags that modify the behavior of signal handling. Common flags include:

SA_RESTART: Causes interrupted system calls to be automatically restarted after the signal handler returns.
SA_SIGINFO: Indicates that the signal handler is expecting additional information about the signal (such as a pointer to a siginfo_t structure).
*/

sa.sa_handler = sigchld_handler; // Set up signal handling to reap all dead processes
sigemptyset(&sa.sa_mask); // Initialize the signal set to empty
sa.sa_flags = SA_RESTART; // Restart interrupted system calls if possible
if (sigaction(SIGCHLD, &sa, NULL) == -1) { // Set up signal handling for SIGCHLD (child termination)
    perror("sigaction"); // Print an error message if sigaction fails
    exit(1); // Exit the program with an error code
}

printf("server: waiting for connections...\n"); // Print a message indicating that the server is waiting for connections

while(1) { // Main accept() loop: continuously accept incoming connections
    sin_size = sizeof their_addr; // Get the size of the client's socket address structure
    new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size); // Accept a new incoming connection
    if (new_fd == -1) { // Check if accept() fails
        perror("accept"); // Print an error message if accept fails
        continue; // Continue to the next iteration of the loop
    }
  /*The following block is inside a conditional statement that checks if the fork() call was successful in creating a child process. If the fork() call is successful, it means we are in the child process, so we perform specific actions (close the listener socket, send a message to the client, and then exit the child process). Otherwise, we're in the parent process, so we simply close the socket connection.

This code demonstrates a common pattern in network programming where a server handles client connections by forking a child process to handle each connection independently, allowing the parent process to continue listening for new connections.*/  
    inet_ntop(their_addr.ss_family, /* This function converts a network address (IP address) from binary form to a presentation format (such as a string in IPv4 or IPv6 format).
    their_addr.ss_family: This member of struct sockaddr_storage specifies the address family of the client's socket address.*/
          get_in_addr((struct sockaddr *)&their_addr),s, sizeof s); /* Get the IP address of the client connection/*This function retrieves the IP address (in binary form) from the struct sockaddr pointer passed to it, which is cast to struct sockaddr //Store the converted address in the buffer s*/
printf("server: got connection from %s\n", s); // Print the client's IP address
if (!fork()) { // Fork to create a child process for handling the client connection
    close(sockfd); // Close the listener socket in the child process
    if (send(new_fd, "Hello, world!", 13, 0) == -1) // Send a message to the client
        perror("send"); // Print an error message if sending fails
    close(new_fd); // Close the socket connection in the child process
    exit(0); // Exit the child process
}
close(new_fd); // Close the socket connection in the parent process
}
return 0;
}



