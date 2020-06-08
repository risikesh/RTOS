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

#include <display_screen.h>

void handle()
{

    thread_exit_sig_SDL_STREAM = 1;               // exit thread_stream
}


void main(int argc, char*argv[])
{
    signal(SIGINT,handle);
  thread_exit_sig_SDL_STREAM = 0;
  int server_desc, client_desc, val;
  size_t  size = IMAGE_HEIGHT*IMAGE_WIDTH*2;
  struct sockaddr_in server,client;
  int opt = 1;
  int addrlen = sizeof(client);
  char buf[size];
        ssize_t r;
  void * buff;
  server_desc = socket(AF_INET,SOCK_STREAM,0);
    if (setsockopt(server_desc, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
  {
    perror("setsockopt");
    exit(EXIT_FAILURE);
  }

  server.sin_family=AF_INET;
  server.sin_addr.s_addr=INADDR_ANY;
  server.sin_port=htons(8000);
  memset(server.sin_zero, '\0', sizeof server.sin_zero);

  int bind_check = bind(server_desc, (struct sockaddr *)&server, sizeof(server));
  printf("Bind check = %d\n",bind_check );
  listen(server_desc, 5);
  client_desc = accept(server_desc, (struct sockaddr *)&client, (socklen_t*)&addrlen);
    pthread_t thread_stream; 
    pthread_create(&thread_stream, NULL,SDL_STREAM,(void *)(&client_desc));
        
        pthread_join(thread_stream, NULL); // wait for thread_stream exiting
        close_disp_screen();
        close(client_desc);
        

}