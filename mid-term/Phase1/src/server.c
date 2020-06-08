// Server side C/C++ program to demonstrate Socket programming 
#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 
#include <pthread.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <errno.h>
#include <pulse/simple.h>
#include <pulse/error.h>
#include <utils.h>

pthread_t pt_voice_receive;
pthread_t pt_voice_send;
int new_socket;

void handle_sig()
{
	pthread_kill(pt_voice_receive,SIGINT);
	pthread_kill(pt_voice_send,SIGINT);
	close(new_socket);
	exit(EXIT_SUCCESS);
}

int main(int argc, char const *argv[]) 
{ 

	
	if (argc < 3)
	{
		printf("Usage: %s <port> <mode>",argv[0]);
		exit(EXIT_FAILURE);
	}

	int server_fd, valread; 
	struct sockaddr_in address; 
	int opt = 1;
	int addrlen = sizeof(address); 
	
	// Creating socket file descriptor 
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
	{
		perror("socket failed"); exit(EXIT_FAILURE); 
	} 
	
	// attaching socket to the port 
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) 
	{ 
		perror("setsockopt"); exit(EXIT_FAILURE); 
	} 

	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY; 
	address.sin_port = htons( atoi(argv[1]) );

	signal(SIGINT, handle_sig);
	
	// attaching socket to the port
	if (bind(server_fd, (struct sockaddr *)&address,sizeof(address))<0) 
	{ 
		perror("bind failed"); exit(EXIT_FAILURE); 
	} 
	if (listen(server_fd, 3) < 0) 
	{ 
		perror("listen"); exit(EXIT_FAILURE); 
	} 
	if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) 
	{ 
		perror("accept"); 	exit(EXIT_FAILURE); 
	} 
		
    pthread_create(&pt_voice_receive, NULL, &voice_receive, &new_socket); 
    pthread_create(&pt_voice_send, NULL, &voice_send, &new_socket); 
	
	pthread_join(pt_voice_receive,NULL);
	pthread_join(pt_voice_send,NULL);

	return 0; 
} 
