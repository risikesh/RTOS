#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>
#include <stdlib.h>

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


void send_test_message(subprocess_t process)
{
    kill(process.pid,SIGUSR1);
}

void *parallel_send_message(void *args){
  int Max_Number=100;
  int runs=0;
  int  t_num= *((int *)args);
  for(int i=0;i<total_groups_;i++)
  {
for(int j=0;j<clients_per_group_;j++)
    {
      runs+=1;
      send_test_message(clients[(i+t_num)%total_groups_][(j+t_num)%clients_per_group_]);
      usleep(10*1000);
      if(runs==Max_Number)
	      break;

    }
      if(runs==Max_Number)
              break;

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

subprocess_t create_new_server() {
  
  subprocess_t process = { fork() };
  
  if (process.pid == 0) {
      execlp(server_location,server_location,NULL);
  }
  usleep(50*1000);
  return process;
}
void kill_cs(subprocess_t process)
{
    kill(process.pid,SIGINT);
}

void testing(int clients_per_group,int total_groups,int clients_parallel,int mswait,char *filename)
{
  
  char user_name[10]  ="User";
  char group_name[10] ="Grp";
  
  //cleaning log files
  

  total_groups_=total_groups;
  clients_per_group_=clients_per_group;
  subprocess_t server=create_new_server();
  char buffer[10];
  //Create clients
  for(int i=0;i<total_groups;i++)
  { 
    //user_name[4]='\0';
    sprintf(buffer, "%d",i);
    strcat(group_name,buffer);
    for(int j=0;j<clients_per_group;j++)
      {
        sprintf(buffer, "%d",j);
        strcpy(user_name,"User");
        strcat(user_name,buffer);
        printf("client created:%d|uname:%s\n",j + i*clients_per_group+1,user_name);
        clients[i][j]=create_new_client(group_name,user_name,"DEV");
        usleep(10*1000);
      }
    group_name[3]='\0';
  }
  usleep(500*1000);
  //send parallel messages
  pthread_t par_t[10];
  for(int i=0;i<clients_parallel;i++)
    pthread_create(&par_t[i],NULL,parallel_send_message,(void *)&i);
  
  for(int i=0;i<clients_parallel;i++)
    pthread_join(par_t[i],NULL);
  
  usleep(mswait*1000);
  //kill all the clients
  for(int i=0;i<total_groups;i++)
    for(int j=0;j<clients_per_group;j++)
    {
      kill_cs(clients[i][j]);
      usleep(50*1000);
    }
  
  //Kill server
      kill_cs(server);
      usleep(500*1000);
  //concatenate data
  char final_store[100]="cat log_temp/*.txt > log/";
  strcat(final_store,filename);
  system(final_store);
  system("rm log_temp/*.txt");
}

int main(int argc, char *argv[]) {
  testing(atoi(argv[1]),atoi(argv[2]),atoi(argv[3]),atoi(argv[4]),argv[5]);  
  return 0;
}
