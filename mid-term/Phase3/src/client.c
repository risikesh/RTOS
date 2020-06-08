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

#include <utils.h>
#include <msg.h>
#include <ntp.h>

int sock = 0;
pthread_t pt_receiveVoice,pt_sendVoice; 
struct group_message msg;
struct group_chat_acknowledge ack;
FILE *fp;

void handle_sig()
{
	
		printf("killing client,PID=%d\n",getpid());
		pthread_kill(pt_receiveVoice,SIGINT);
		pthread_kill(pt_sendVoice,SIGINT);
		close(sock);
		exit(0);
}


int main(int argc, char const *argv[]) 
{ 	
	printf("client created,PID=%d\n",getpid());
	struct sockaddr_in serv_addr;  
    	if(argc!=5)
    	{
        	printf("Wrong Arguments!! \nUsage:%s <Server-IP-Addr> <Port> <User-Name> <Group-Name>\n",argv[0]);
       		exit(EXIT_FAILURE);
    	}
	
    
    int port=atoi(argv[2]);
    struct group_chat_request req;
        
    strcpy(req.group_name,argv[4]);
    strcpy(req.user_name,argv[3]);

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{ 
		printf("\n Socket creation error \n"); 
		return -1; 
	} 

	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(port); 
	signal(SIGINT,handle_sig);

	if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0) 
	{ 
		printf("\nInvalid address/ Address not supported \n"); 
		return -1; 
	} 

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
	{ 
		printf("\nConnection Failed \n"); 
		return -1; 
	} 
	write(sock,&req,sizeof(req));
    read(sock,&ack,sizeof(ack));
    
    msg.group_identifier=ack.group_identifier;
    msg.user_identifier=ack.user_identifier;
    strcpy(msg.user_name,req.user_name);
    //printf("Group_IDENTIFIER:%d\nUser_IDENTIFIER:%d\n",ack.group_identifier,ack.user_identifier);
	struct pulseaudio_con socket_client;
	socket_client.socket=sock;
	socket_client.message=msg;
	
    pthread_create(&pt_receiveVoice, NULL, &voice_receive, &socket_client); 
    pthread_create(&pt_sendVoice, NULL, &voice_send, &socket_client); 

	pthread_join(pt_receiveVoice,NULL);
	pthread_join(pt_sendVoice,NULL);
	return 0; 
} 
