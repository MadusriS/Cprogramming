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

void proxy_to_server(int socket_client, char* port)
{
	char request[MAX],host_name[MAX],response[MAX];
	int i,j,k=0,n,socket_server;
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
			}
			memset(&host_name,'\0',strlen(host_name));
			for(i=0;i<sizeof(request)-2;i++)
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
		proxy_to_server(socket_client, "80");
	}
	close(socket_server);
	return 0;
}
