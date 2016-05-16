#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>  // used for pid and key_t
#include <sys/ipc.h>    // defines flags in shared memory
#include <sys/shm.h>    // usage of shared memory
//#include <argp.h>
#ifndef MAX_CANON
#define MAX_CANON 8192
#endif 

int makeargv(const char *s, const char *delimiters, char ***argvp);

int main (int argc, char *argv[]) 
{
    if (argc != 2)  // check for valid number of command-line arguments 
    {   
      fprintf(stderr, "Usage: %s processes.\nYou must enter an integer with the command.\n", argv[0]);
      return 1; 
    }  

    int pr_limit;
    pr_limit = atoi(argv[1]);  
    int *pr_count;                 // # of concurrent processes (children)
    int ShmID;                     // IDs the shared memory

   ShmID = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666);  // IPC_CREAT creates shared mem for first time with bitwise OR ( see notes )
   
   if (ShmID < 0) 
   {
      printf("*** shmget error (server) ***\n");
      exit(1);
    }
     //printf("Server has received a shared memory of four integers...\n");

     pr_count = (int *) shmat(ShmID, NULL, 0);  // sets/returns the address of pr_count for all processes with this ID (where we are working - returns pointer to an int)
     if ((int) pr_count == -1)           // upon error, returns void pointer, so needs to be cast to int to make comparison to -1
     {
          printf("*** shmat error (server) ***\n");
          exit(1);
     }
    //printf("Server has attached the shared memory...\n");

    *pr_count = 0;  // at the address on line 29, set its value equal to zero (no processes running)

    char cmd[MAX_CANON]; // the string we want to process

    printf ( "Enter next command: " );

    while(fgets(cmd, MAX_CANON, stdin) != NULL || strcmp( cmd, "exit" ) ) 
    {
        fprintf(stderr, "Enter the while.\n");

        if (*pr_count == pr_limit)  // if reached thread limit, wait for other processes to finish 
        {
          printf( "One moment while we wait for a process to finish....");
          wait( NULL );       // test to see if working
          printf( "Executing %s ", cmd);
          *pr_count--;
        }
        
        fprintf(stderr, "About to fork ... ");
        pid_t childpid; 
        childpid = fork(); // "assign value of fork to childpid"

        fprintf(stderr, "Forked.\n");

        switch ( childpid )
        {
            case -1:
              fprintf(stderr, "Usage: %s processes.\nFork failed.\n", argv[0]);
                break;
            case 0: // child
            {
              printf ( "Child is born.\n" );
              char **argp; // a complex pointer to a 2 dimensional array
                            // string is char array, and will have 3 strings (so array of arrays)
              int numArgs; // the number of arguments
              numArgs = makeargv( cmd, " ", &argp ); // look at the 'cmd' string and parse by [space] and store the return built array of arguments into "argp" then return the size of the array
			        
              if ( execvp( *argp, argp ) < 0)  // execute command at [0] and pass all parameters
			        {
                printf ( "Error executing.");
                exit(1);
              }

              printf( "%s process ID: %d\n", argp[0], childpid ); // test
              waitpid( -1, NULL, WNOHANG );
              
			  // this is a copy of 'pr_count' form the parent; therefore, decrementing here has no effect
              *pr_count--;
              exit( 1 );  // child ends
              break;
            }
            default: // parent forks child process
                printf ( "Proud parent." );
                *pr_count++;
                break;
        }

        printf ( "Enter next command: " );
    }
    
    printf ( "Program terminating." );
    return 0; 
}
