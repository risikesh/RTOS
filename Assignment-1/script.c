
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>

void main()

{
  char *output_filename="test.csv";
  char *argscon[]={"cat","log/*.txt",">",(char *)output_filename,NULL};
  execlp(".","cat","log/*.txt",">",(char *)output_filename,NULL);
  
}
