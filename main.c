/*******************************************************************************
This programming assignment exercises use of a shared memory segment by having
one process that means to communicate to the others by editing the shared memory
and having those listening-processes edit their flags of "have received."

@author   - Joseph Cutino, Brendon Murthum
@version  - Winter 2018

Requirements:
    - Make sure to maximize potential parallelism.
    - Protect the critical sections given in the sample code to prevent memory
      access conflicts from causing inconsistencies in the output.
        - Insert the appropriate code to create and initialize a semaphore.
        - User semaphore operations to synchronize the two processes.
        - Perform required cleanup operations.
Notes:
    - Semaphore creation and initialization are two different, non-atomic
      operations. Be sure they both have completed before another process 
      attempts to access the semaphore.

Assignment provided by Prof. Hans Dulimarta.
http://www.cis.gvsu.edu/~dulimarh/CS452/Labs/Lab06-Sem/
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>


#define SIZE 16

int main (int argc, char *argv[]) {
    int status;
    long int i, loop, temp, *shmPtr;
    int shmId;
    pid_t pid;
    
    // Get the "loop" value from the arguments.
	if(argc != 2)
	    exit(0); 
	loop = atoi(argv[1]);

    if ((shmId = shmget (IPC_PRIVATE, SIZE,
                         IPC_CREAT | S_IRUSR | S_IWUSR)) < 0) {
        perror ("i can't get no..\n");
        exit (1);
    }
    if ((shmPtr = shmat (shmId, 0, 0)) == (void *) -1) {
        perror ("can't attach\n");
        exit (1);
    }

    shmPtr[0] = 0;
    shmPtr[1] = 1;

    if (!(pid = fork ())) {
    /*child*/
        for (i = 0; i < loop; i++) {

            /*
             * TODO: swap the contents of shmPtr[0] and  shmPtr[1]
             */
             temp = shmPtr[0];
             shmPtr[0]=shmPtr[1];
             shmPtr[1]=temp;
             //printf ("In Child Loop values: %li\t%li\n", shmPtr[0], shmPtr[1]);
             if(shmPtr[0] == shmPtr[1]){
             	printf("Error: %li\n",shmPtr[0]);
             }
        }
        if (shmdt (shmPtr) < 0) {
            perror ("just can 't let go\n");
            exit (1);
        }
        exit (0);
    }
    else {
        for (i = 0; i < loop; i++) {

            /*
             * TODO: swap the contents of shmPtr[1] and shmPtr[0]
             */
             temp = shmPtr[0];
             shmPtr[0]=shmPtr[1];
             shmPtr[1]=temp;
             //printf ("In Parent Loop values: %li\t%li\n", shmPtr[0], shmPtr[1]);
        }
    }

    wait (&status);
    printf ("values: %li\t%li\n", shmPtr[0], shmPtr[1]);

    if (shmdt (shmPtr) < 0) {
        perror ("just can't let go\n");
        exit (1);
    }
    if (shmctl (shmId, IPC_RMID, 0) < 0) {
        perror ("can't deallocate\n");
        exit (1);
    }
    return 0;
}
