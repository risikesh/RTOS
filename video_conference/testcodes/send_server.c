#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>          
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/time.h>
#include <getopt.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h> 
#include <signal.h>
#include <linux/videodev2.h>

#include <video_utils.h>
void handle()
{

    thread_exit_sig_STREAM_video = 1;               // exit thread_stream
}

void main()
{
        signal(SIGINT,handle);
        thread_exit_sig_STREAM_video = 0;
        int len_c, n, check;
        struct sockaddr_in server;
        //uint8_t buf[BUFSIZE];
        int con_check;
        //unsigned char *message;

        int client_fd = socket(AF_INET,SOCK_STREAM,0);
        //printf("client desc = %d\n",client_fd );

        server.sin_family=AF_INET;
        server.sin_addr.s_addr=inet_addr("127.0.0.1");
        server.sin_port=htons(8000);
        memset(server.sin_zero, '\0', sizeof server.sin_zero);
        con_check = connect(client_fd,(struct sockaddr *)&server, sizeof(server));
        
        pthread_t thread_stream;    
        int fd = capture_open("/dev/video0");
        if (fd == -1) {
            exit(-1); }
        init_device(fd);
        struct fd_connect send;
        send.fd_con=client_fd;
        send.fd_stream=fd;
        pthread_create(&thread_stream, NULL,STREAM_video,(void *)(&send));
        
        pthread_join(thread_stream, NULL); // wait for thread_stream exiting
        close_all(fd);
        close(client_fd);
        
}