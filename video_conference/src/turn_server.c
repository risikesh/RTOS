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
#include <vc_config.h>

#include <signalling_utils.h>

#define audio_port_def 8090
#define video_port_def 8091


pthread_t client_sig_handler;
int client_sig_socket[2];
int thread_exit_FWD=0;
int client_count=0;
struct Fwd_Server
{
    int recv_FD;
    int send_FD;
    size_t buffer_size;

};


int loop_write(int fd,void *data,size_t size)
{
    ssize_t total_byte_sent=0;
    while (size>0){
        ssize_t write_done_cycle;
        if ((write_done_cycle=write(fd,data,size))<0)
            return write_done_cycle;
        if (write_done_cycle == 0)
            break;
        
        total_byte_sent += write_done_cycle;
        data = (uint8_t*) data + write_done_cycle;
        size -= write_done_cycle;
    }

    return total_byte_sent;
}

int loop_read(int fd,void *data,size_t size)
{
    ssize_t total_byte_sent=0;
    while (size>0){
        ssize_t write_done_cycle;
        if ((write_done_cycle=read(fd,data,size))<0)
            return write_done_cycle;
        if (write_done_cycle == 0)
            break;
        
        total_byte_sent += write_done_cycle;
        data = (uint8_t*) data + write_done_cycle;
        size -= write_done_cycle;
    }

    return total_byte_sent;
}
// Video & Audio forward grp
void *ForwardServer(void * Sock)
{
    printf("forwarding server created\n");
    struct Fwd_Server *sock_s=(struct Fwd_Server *) Sock; 
    char *buffer=malloc(sock_s->buffer_size);
    int recv_FD=sock_s->recv_FD;
    int send_FD=sock_s->send_FD;
    size_t buff_size=sock_s->buffer_size;
    
    while(!thread_exit_FWD){
        if(loop_read(recv_FD,buffer,buff_size)>0)
        {
            loop_write(send_FD,buffer,buff_size);
        }

    }
    printf("Ended FWD_servers");
    return NULL;
}

