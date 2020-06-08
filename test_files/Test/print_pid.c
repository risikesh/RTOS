#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>

int main()
{
printf("launched program execvp:%d\n",getpid());
execp("../../Assignment-1/build/server","../../Assignment-1/build/server",NULL);

return 0;
}