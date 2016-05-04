#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main (int argc, char *argv[]) 
{
   pid_t childpid = 0; 
   int i, n;
 
   if (argc != 2)
   {   /* check for valid number of command-line arguments */ 
      fprintf(stderr, "Usage: %s processes\n", argv[0]);
      return 1; 
   }     
   
   n = atoi(argv[1]);  // atoi converts char to int
   for (i = 1; i < n; i++)
      if ( (childpid = fork() ) ) // returns -1 on error 
                                 // 0 success and forks and you are child process (converts to false); pos value you are parent getting pid 
                                 // "ifs" convert to bool 
         break;                  // if forks, parent breaks; if not, child loops

   sleep(10);                    // each process blocked for 10 seconds before printing
   fprintf(stderr, "i:%d  process ID:%ld  parent ID:%ld  child ID:%ld\n",
           i, (long)getpid(), (long)getppid(), (long)childpid);
   return 0; 
}

/*
int main()
{
   int x = 5;
   int y = fork();
   if ( y == 0) // child
      --x;
   if ( y > 0) // parent
      ++x;
}*/