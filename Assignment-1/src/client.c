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
FILE *fp;
struct group_message msg;
NTPClient *send_client;

void handle_sig(int sign)
{
	switch (sign)
	{
	case SIGINT:
		printf("killing");
		pthread_kill(ptid,SIGINT);
		close(sock);
		if(mode==DEV)
		{
			fclose(fp);
		}
		exit(0);
		break;
	case SIGUSR1:
		
		printf("Sending test message\n");
		strcpy(msg.message,"Test message plsss!!");
		msg.time_stamp=get_time_now(send_client);
		write(sock , &msg , sizeof(msg)); 
		printf("Me:%s",msg.message);
		break;
	}
}

void* recv_read(void *v)
{
	 printf("executing program:%d\n",getpid());
	int valread;
	NTPClient *recv_client =malloc(sizeof(NTPClient));
	NTPClient_init(recv_client);
	double delay=0;
	int *sock = (int *)v;
	struct group_message incoming;
	while(1)
	{
		valread = read( *sock , &incoming, sizeof(incoming)); 
		if(valread)
		{
			printf("%s:%s\n",incoming.user_name,incoming.message);
			delay=get_time_now(recv_client)-incoming.time_stamp;
			if(mode==DEV)
			fprintf(fp,"%d,%f\n",incoming.msgid,delay);
		}
	} 
}

int main(int argc, char const *argv[]) 
{ 
	int valread; 
	struct sockaddr_in serv_addr; 
	char hello[1024]; 
	char buffer[1024];
    if(argc!=6)
    {
        printf("Wrong Arguments!! \nUsage:%s <Server-IP-Addr> <Port> <User-Name> <Group-Name> <Mode>\n",argv[0]);
        exit(EXIT_FAILURE);
    }
	
    
    int port=atoi(argv[2]);
    struct group_chat_request req;
    struct group_chat_acknowledge ack;
    
    strcpy(req.group_name,argv[4]);
    strcpy(req.user_name,argv[3]);

    if(!strcmp(argv[5],"DEV"))
        mode=DEV;
    else
        mode=PROD;
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
    printf("Group_IDENTIFIER:%d\nUser_IDENTIFIER:%d\n",ack.group_identifier,ack.user_identifier);
    
	char filename[100];
	if(mode==DEV)
	{
		sprintf(filename,"./log/log_%d_%d.txt",ack.group_identifier,ack.user_identifier);	
		fp = fopen(filename, "w");
		printf("Filename=%s\n",filename);
		if(fp == NULL)
		{
			printf("Error opening file\n");
			exit(1);
		}
	}
	 printf("executing program:%d\n",getpid());
    pthread_create(&ptid, NULL, &recv_read, &sock); 
	send_client=malloc(sizeof(NTPClient));
	NTPClient_init(send_client);

	while(1)
	{
		fgets(msg.message,1024,stdin);
		msg.time_stamp=get_time_now(send_client);
		write(sock , &msg , sizeof(msg)); 
		printf("Me:%s",msg.message);
	}
	return 0; 
} 
