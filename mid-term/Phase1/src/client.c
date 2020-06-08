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


int sock = 0;
pthread_t pt_voice_receive;
pthread_t pt_voice_send;

void handle_sig()
{
	pthread_kill(pt_voice_receive,SIGINT);
	pthread_kill(pt_voice_send,SIGINT);
	close(sock);
	exit(EXIT_SUCCESS); 
}



int main(int argc, char const *argv[]) // $ ./client <server ip-address> <port>
{ 
	if (argc < 3)
	{
		printf("Usage: %s <server ip-address> <port>",argv[0]);
		exit(EXIT_FAILURE);
	}
	
	struct sockaddr_in serv_addr; 
	
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{ 
		printf("\n Socket creation error \n"); 
		return -1; 
	} 

	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(atoi(argv[2])); 
	signal(SIGINT,handle_sig);

	// Convert IPv4 and IPv6 addresses from text to binary form 
	if(inet_pton(AF_INET, argv[1] , &serv_addr.sin_addr)<=0) 
	{ 
		printf("\nInvalid address/ Address not supported \n"); 
		return -1; 
	} 

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
	{ 
		printf("\nConnection Failed \n"); 
		return -1; 
	} 
	struct pulseaudio_con socket_data;
	strcpy(socket_data.name,argv[0]);
	socket_data.socket=sock;

    pthread_create(&pt_voice_receive, NULL, &voice_receive, &socket_data); 
    pthread_create(&pt_voice_send, NULL, &voice_send, &socket_data); 
	
	pthread_join(pt_voice_receive,NULL);
	pthread_join(pt_voice_send,NULL);
	return 0; 
} 
