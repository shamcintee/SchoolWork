#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
/* #include <limits.h>
#ifndef MAX_CANON
#define MAX_CANON 8192
#endif */

int main (int argc, char *argv[]) 
{
   if (argc != 2)  // check for valid number of command-line arguments 
   {   
      fprintf(stderr, "Usage: %s processes.\nYou must enter an integer with the command.\n", argv[0]);
      return 1; 
   }  

   int pr_limit = atoi(argv[1]);  
   int pr_count = 0;
   char cmd[MAX_CANNON]; // the string we want to process

   printf ( "Enter next command: " );

   while(fgets(cmd, MAX_CANON, stdin) != NULL) 
   {
      if (pr_count == pr_limit)  // if reached thread limit, wait for other processes to finish 
      {
        printf( "One moment while we wait for a process to finish....");
        wait();       // test to see if working
        printf( "Executing %s ", cmd);
        pr_count--;
      }
      
      pid_t childpid; 
      pipe( pr_count );
      childpid = fork(); // "assign value of fork to childpid"

      switch ( childpid )
      {
        case -1:
            err_sys( "fork failed" );
            break;
        case 0: // child
            char **argp; // a complex pointer to a 2 dimensional array
                          // string is char array, and will have 3 strings (so array of arrays)
            int numArgs; // the number of arguments
            numArgs = makeargv( cmd, " ", &argp ); // look at the 'cmd' string and parse by [space] and store the return built array of arguments into "argp" then return the size of the array

            execvp( argp[0], &argp );  // execute command at [0] and pass all commonds
            printf( "%s process ID: %d\n", argp[0], childpid ); // test
            waitpid( -1, NULL, WNOHANG );
            pr_count--;
            exit( 1 );  // child ends
            break;
        default: // parent forks child process
            pr_count++;
            break;
      }

      printf ( "Enter next command: " );
   }

   return 0; 
}


//   scanf( "%d", &pr_limit );
  // fprintf(stderr, "Limit: ", argv[1]);

   /*pid_t childpid = 0; 
   int i, n;
   pr_limit = n;

   for (i = 1; i < n; i++)
      if ((childpid = fork()) <= 0)
         break;
 
   fprintf(stderr, "i:%d  process ID:%ld  parent ID:%ld  child ID:%ld\n",
           i, (long)getpid(), (long)getppid(), (long)childpid);*/

