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

#include <linux/videodev2.h>

#include <video_utils.h>
#include <display_screen.h>
//#include <signalling_utils.h>

pthread_t thread_stream;
void main()
{
    int fd = capture_open("/dev/video0");
    if (fd == -1) {
        exit(-1); }
    init_device(fd);
    pthread_create(&thread_stream, NULL, SDL_STREAM,(void *)(&fd));
    int quit = 0;
    SDL_Event e;
    while (!quit) {
        while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) { // click close icon then quit
            quit = 1;
        }
        if (e.type == SDL_KEYDOWN) {
            if (e.key.keysym.sym == SDLK_ESCAPE) // press ESC the quit
            quit = 1;
        }
        }
        usleep(25);
    }

    thread_exit_sig = 1;               // exit thread_stream
    pthread_join(thread_stream, NULL); // wait for thread_stream exiting

    close_all(fd);

}