void *handle_client()
{
    printf("client_handler on\n");
    struct signalling clients[2];
    thread_exit_FWD=0;
    //--------------
    int server_fd_audio;
	struct sockaddr_in address_audio; 
	int opt = 1; 
	int addrlen = sizeof(address_audio); 
	if ((server_fd_audio = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
	{   perror("socket failed"); exit(EXIT_FAILURE); } 
	if (setsockopt(server_fd_audio, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT ,&opt, sizeof(opt))) 
	{   perror("setsockopt"); exit(EXIT_FAILURE); } 
	address_audio.sin_family = AF_INET; 
	address_audio.sin_addr.s_addr = INADDR_ANY; 
	address_audio.sin_port = htons( audio_port_def );
	if (bind(server_fd_audio, (struct sockaddr *)&address_audio, sizeof(address_audio))<0) 
	{ 	perror("bind failed"); exit(EXIT_FAILURE); }
    if (listen(server_fd_audio, 10) < 0) 
	{ 
		perror("listen"); 
		exit(EXIT_FAILURE); 
	} 
    //--------------
    int server_fd_video;
	struct sockaddr_in address_video; 
	if ((server_fd_video = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
	{   perror("socket failed"); exit(EXIT_FAILURE); } 
	if (setsockopt(server_fd_video, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT ,&opt, sizeof(opt))) 
	{   perror("setsockopt"); exit(EXIT_FAILURE); } 
	address_video.sin_family = AF_INET; 
	address_video.sin_addr.s_addr = INADDR_ANY; 
	address_video.sin_port = htons( video_port_def );
	if (bind(server_fd_video, (struct sockaddr *)&address_video, sizeof(address_video))<0) 
	{ 	perror("bind failed"); exit(EXIT_FAILURE); }
    if (listen(server_fd_video, 10) < 0) 
	{ 
		perror("listen"); 
		exit(EXIT_FAILURE); 
	} 
    //--------------
    printf("EXIT SERVER INIT\n");
    while(1)
    {
        if(read(client_sig_socket[0],&clients[0],sizeof(clients[0])))
        {   
            if(clients[0].sig==INITIAL_SIGNAL)
            {
                clients[0].sig=WAIT_FOR_PARTNER;
                write(client_sig_socket[0],&clients[0],sizeof(clients[0]));
                printf("INITIAL SIGNAL\n");
            
                break;
            }
        }
    }
    printf("First message sent \n");

    while(1){
        if(client_count==2)
        {
            if(read(client_sig_socket[1],&clients[1],sizeof(clients[1])))
            {
                if(clients[1].sig==INITIAL_SIGNAL)
                {
                    printf("PARTNER_CONNECTED\n");

                    clients[0].sig=PARTNER_CONNECTED;
                    clients[1].sig=PARTNER_CONNECTED;
                    clients[0].audio_port=audio_port_def;
                    clients[1].audio_port=audio_port_def;
                    clients[0].video_port=video_port_def;
                    clients[1].video_port=video_port_def;
                    write(client_sig_socket[0],&clients[0],sizeof(clients[0]));
                    write(client_sig_socket[1],&clients[1],sizeof(clients[1]));
                    
                    break;
                }
            }
        }

    }
    printf("AV_INIT");
    int audio_clients[2],audio_clients_count=0;
    while(1){
        audio_clients[audio_clients_count++]=  accept(server_fd_audio, (struct sockaddr *)&address_audio, (socklen_t*)&addrlen); 
        if(audio_clients_count==2)
            break;

    }

    int video_clients[2],video_clients_count=0;
    while(1){
        video_clients[video_clients_count++]=  accept(server_fd_video, (struct sockaddr *)&address_video, (socklen_t*)&addrlen); 
        if(video_clients_count==2)
            break;

    }
    
    struct Fwd_Server audio_fwd[2],video_fwd[2];
    audio_fwd[0].recv_FD=audio_clients[0];
    audio_fwd[0].send_FD=audio_clients[1];
    audio_fwd[0].buffer_size=1024;

    audio_fwd[1].recv_FD=audio_clients[1];
    audio_fwd[1].send_FD=audio_clients[0];
    audio_fwd[1].buffer_size=1024;

    video_fwd[0].recv_FD=video_clients[0];
    video_fwd[0].send_FD=video_clients[1];
    video_fwd[0].buffer_size=IMAGE_HEIGHT*IMAGE_WIDTH*2;

    video_fwd[1].recv_FD=video_clients[1];
    video_fwd[1].send_FD=video_clients[0];
    video_fwd[1].buffer_size=IMAGE_HEIGHT*IMAGE_WIDTH*2;
    
    pthread_t aud_thread[2],vid_thread[2];

    pthread_create(&aud_thread[0],NULL,ForwardServer,&audio_fwd[0]);
    pthread_create(&aud_thread[1],NULL,ForwardServer,&audio_fwd[1]);
    pthread_create(&vid_thread[0],NULL,ForwardServer,&video_fwd[0]);
    pthread_create(&vid_thread[1],NULL,ForwardServer,&video_fwd[1]);
    //    signals handling recv signals and check if it is send_endcall then end the call
    while(1){

        if(read(client_sig_socket[0],&clients[0],sizeof(clients[0])))
        {
            if(clients[0].sig==SEND_END_CALL)
            {
                clients[0].sig=END_CALL;
                clients[1].sig=END_CALL;
                write(client_sig_socket[0],&clients[0],sizeof(clients[0]));
                write(client_sig_socket[1],&clients[1],sizeof(clients[1]));
            }
        }
        if(read(client_sig_socket[1],&clients[1],sizeof(clients[1])))
        {
            if(clients[1].sig==SEND_END_CALL)
            {
                clients[0].sig=END_CALL;
                clients[1].sig=END_CALL;
                write(client_sig_socket[0],&clients[0],sizeof(clients[0]));
                write(client_sig_socket[1],&clients[1],sizeof(clients[1]));
            }
        }
    }
    pthread_join(aud_thread[0],NULL);
    pthread_join(aud_thread[1],NULL);
    pthread_join(vid_thread[0],NULL);
    pthread_join(vid_thread[1],NULL);


    return NULL;
}



int main(int argc, char const *argv[])
{   
    if(argc!=2){
        printf("USAGE: %s <port number>",argv[0]);
        exit(EXIT_FAILURE);
    }
    int server_fd;
    printf("server created\n");
	int valread; 
	struct sockaddr_in address; 
	int opt = 1; 
	int addrlen = sizeof(address); 
	
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
    printf("PORT NUMBER:%s\n",argv[1]);
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY; 
	address.sin_port = htons( atoi(argv[1]) );
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) 
	{ 	perror("bind failed"); exit(EXIT_FAILURE); }
    int flag_handler_created=0;
    if (listen(server_fd, 10) < 0) 
	{ 
		perror("listen"); 
		exit(EXIT_FAILURE); 
	} 
	client_count=0;
    while (1)
	{
		int *new_socket=malloc(sizeof(int));
		*new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen); 
        client_sig_socket[client_count]=*new_socket;
        client_count++;
        if(!flag_handler_created){
            if(pthread_create(&client_sig_handler,NULL,handle_client,NULL) < 0)
                            {
                                    perror("pthread_create()");
                                    exit(0);
                            }
            flag_handler_created=1;
            printf("Client Count:%d\n",client_count);
        }
        if(client_count==2){
            printf("SECOND_pAr");
            break;
        }
	
	} 
    printf("DONE HANDLING");
    pthread_join(client_sig_handler,NULL);
    
    printf("Server Exit Sucessfull");
}