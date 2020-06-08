/*
Documentation
*/
// Server side C/C++ program to demonstrate Socket programming 
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <pthread.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <signal.h>

#include<msg.h>



#define PORT 8080 

#define MAX_GROUPS 3
#define MAX_USER_PERGROUP 100
#define MAX_RECEIVER_THREAD 300



pthread_t recv_pthread[MAX_RECEIVER_THREAD];
pthread_t send_pthread[MAX_USER_PERGROUP];

pthread_mutex_t connection_init_lock 	= PTHREAD_MUTEX_INITIALIZER; 
// CONNECTION Initialization Lock: only enabled during new incoming connection

pthread_mutex_t message_locking 		= PTHREAD_MUTEX_INITIALIZER;
//Message Locking: Enabled when messages are being received or sent

pthread_cond_t  QEMPTYWAIT				= PTHREAD_COND_INITIALIZER;
//Conditional Wait: When Queue is empty these wait 

//-----------------------------------------------------------------------
struct message_queue *Message_Q=NULL;

void push_to_queue(struct group_message incoming_message)
{
    struct message_queue *new_msg=malloc(sizeof(struct message_queue));
    
    new_msg->message_q_ll   = incoming_message;
    //printf("incoming:%s\n",incoming_message.message);
    new_msg->next           = Message_Q;

    Message_Q                 =new_msg;

}

struct group_message pop_from_queue()
{   
    //printf("controlJumpHere\n");
    
    struct message_queue *pop_msg   =Message_Q;
    
    Message_Q                       =Message_Q->next;

    struct group_message pop_ret    =pop_msg->message_q_ll;
    //printf("outgoing:%s\n",pop_ret.message);
    free(pop_msg);

    return pop_ret;

}

int is_queue_empty()
{
    return Message_Q==NULL;
}

void delete_queue(){

    while(!is_queue_empty())
    {
        pop_from_queue();
    }
}
//-----------------------------------------------------------------------

int connection_socket[MAX_GROUPS][MAX_USER_PERGROUP];
int connection_count[MAX_GROUPS];
int client_count=0;
int server_fd;

int msg_id_counter;

char group_name[MAX_GROUPS][gname_size];
int group_init_count	=0;

void handle_sig()
{
	printf("Do you want to close the server program?(Y/n):");
	char close_in[10]="y";
	if(Message_Q==NULL)
		printf("Server is empty");
	else
	{
		while(!is_queue_empty)
		{
			printf("%s:%s\n",Message_Q->message_q_ll.user_name,Message_Q->message_q_ll.message);
		}
	}
	
	if(close_in[0]=='y'||close_in[0]=='Y')
	{
		for(int i=0;i<3;i++)
			pthread_kill(send_pthread[i],SIGKILL);
		for(int i=0;i<client_count-1;i++)
			pthread_kill(recv_pthread[i],SIGKILL);
		
		for(int i=0;i<group_init_count;i++)
			for(int j=0;j<connection_count[i];j++)
			close(connection_socket[i][j]);
		close(server_fd);
		delete_queue();
		exit(0);
	}
}



void* recv_message(void *nsock)
{
	int new_socket = *(int *)nsock;
	pthread_mutex_lock(&connection_init_lock);
	
	struct group_chat_request new_request;
	read(new_socket,&new_request,sizeof(new_request));
	int group_identifier;
	int user_identifier;

	int flag=0;
	struct group_chat_acknowledge new_ack;
	
	for(int i=0;i<group_init_count;i++)
		if(!strcmp(new_request.group_name,group_name[i]))
		{
			group_identifier =i;
			user_identifier=connection_count[i];
			connection_socket[i][connection_count[i]++]=new_socket;
			flag=1;
			break;
		}

	if(!flag)
	{
		strcpy(group_name[group_init_count],new_request.group_name);
		group_identifier=group_init_count;
		user_identifier=connection_count[group_identifier];
		connection_socket[group_identifier][connection_count[group_identifier]++]	=new_socket;
		group_init_count++;
	}

	new_ack.group_identifier	=group_identifier;
	new_ack.user_identifier		=user_identifier;
	write(new_socket,&new_ack,sizeof(new_ack));
	pthread_mutex_unlock(&connection_init_lock);

	struct group_message new_message;

	while(read(new_socket ,&new_message,sizeof(new_message)))
	{
			pthread_mutex_lock(&message_locking);
			new_message.msgid=msg_id_counter++;
			push_to_queue(new_message);
			pthread_mutex_unlock(&message_locking);
			pthread_cond_signal(&QEMPTYWAIT);
	}
	free(nsock);
}

void* send_message(void *nsock)
{	printf("New Send_message_thread\n");

	struct group_message message_to_send;
	int group_id;
	int user_id;


	while(1)
	{
		pthread_mutex_lock(&message_locking);	
		while(is_queue_empty(Message_Q))
		{
			//printf("Waiting...\n");
			pthread_cond_wait(&QEMPTYWAIT,&message_locking);
		
		}
		message_to_send=pop_from_queue();
		group_id=message_to_send.group_identifier;
		user_id=message_to_send.user_identifier;
		pthread_mutex_unlock(&message_locking);
		for(int i=0;i<connection_count[group_id];i++)
			if(~connection_socket[group_id][i] && i!=user_id)
				write(connection_socket[group_id][i],&message_to_send,sizeof(message_to_send));
	}
}

int main(int argc, char const *argv[]) 
{ 
	if(argc!=2)
	{
		printf("Wrong usage:%s <PortNumber>",argv[0]);
		exit(EXIT_FAILURE);
	}
	printf("server created\n");
	int valread; 
	struct sockaddr_in address; 
	int opt = 1; 
	int addrlen = sizeof(address); 
	
	msg_id_counter=0;
	// Creating socket file descriptor 
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
	{ 
		perror("socket failed"); 
		exit(EXIT_FAILURE); 
	} 
	
	// Forcefully attaching socket to the port 8080 
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT ,&opt, sizeof(opt))) 
	{ 
		perror("setsockopt"); 
		exit(EXIT_FAILURE); 
	} 
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY; 
	address.sin_port = htons( atoi(argv[1]) ); 
	signal(SIGINT, handle_sig);
	// Forcefully attaching socket to the port 8080 
	if (bind(server_fd, (struct sockaddr *)&address, 
								sizeof(address))<0) 
	{ 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	} 
	for(int i=0;i<MAX_GROUPS;i++)
		{
			for(int j=0;j<MAX_USER_PERGROUP;j++)
				connection_socket[i][j]=-1;
			connection_count[i]=0;
		}
	printf("executing...\n");
	for(int i=0;i<MAX_GROUPS;i++)
    pthread_create(&send_pthread[i], NULL, &send_message, NULL); 
	if (listen(server_fd, 10) < 0) 
	{ 
		perror("listen"); 
		exit(EXIT_FAILURE); 
	} 
	

	while (1)
	{
		int *new_socket=malloc(sizeof(int));
		*new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen); 
	
		if(pthread_create(&recv_pthread[client_count++],NULL,
                           recv_message, (void *)new_socket) < 0)
						   {
							    perror("pthread_create()");
            					exit(0);
						   }
	
	}

	pthread_exit(NULL);
	return 0; 
} 
