//ORIGINAL DONT REPLACE


#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>

enum status { on, off};
typedef enum status status;

enum boolean { false, true };
typedef enum boolean boolean;

unsigned  short i = 0;
int pid;
status flag = on;

struct sigaction newhandler, oldhandler;
sigset_t sig;

void setblock( int fd ,boolean block)
{
   static int blockf, nonblockf;
   static boolean first = true;

   int flags;

   if (first) {
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

void next( int code)
{
  static status flag = on;

  if (flag == on) {
      kill(pid,SIGSTOP);
      flag = off;
  } else {
     kill(pid,SIGCONT);
     flag = on;
 }
}

void clrscr()
{
   printf("\033[23");
   fflush(stdout);
}

void writexy(int row, int col, char *s)
{
   printf("\033[%d;%dH%s",row,col,s);
   fflush(stdout);
}

void gotoxy(int row,int col)
{
   printf("\033[%d;%dH",row,col);
   fflush(stdout);
}

void main()
    {
    char mess[80];
    int fd;
    int numch;

    pid = fork();
    if ( pid == 0 )
        {
          while (1) 
             { 
              i++;
              if (i % 10000 == 0 ) putc('.',stderr); //child
             }
        }
    else { 

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
        while (1) {
          alarm(3);
          pause();
          switch  ( numch = read(fd,mess,80))  {
          case -1 :
          case 0 :
              break;
          default: 
	      fprintf(stderr," %d   <%s>\n",numch,mess);
          }
          fprintf(stderr," in parent\n");
        }
     }
}
