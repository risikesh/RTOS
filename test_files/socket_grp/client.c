// Client side C/C++ program to demonstrate Socket programming 
#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 
#include <pthread.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <signal.h>

#define PORT 8080 
int sock = 0;
pthread_t ptid; 

void handle_sig()
{
	pthread_kill(ptid,SIGINT);
	close(sock);
	exit(0);
}

void* recv_read(void *v)
{
	int valread;
	int *sock = (int *)v;
	char buffer[1024];
	while(1)
	{
		valread = read( *sock , buffer, 1024); 
		if(valread)
		printf("Message:%s\n",buffer );
	}
	 
}

int main(int argc, char const *argv[]) 
{ 
	int valread; 
	struct sockaddr_in serv_addr; 
	char hello[1024] = "Hello from client"; 
	char buffer[1024] = {0}; 
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{ 
		printf("\n Socket creation error \n"); 
		return -1; 
	} 

	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(PORT); 
	signal(SIGINT,handle_sig);

	// Convert IPv4 and IPv6 addresses from text to binary form 
	if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) 
	{ 
		printf("\nInvalid address/ Address not supported \n"); 
		return -1; 
	} 

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
	{ 
		printf("\nConnection Failed \n"); 
		return -1; 
	} 
	
    pthread_create(&ptid, NULL, &recv_read, &sock); 
	
	while(1)
	{
		fgets(hello,1024,stdin);
		write(sock , hello , strlen(hello)); 
		printf("sent_from_client\n");
	}
	 
	
	return 0; 
} 
