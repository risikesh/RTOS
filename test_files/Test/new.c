#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>

int main()
{
    printf("executing program:%d\n",getpid());
    char *argv[] = {"/bin/sh", "-c", "./print_pid", NULL};
    execvp(argv[0], argv);
    return 0;
}