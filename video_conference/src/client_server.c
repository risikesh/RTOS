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
#include <pulse/simple.h>
#include <pulse/error.h>
#include <netdb.h> 
#include <utils.h>
#include <display_screen.h>
#include <video_utils.h>
#include <signalling_utils.h>

int sig_sock;
int audio_sock;
int video_sock;

int main(int argc, char const *argv[]) // $ ./client <server ip-address> <port>
{ 
	thread_exit_sig_SDL_STREAM=0;
    thread_exit_sig_STREAM_video=0;
    thread_exit_audio_recv=0;
    thread_exit_audio_send=0;
    
    if (argc < 5)
	{
		printf("Usage: %s <server ip-address> <port> <username> <Device name>",argv[0]);exit(EXIT_FAILURE); }
	
	struct sockaddr_in serv_addr; 
	
	if ((sig_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{ printf("\n Socket creation error \n"); return -1; } 
	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(atoi(argv[2])); 
	// Convert IPv4 and IPv6 addresses from text to binary form 
    char ip[20];
    struct hostent *host_get;
    host_get=gethostbyname(argv[1]);
    strcpy(ip,inet_ntoa(*((struct in_addr*) host_get->h_addr_list[0])));
	if(inet_pton(AF_INET, ip , &serv_addr.sin_addr)<=0) 
	{ printf("\nInvalid address/ Address not supported \n"); 	return -1; 	} 
	if (connect(sig_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
	{ printf("\nConnection Failed:X1 \n"); return -1; } 
    struct signalling vc_sig;
    strcpy(vc_sig.username,argv[3]);
    vc_sig.sig=INITIAL_SIGNAL;
    write(sig_sock,&vc_sig,sizeof(vc_sig));
    struct signalling vc_sig_recv;
    
    int sig_sock_audio;
    struct sockaddr_in serv_addr_1;
    int sig_sock_video;
    struct sockaddr_in serv_addr_2;
    usleep(1000);
    int flag=0;
    while(1)
    {
        if(read(sig_sock,&vc_sig_recv,sizeof(vc_sig_recv)))
        {
            if(vc_sig_recv.sig==WAIT_FOR_PARTNER & !flag){
                printf("waiting for partner\n");
                flag=1;        
            }
            else if(vc_sig_recv.sig==PARTNER_CONNECTED){
                printf("partner connected\n");
                break;
            }
             
        }

    }
    
    //Create threads AUDIO/VIDEO (4 threads)

     
	
	if ((sig_sock_audio = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{ printf("\n Socket creation error \n"); return -1; } 
	serv_addr_1.sin_family = AF_INET; 
	serv_addr_1.sin_port = htons(vc_sig_recv.audio_port); 
	// Convert IPv4 and IPv6 addresses from text to binary form 
	if(inet_pton(AF_INET, ip , &serv_addr_1.sin_addr)<=0) 
	{ printf("\nInvalid address/ Address not supported \n"); 	return -1; 	} 
	if (connect(sig_sock_audio, (struct sockaddr *)&serv_addr_1, sizeof(serv_addr_1)) < 0) 
	{ printf("\nConnection Failed:X2 \n"); return -1; } 

    usleep(100000);
	
	if ((sig_sock_video = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{ printf("\n Socket creation error \n"); return -1; } 
	serv_addr_2.sin_family = AF_INET; 
	serv_addr_2.sin_port = htons(vc_sig_recv.video_port); 
	// Convert IPv4 and IPv6 addresses from text to binary form 
	if(inet_pton(AF_INET, ip , &serv_addr_2.sin_addr)<=0) 
	{ printf("\nInvalid address/ Address not supported \n"); 	return -1; 	} 
	if (connect(sig_sock_video, (struct sockaddr *)&serv_addr_2, sizeof(serv_addr_2)) < 0) 
	{ printf("\nConnection Failed :X3 \n"); return -1; } 

    usleep(1000000);
	
    pthread_t audio_send,audio_recv,video_stream,video_SDL;
    struct pulseaudio_con socket_data;
	strcpy(socket_data.name,argv[3]);
	socket_data.socket=sig_sock_audio;

    //creating video port and sdl
    int capture_fd = capture_open(argv[4]);
    if (capture_fd == -1) {
        exit(-1); }
    init_device(capture_fd);
    struct fd_connect send;
    send.fd_con=sig_sock_video;
    send.fd_stream=capture_fd;
    
    pthread_create(&video_stream, NULL,STREAM_video,(void *)(&send));
    pthread_create(&video_SDL, NULL, SDL_STREAM,(void *)(&sig_sock_video));
    pthread_create(&audio_recv, NULL, voice_receive, &socket_data); 
    pthread_create(&audio_send, NULL, voice_send, &socket_data); 



    
    int quit = 0,end_poll = 1;
    SDL_Event e;

    while (!quit) {
        while (SDL_PollEvent(&e) && end_poll) {
        if (e.type == SDL_QUIT) { // click close icon then quit
            vc_sig_recv.sig=SEND_END_CALL;
            write(sig_sock,&vc_sig_recv,sizeof(vc_sig_recv));
            end_poll = 0;  
            }
        }
        if(!read(sig_sock,&vc_sig_recv,sizeof(vc_sig_recv)))
        {
            if(vc_sig_recv.sig==END_CALL){
                printf("ENDCALL\n");
                quit = 1;   
            }
        }

        usleep(25);
    }
    
    thread_exit_sig_SDL_STREAM=1;
    thread_exit_sig_STREAM_video=1;
    thread_exit_audio_recv=1;
    thread_exit_audio_send=1;
    
    close_all(capture_fd);
    close_disp_screen();
    close(vc_sig_recv.audio_port);
    close(vc_sig_recv.video_port);
    close(sig_sock);
    pthread_join(audio_send,NULL);
    pthread_join(audio_recv,NULL);
    pthread_join(video_stream,NULL);
    pthread_join(video_SDL,NULL);

    printf("Exit Sucessfull");
}
