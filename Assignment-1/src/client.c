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

#include <msg.h>
#include <ntp.h>

enum Mode mode;

int sock = 0;
pthread_t ptid; 
struct group_message msg;
struct group_chat_acknowledge ack;
FILE *fp;

void handle_sig(int sign)
{
	switch (sign)
	{
	case SIGINT:
		printf("killing client,PID=%d\n",getpid());
		pthread_kill(ptid,SIGINT);
		close(sock);
		if(mode==DEV)
		{
			fclose(fp);
		}
		exit(0);
		break;
	case SIGUSR1:
		if(mode==DEV)
		{	//printf("Sending test message\n");
			sprintf(msg.message,"Test message,sent from:%d",getpid());
			msg.time_stamp= GetTimeStamp();
			write(sock , &msg , sizeof(msg)); 
			printf("Me:%s\n",msg.message);
		}
		break;
	}
}

void* recv_read(void *v)
{
	char filename[30];
	if(mode==DEV)
	{
		sprintf(filename,"./log_temp/log_%d_%d.txt",ack.group_identifier,ack.user_identifier);	
		fp = fopen(filename, "w");
		if(fp == NULL)
		{
			printf("Error opening file\n");
			exit(1);
		}
	}

	long long delay=0;
	int *sock = (int *)v;
	struct group_message incoming;

	while(1)
	{
		if(read( *sock , &incoming, sizeof(incoming)))
		{
			
			
			if(mode==DEV){
				//delay=get_time_now(recv_client)-incoming.time_stamp;
				struct timeval currclock= GetTimeStamp();
				delay=((currclock.tv_sec - incoming.time_stamp.tv_sec)*1000*1000) +(currclock.tv_usec- incoming.time_stamp.tv_usec);
				fprintf(fp,"%d,%lld,%s\n",incoming.msgid,delay,incoming.user_name);
			}
			else
				printf("%s:%s\n",incoming.user_name,incoming.message);
			
			
		}
	} 
}

int main(int argc, char const *argv[]) 
{ 	
	printf("client created,PID=%d\n",getpid());
	struct sockaddr_in serv_addr;  
    	if(argc!=6)
    	{
        	printf("Wrong Arguments!! \nUsage:%s <Server-IP-Addr> <Port> <User-Name> <Group-Name> <Mode>\n",argv[0]);
       		exit(EXIT_FAILURE);
    	}
	
    
    int port=atoi(argv[2]);
    struct group_chat_request req;
        
    strcpy(req.group_name,argv[4]);
    strcpy(req.user_name,argv[3]);

    if(!strcmp(argv[5],"DEV"))
        mode=DEV;
    else if(!strcmp(argv[5],"PROD"))
        mode=PROD;
	else
		exit(EXIT_FAILURE);
	
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{ 
		printf("\n Socket creation error \n"); 
		return -1; 
	} 

	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(port); 
	signal(SIGINT,handle_sig);
	signal(SIGUSR1,handle_sig);

	// Convert IPv4 and IPv6 addresses from text to binary form 
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

    pthread_create(&ptid, NULL, &recv_read, &sock); 
	while(1)
	{
		fgets(msg.message,1024,stdin);
		write(sock , &msg , sizeof(msg)); 
		if(mode==PROD)
		printf("Me:%s",msg.message);
	}
	return 0; 
} 
