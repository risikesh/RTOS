/**
* File: subprocess.h
*
**/

#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>

typedef struct {
  pid_t pid;
} subprocess_t;
subprocess_t clients[3][100];
int total_groups_;
int clients_per_group_;
char server_location[20]="./build/server";
char client_location[20]="./build/client";
char bind_ip[20]="127.0.0.1";
char port[10]="8080";

void *parallel_send_message(void *args){
  int  t_num= *((int *)args); 
  for(int i=0;i<total_groups_;i++)
    for(int j=0;j<clients_per_group_;j++)
    {
      send_test_message(clients[(i+t_num)%total_groups_][(j+t_num)%clients_per_group_]);
      usleep(50*1000);
    }
  return NULL;
}

subprocess_t create_new_client(char *groupname,char *username,char *mode) {
  
  subprocess_t process = { fork() };
  
  if (process.pid == 0) {
      execlp(client_location,client_location,bind_ip, port, username ,groupname, mode,NULL);
  }

  return process;
}

subprocess_t concatenate_results(char *filename) {
  
  subprocess_t process = { fork() };
  
  if (process.pid == 0) {
      execlp(,NULL);
  }

  return process;

}

subprocess_t create_new_server() {
  
  subprocess_t process = { fork() };
  
  if (process.pid == 0) {
      execlp(server_location,server_location,NULL);
  }

  return process;
}

subprocess_t create_new_server(char *groupname,char *username,char *mode) {
  
  subprocess_t process = { fork() };
  
  if (process.pid == 0) {
      execlp(server_location,server_location,NULL);
  }

  return process;
}

void send_test_message(subprocess_t process)
{
    kill(process.pid,SIGUSR1);
}

void kill_cs(subprocess_t process)
{
    kill(process.pid,SIGINT);
}

void testing(int clients_per_group,int total_groups,int clients_parallel,char *output_filename)
{
  
  char user_name[10]  ="User";
  char group_name[10] ="Grp";
  
  //cleaning log files
  char *args[]={"rm","log/*.txt",NULL};
  execv(args[0],args);

  total_groups_=total_groups;
  clients_per_group_=clients_per_group;
  subprocess_t server=create_new_server();

  //Create clients
  for(int i=0;i<total_groups;i++)
  { 
    user_name[4]='\0';
    strcat(group_name,itoa(i));
    for(int j=0;j<clients_per_group;j++)
      {
        group_name[3]='\0';
        strcat(user_name,itoa(j));
        clients[i][j]=create_new_client(group_name,user_name,"DEV");
      }
  }

  //send parallel messages
  pthread_t par_t[clients_parallel];
  for(int i=0;i<clients_parallel;i++)
    pthread_create(&par_t[i],NULL,parallel_send_message,(void *)&i);
  for(int i=0;i<clients_parallel;i++)
    pthread_join(par_t[i],NULL);
  //kill all the clients
  for(int i=0;i<total_groups;i++)
    for(int j=0;j<clients_per_group;j++)
    {
      kill_cs(clients[i][j]);
      usleep(50*1000);
    }
  
  //Kill server
      kill_cs(server);
  //concatenate data
  char *argscon[]={"cp","log/*.txt",">",(char *)output_filename,NULL};
  execv(args[0],args);


}

int main(int argc, char *argv[]) {


  return 0;
}