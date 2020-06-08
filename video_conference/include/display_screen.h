#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <inttypes.h>
#include <pthread.h>
#include <vc_config.h>
#include <utils.h>
#include <SDL2/SDL.h>
SDL_Window *sdlScreen;
SDL_Renderer *sdlRenderer;
SDL_Texture *sdlTexture;
SDL_Rect sdlRect;


#define BUFFSIZE IMAGE_HEIGHT*IMAGE_WIDTH*2
int thread_exit_sig_SDL_STREAM=0;


static void* SDL_STREAM(void *arg)//connection FD as argument
{


  // SDL2 begins
    memset(&sdlRect, 0, sizeof(sdlRect));
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER)) {
        printf("Could not initialize SDL - %s\n", SDL_GetError()); return NULL;}

    sdlScreen = SDL_CreateWindow("Simple YUV Window", SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED, IMAGE_WIDTH,IMAGE_HEIGHT, SDL_WINDOW_SHOWN);

    if (!sdlScreen) {
        fprintf(stderr, "SDL: could not create window - exiting:%s\n",SDL_GetError());return NULL;}

    sdlRenderer = SDL_CreateRenderer(sdlScreen, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (sdlRenderer == NULL) {
        fprintf(stderr, "SDL_CreateRenderer Error\n");return NULL;}
    sdlTexture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_YUY2, SDL_TEXTUREACCESS_STREAMING, IMAGE_WIDTH, IMAGE_HEIGHT);
    sdlRect.w = IMAGE_WIDTH;
    sdlRect.h = IMAGE_HEIGHT;

    int fd = *((int *) ( arg));
    //void (*handler)(void *pframe, int length) = ((struct streamHandler *)(arg))->framehandler;
    char buffer[BUFFSIZE];
    while (!thread_exit_sig_SDL_STREAM) {
        // read mem_buffer from fd
        loop_read(fd,buffer,sizeof(buffer));
        SDL_UpdateTexture(sdlTexture, &sdlRect,(void *) buffer, IMAGE_WIDTH *2 );
        SDL_RenderClear(sdlRenderer);
        SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, &sdlRect);
        SDL_RenderPresent(sdlRenderer);

        }
    
    return NULL;

}

void close_disp_screen()
{
    SDL_Quit();
}