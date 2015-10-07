#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

enum status { on, off};
typedef enum status status;

enum boolean { false, true };
typedef enum boolean boolean;

unsigned  short i = 0;
int pid[10];			//Array holding PID's of children
status flag = on;
char program[256] = {0};	//Buffer for program name
char process[][10] = {"a","b"};	//Array holding child process names
char path[256];			//Array holding path name
int numProcess = 2;		//Number of processes running
int current = 0;		//Current process running

struct sigaction newhandler, oldhandler;
sigset_t sig;

void setblock( int fd ,boolean block)
{
   static int blockf, nonblockf;
   static boolean first = true;

   int flags;

   if (first) 
   {
      first = false;
      if (( flags = fcntl(fd,F_GETFL,0)) == -1)
      {
         fprintf(stderr," fcntl - error \n");
         exit(1);
      }
     blockf = flags & ~ O_NDELAY;
     nonblockf = flags | O_NDELAY;
  }
  if ( fcntl(fd,F_SETFL, block ? blockf : nonblockf) == -1 )
      {
         fprintf(stderr," fcntl2 - error \n");
         exit(1);
      }
}

/*void next( int code)	//rapidly alternates between a's and b's
{
  static status flag = on;

  if (flag == on) {
      kill(pid,SIGSTOP);
      flag = off;
  } else {
     kill(pid,SIGCONT);
     flag = on;
 }
}*/

void next( int code)	//only a's or b's
{

    kill(pid[current],SIGSTOP);
    current++;
    if(current == numProcess) current = 0;
    kill(pid[current],SIGCONT);

}

void clrscr()		//Clears the screen
{
   printf("\033[23");
   fflush(stdout);
}

void writexy(int row, int col, char *s)	//Writes a string at row, col
{
   printf("\033[%d;%dH%s",row,col,s);
   fflush(stdout);
}

void gotoxy(int row,int col)		//Puts cursor at row,col
{
   printf("\033[%d;%dH",row,col);
   fflush(stdout);
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
    char mess[80];
    int fd;
    int numch;

    schedule();

    /*pid = fork();	//not needed
    if ( pid == 0 )
        {
          while (1) 
             { 
              i++;
              if (i % 10000 == 0 ) putc('.',stderr); //child
             }
        }
    else { */

        fd = open("/dev/tty",O_RDWR);
        setblock(fd,false);

        sigemptyset(&sig);  /* clear the signal mask */
        newhandler.sa_handler = next;
        newhandler.sa_mask = sig;
        newhandler.sa_flags = 0;

        if ( sigaction(SIGALRM,&newhandler,&oldhandler) == -1 )
          {
            printf("1 - can't install signal handler \n");
            exit(-1);
          }

        while (1) 
	{
          alarm(3);
          pause();
          switch  ( numch = read(fd,mess,80))  
	  {
          case -1 :
          case 0 :
              break;
          default: 
	      fprintf(stderr," %d   <%s>\n",numch,mess);
          }
          fprintf(stderr," in parent\n");
        }
     
}
