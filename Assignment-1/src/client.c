#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h> 

#include "msg.h"
#define PORT 8080 

pthread_t recv_thread;


void messageread_sock(void *recvfd)
{
    int refd=*(int *)recvfd;
    struct grp_message recv_message;

    while(!read( refd , &recv_message, sizeof(recv_message)))
    {
        printf("%s|$s\n",recv_message.user_name,recv_message.message)
    }

}

int main(int argc, char const *argv[]) 
{ 
    char username[uname_size] ="test_grp";
    char grp_name[gname_size] ="test_user";

    int sock = 0, valread; 
	struct sockaddr_in serv_addr; 
	char buffer[1024] = {0}; 
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{ 
		printf("\n Socket creation error \n"); 
		return -1; 
	} 

	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(PORT); 
	
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
//----------------------Till here connecting to server-----------------------
/*
implement the initial setup,
initial serrver time download so that we can get offset of all the clocks of 
different clients from server time.
*/ 
//---------------------------------------------------------------------------
	struct grpchat_request init_req;
    strcpy(init_req.group_name,grp_name);
    strcpy(init_req.user_name,username);

    write(sock,&init_req,sizeof(init_req));//sending the intial request to the server 


    struct grpchat_indentifier grp_identi;
    read(sock, &grp_identi, sizeof(grp_identi));

    if(pthread_create(&recv_thread,NULL,messageread_sock,(void *) sock)<0)
    {
        perror("error in thread creation");
    }

    struct grp_message send_message;
    send_message.grp_identifier=grp_identi.grp_identifier;
    send_message.user_identifier=grp_identi.user_identifier;
    strcpy(send_message.user_name,init_req.user_name);


    while (1)
    {
        fgets(send_message.message,sizeof(send_message.message),stdin);
        write(sock,&send_message,sizeof(send_message));
    }
	return 0; 
} 
