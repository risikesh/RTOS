#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
fd_set rfds;
struct timeval tv;
int retval;
int input_read_timer(char *out,long long int wait_time)
{
    tv.tv_sec = 0;
    tv.tv_usec = wait_time;
    FD_ZERO(&rfds);
    FD_SET(0, &rfds);
    retval = select(1, &rfds, NULL, NULL, &tv);
    if (retval == -1)
        perror("select()");
    else if (retval)
    {
        printf("Data is available now.\n");
        /* FD_ISSET(0, &rfds) will be true. */
        int size;
        size=read(0,out,1000);
        out[size]='\0';
        return 0;
    }
    else
        return 1; //printf("No data within five seconds.\n");
    }

int main(void) {
    char out[1000];
    while(1)
    {
        if(!input_read_timer(out,10))
        {
            printf("%s",out);
            break;
        }
    }
    return 0;
}