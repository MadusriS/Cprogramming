#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<error.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<unistd.h>
#define MAX 65536
#define PORT 1234
/*n = getaddrinfo(host_name, port, &hints, &hserv);
host_name: This is the host name or IP address of the remote server you want to connect to. In your code, it's extracted from the HTTP request.

port: This is the service name or port number as a string where you want to connect on the remote server. In HTTP communication, it's typically set to "80" for regular HTTP traffic. In your code, you extract it from the HTTP request or set it as needed.

&hints: This is a pointer to a struct addrinfo named hints that you've previously defined and initialized. It provides hints to the getaddrinfo function regarding the type of socket addresses you are interested in and other preferences. In your code, it's set to allow any address family (AF_UNSPEC) and specify the socket type (SOCK_STREAM for TCP).

&hserv: This is a pointer to a pointer to struct addrinfo where the resulting address information will be stored. After the function call, hserv will point to a linked list of struct addrinfo structures containing the resolved addresses.

n: This variable stores the return value of the getaddrinfo function. If the function succeeds, it returns 0, and if there is an error, it returns a non-zero value. In your code, it is used for error checking.

The getaddrinfo function performs DNS resolution, considering the provided host name and service (port) information. It may return multiple possible addresses, and the resulting linked list of struct addrinfo structures (hserv) can be used to iterate through the available options.*/
/*struct addrinfo {
    int              ai_flags;      // AI_PASSIVE, AI_CANONNAME, etc.
    int              ai_family;     // AF_INET, AF_INET6, AF_UNSPEC
    int              ai_socktype;   // SOCK_STREAM, SOCK_DGRAM, etc.
    int              ai_protocol;   // 0 for "any"
    size_t           ai_addrlen;    // size of ai_addr in bytes
    struct sockaddr *ai_addr;       // struct sockaddr containing the actual address
    char            *ai_canonname;  // canonical name for the host
    struct addrinfo *ai_next;       // linked list, next structure
};
*/
/*#include <netinet/in.h>

struct sockaddr_in {
    short            sin_family;    // AF_INET
    unsigned short   sin_port;      // Port number (in network byte order - big-endian)
    struct in_addr   sin_addr;      // IP address
    char             sin_zero[8];   // Padding to make the structure the same size as struct sockaddr
};
struct in_addr {
    in_addr_t s_addr;  // IPv4 address
};
*/

