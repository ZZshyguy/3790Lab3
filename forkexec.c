/*
Parent has to store PIDs of all forked processes
*/

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

enum status { on, off};
typedef enum status status;


unsigned  i = 0;
int pid[10];			//Array holding PID's of children
status flag = on;
char program[256] = {0};	//Buffer for program name
char process[][10] = {"a","b"};	//Array holding child process names
char path[256];			//Array holding path name
int numProcess = 2;		//Number of processes running
int current = 0;		//Current process running

struct sigaction newhandler, oldhandler;
sigset_t sig;

void next( int code) //signal handler
{
    kill(pid[current],SIGSTOP);
    current++;
    if(current == numProcess) current = 0;
    kill(pid[current],SIGCONT);
}

void schedule()			//Function that starts child processes
{
   int i;
   for(i=0; i<numProcess; i++)  //For each process
   {
   pid[i] = fork();		//Fork off a child
   if ( pid[i] == 0 )		//If child, start a different program
        {
	  strcpy(path,"./");
	  strcat(path,process[i]);
          execl(path,program,NULL); //READ MANUAL
        }
   }
}

void main()
{
        schedule();
        
	sigemptyset(&sig);  /* clear the signal mask */
        newhandler.sa_handler = next;
        newhandler.sa_mask = sig;
        newhandler.sa_flags = 0;

        if ( sigaction(SIGALRM,&newhandler,&oldhandler) == -1 )
          {
            printf("1 - can't install signal handler \n");
            exit(-1);
          }
        
	while (1) {
          alarm(3); 		//alarm will send signal after 3 seconds
          pause(); 		//do nothing until alarm's off
        }
}




