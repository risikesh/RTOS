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
#define PORT 8080 

pthread_t ptid;

pthread_t recv_pthread[300];
pthread_t send_pthread[3];

pthread_mutex_t newConnectionLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t messageLock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t c_cons = PTHREAD_COND_INITIALIZER;
pthread_cond_t c_prod = PTHREAD_COND_INITIALIZER;


int connections[3][100];
int connection_count[3];
int client_count=0;
int server_fd;


//-------------------------------------------------------------------------------

struct messageQ{
	char messg[1000];
	struct messageQ *next;
}*mQ;

void add_Q(char *message_incoming)
{
	struct messageQ *new_q=malloc(sizeof(struct messageQ));
	new_q->next=mQ;
	strcpy(new_q->messg,message_incoming);
	mQ=new_q;
}

void pop_Q()
{
	struct messageQ *del_q=mQ;
	printf("Poped from queue: %s",mQ->messg);
	mQ=mQ->next;
	free(del_q);
	
}

int isQ_empty()
{
	return mQ==NULL;
}

void printQ()
{
	struct messageQ	*qlink=mQ;
	while(qlink!=NULL)
	{
		printf("%s-->",qlink->messg);
		qlink=qlink->next;
	}
	printf("Null\n");
}
//-------------------------------------------------------------------------------

void handle_sig()
{
	for(int i=0;i<3;i++)
		pthread_kill(send_pthread[i],SIGKILL);
	for(int i=0;i<client_count-1;i++)
		pthread_kill(recv_pthread[i],SIGKILL);
	for(int i=0;i<connection_count[0];i++)
		close(connections[0][i]);
	close(server_fd);
	exit(0);
}



void* recv_message(void *nsock)
{
	printf("New Incomming recv_message\n");

	int new_socket = *(int *)nsock;
	pthread_mutex_lock(&newConnectionLock);
	connections[0][connection_count[0]]=new_socket;
	connection_count[0]++;
	printf("new_socket= %d \n",new_socket);
	pthread_mutex_unlock(&newConnectionLock);

	char buffer[1024];
	while(read(new_socket , buffer, 1024))
	{
			pthread_mutex_lock(&messageLock);

			add_Q(buffer);
			printQ();
			pthread_mutex_unlock(&messageLock);
			pthread_cond_signal(&c_cons);
			printf("%s\n",buffer );
	}
	free(nsock);
}

void* send_message(void *nsock)
{

printf("New Send_message thread\n");

char buffer[1000];

while(1)
{
	pthread_mutex_lock(&messageLock);	
	while(isQ_empty())
		pthread_cond_wait(&c_cons,&messageLock);
	strcpy(buffer,mQ->messg);
	pop_Q();
	pthread_mutex_unlock(&messageLock);
	for(int i=0;i<connection_count[0];i++)
		if(~connections[0][i])
			write(connections[0][i],&buffer,sizeof(buffer));

	
}
}

int main(int argc, char const *argv[]) 
{ 
	int valread; 
	struct sockaddr_in address; 
	int opt = 1; 
	int addrlen = sizeof(address); 
	char buffer[1024] = {0}; 
	char hello[1024]; 
	
	// Creating socket file descriptor 
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
	{ 
		perror("socket failed"); 
		exit(EXIT_FAILURE); 
	} 
	
	// Forcefully attaching socket to the port 8080 
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
												&opt, sizeof(opt))) 
	{ 
		perror("setsockopt"); 
		exit(EXIT_FAILURE); 
	} 
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY; 
	address.sin_port = htons( PORT ); 
	signal(SIGINT, handle_sig);
	// Forcefully attaching socket to the port 8080 
	if (bind(server_fd, (struct sockaddr *)&address, 
								sizeof(address))<0) 
	{ 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	} 
	for(int i=0;i<3;i++)
		{
			for(int j=0;j<100;j++)
				connections[i][j]=-1;
			connection_count[i]=0;
		}
	printf("executing...\n");
	for(int i=0;i<3;i++)
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