void proxy_to_server(int socket_client)
{
	char request[MAX],host_name[MAX],response[MAX];
	int i,j,k=0,n,socket_server,bytes_received;
	size_t r;
  	struct addrinfo *hserv,hints,*p;
	memset(request,0,sizeof(request));
	/*#include <unistd.h>

ssize_t read(int fd, void *buf, size_t count);
fd: File descriptor referring to the open file, socket, or other input/output resource from which to read.
buf: Pointer to the buffer where the read data will be stored.
count: Number of bytes to read.*/

	if((r = read(socket_client,request,MAX))>0)
	{
			if(r>0)
			{
				printf("the Request is:\n");
				printf("%s\n",request);
			}}
			memset(&host_name,'\0',strlen(host_name));
			if(strstr(request,"CONNECT")==NULL){
			 printf("Unsupported HTTP method\n");
        return;
    }
     char *host = strtok(request + 8, ":");
    char *port_str = strtok(NULL, " ");
    int port = atoi(port_str);
    printf("Connecting to %s:%d...\n\n", host, port);
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Failed to create server socket");
        return;
    }
    printf("Proxy Server - %s:%d socket created successfully...\n",host, port);
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    struct hostent *server_hostent = gethostbyname(host);
    if (server_hostent == NULL) {
        perror("Failed to resolve destination server");
        close(server_socket);
        return;
    }
	printf("%s:%d address resolved...\n",host, port);
    memcpy(&server_addr.sin_addr.s_addr, server_hostent->h_addr, server_hostent->h_length);
    if (connect(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Failed to connect to destination server");
        close(server_socket);
        return;
    }
    const char *success_response = "HTTP/1.1 200 Connection established\r\n\r\n";
    send(socket_client, success_response, strlen(success_response), 0);
	printf("Connected to %s:%d\n",host, port);
    fd_set read_fds;
    int max_fd = (socket_client > server_socket) ? socket_client : server_socket;
    while (1)
	{
        FD_ZERO(&read_fds);
        FD_SET(socket_client, &read_fds);
        FD_SET(server_socket, &read_fds);
        if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) < 0) {
            perror("Select error");
            break;
        }
        if (FD_ISSET(socket_client, &read_fds)) {
            bytes_received = recv(socket_client, request, sizeof(request), 0);
            if (bytes_received <= 0) {
                break;
            }
            send(server_socket, request, bytes_received, 0);
        }
        if (FD_ISSET(server_socket, &read_fds)) {
            bytes_received = recv(server_socket, request, sizeof(request), 0);
            if (bytes_received <= 0) {
                break;
            }
            send(socket_client, request, bytes_received, 0);
        }
    }
    close(server_socket);
}
			
			
			
			
			
			
			
			
			
			
			/*for(i=0;i<sizeof(request)-2;i++)
			{	//http://mekkenlar.com
				if(request[i] == '/' && request[i+1] == '/')
				{
					k=0;
					k=i+2;
					for(j=0;request[k] !='/';j++)
					{
						host_name[j]=request[k];
						k++;
					}
					host_name[j]='\0';
					break;
				}
			}
			char* portchr = strstr(host_name,":");
			if(portchr != NULL)
			{
				*portchr = '\0';
				port = portchr + 1;
			}
			printf("Server hostname: %s\n",host_name);
			printf("Server port: %s\n", port);	
			memset(&hints,'\0',sizeof(hints));
			hints.ai_family = AF_UNSPEC;
			hints.ai_socktype = SOCK_STREAM;		
			if((n=getaddrinfo(host_name,port,&hints,&hserv))!= 0)
			{
				herror("Getaddrinfo error...\n");
				exit(-1);
			}
			for(p = hserv;p!=NULL;p = p->ai_next)
			{
					socket_server = socket(p->ai_family,p->ai_socktype,p->ai_protocol);
					if(socket_server<0)
					{
						perror("Socket creation error...\n");
						continue;
					}
					if(connect(socket_server,p->ai_addr,p->ai_addrlen) < 0)
					{
						close(socket_server);
						perror("Connection error...\n");
						continue;				
					}
					break;	
			}
			if(p == NULL)
			{
				printf("Not connected\n");
				exit(-1);			
			}
			puts("Connected to the remote server...\n");
			if(send(socket_server,request,MAX,0) < 0)
			{
				perror("Send error...\n");
				exit(-1);
			}
			puts("Request sent to the remote server..\n");
			int byte_count = 0; 
			memset(&response,'\0',sizeof(response));
			while((byte_count = recv(socket_server,response,MAX,0)) > 0)
				send(socket_client,response,byte_count,0);	
			printf("Data completely sent to the web browser through HTTP Proxy...\n");
			//printf("-----------------------------------------------------------");
			close(socket_server);	
		}
		close(socket_client);
}

*/




int main()
{
	int socket_server,socket_client;
	socklen_t cli_len;
	size_t l;
	struct sockaddr_in server_address,client_address;
	socket_server = socket(AF_INET,SOCK_STREAM,0);
	if(socket_server == -1)
	{
		perror("Socket creation error...\n");
		exit(-1);
	}
	memset(&server_address,0,sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(PORT);
	server_address.sin_addr.s_addr = INADDR_ANY;
	printf("\nSocket created...\n");
	if(bind(socket_server,(struct sockaddr*)&server_address,sizeof(server_address)) < 0)
	{	
		perror("Bind error...\n");
		exit(-1);
	}
	printf("Bind is done...\n");
	if(listen(socket_server,5) == -1)
	{
		perror("listen error...\n");
		exit(-1);
	}
	printf("Proxy server listening at port %d...\n",PORT);
	cli_len = sizeof(client_address);
	while(1)
	{
		socket_client = accept(socket_server,(struct sockaddr*) &client_address,&cli_len);
		if(socket_client == -1)
		{
			perror("Client connection error...\n");
			exit(-1);		
		}
		printf("\nthe Client is connected...\n");
		
		proxy_to_server(socket_client);
		close(socket_client);
	}
	close(socket_server);
	return 0;
}
