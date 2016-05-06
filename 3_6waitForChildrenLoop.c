#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>  // why??
/*#ifndef MAX_CANON
#define MAX_CANON 8192
#endif*/

int main (int argc, char *argv[]) 
{
  if (argc != 2)  // check for valid number of command-line arguments 
  {   
    fprintf(stderr, "Usage: %s processes.\nYou must enter an integer with the command.\n", argv[0]);
    return 1; 
  }  

  int pr_limit = atoi(argv[1]);  
  int pr_count = 0;
  char cmd[MAX_CANON]; // the string we want to process

  printf ( "Enter next command: " );

  int shm_count = 0;  // shared memory space variable
  pipe( &shm_count );  // piping officially makes it shared

  while(fgets(cmd, MAX_CANON, stdin) != NULL) 
  {
    read( shm_count, &pr_count, sizeof(pr_count) ); // check shm_count to make sure have current version of pr_count

    if (pr_count == pr_limit)  // if reached thread limit, wait for other processes to finish 
    {
      printf( "One moment while we wait for a process to finish....");
      wait( NULL );       // test to see if working
      printf( "Executing %s ", cmd);
      read( shm_count, &pr_count, sizeof(pr_count) ); // check to make it hasn't changed
      pr_count--;
      write( shm_count, &pr_count, sizeof(pr_count) );
    }
    
    pid_t childpid; 
    childpid = fork(); // "assign value of fork to childpid"

    switch ( childpid )
    {
      case -1:
          err_sys( "fork failed" );
          break;
      case 0: // child
        { 
          char **argp; // a complex pointer to a 2 dimensional array
                        // string is char array, and will have 3 strings (so array of arrays)
          int numArgs; // the number of arguments
          numArgs = makeargv( cmd, " ", &argp ); // look at the 'cmd' string and parse by [space] and store the return built array of arguments into "argp" then return the size of the array

          execvp( argp[0], &argp );  // execute command at [0] and pass all commonds
          printf( "%s process ID: %d\n", argp[0], childpid ); // test
          waitpid( -1, NULL, WNOHANG );
          read( shm_count, &pr_count, sizeof(pr_count) ); // check to make it hasn't changed
          pr_count--;
          write( shm_count, &pr_count, sizeof(pr_count) );
          exit( 1 );  // child ends
        }
          break;
      default: // parent forks child process
          read( shm_count, &pr_count, sizeof(pr_count) ); // check to make it hasn't changed
          pr_count++;
          write( shm_count, &pr_count, sizeof(pr_count) );          
          break;
    }

    printf ( "Enter next command: " );

  }

// check for children and wait if necessary
  read( shm_count, &pr_count, sizeof(pr_count) );
  while( pr_count ) // may need to say while pr_count > 0
    {
        waitpid( -1, NULL, WNOHANG );  
        read( shm_count, &pr_count, sizeof(pr_count) );
        printf( "%d", pr_count);
    }

   return 0; 
}

