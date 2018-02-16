/*******************************************************************************
This programming assignment exercises use of a shared memory segment by having
a parent an child process swap two values in shared memory N number of times. 
A semaphore is used to prevent errors in the critical section data.
@author   - Joseph Cutino, Brendon Murthum
@version  - Winter 2018

Observations:


Assignment provided by Prof. Hans Dulimarta.
http://www.cis.gvsu.edu/~dulimarh/CS452/Labs/Lab06-Sem/
*******************************************************************************/

/* Includes */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/sem.h>

/* User Includes */
#include "sem_wrapper.h"

/* Defines */
#define SIZE 16
#define CHAR_BUFFER 256

int main (int argc, char *argv[]) {
	/* Shared Memory Variables */
	int shmId;
    long int i, loop, temp, *shmPtr;
    
	/* Multi Process Variables */
    pid_t pid;
	int status;

	/* Semaphore Variables */
	int semId;
	unsigned short start_values[1] = {0};
	
	/* Create Semaphore */
	semId = sem_create(1,start_values);
    
	/* Retrieve Command Line Argument */
	if(argc != 2){
		// exit if an argument isn't provided
		exit(0);	
	}
	
	//assign loop variable to user provided number 
	loop = atoi(argv[1]);
	
	/* Retrieve shared Memory */
    if ((shmId = shmget (IPC_PRIVATE, SIZE,
                         IPC_CREAT | S_IRUSR | S_IWUSR)) < 0) {
        perror ("i can't get no..\n");
        exit (1);
    }
	
	/* Attach Shared Memory */
    if ((shmPtr = shmat (shmId, 0, 0)) == (void *) -1) {
        perror ("can't attach\n");
        exit (1);
    }
	
	/* Initial Set of Shared Values */
    shmPtr[0] = 0;
    shmPtr[1] = 1;
	
	/* Create Seperate Processes */
    if (!(pid = fork ())) {
		/* Child Process */
		
		//Allow first entrance to Critical Section
		sem_signal(semId,0);
		
		//Swap user provided number of times 
        for (i = 0; i < loop; i++)
		{
			printf ("In Child Loop values: %li\t%li\n", shmPtr[0], shmPtr[1]);
			//Request Access to Critical Section
			sem_wait(semId,0);
			
			/* Start of Critical Section */
			//swap shared values 
            temp = shmPtr[0];
            shmPtr[0]=shmPtr[1];
            shmPtr[1]=temp;
            
			//If values have some how become equal, an error has occurred
            if(shmPtr[0] == shmPtr[1]){
            	printf("Error: %li\n",shmPtr[0]);
            }
			/* End of Critical Section */
			
			//Signal Exit of Critical Section
			sem_signal(semId,0);
        }
		
		/* Detach Shared Memory */
        if (shmdt (shmPtr) < 0) {
            perror ("just can 't let go\n");
            exit (1);
        }
		
		/* Exit Child Process */
        exit (0);
    }
    else {
        for (i = 0; i < loop; i++) {
             printf ("In Parent Loop values: %li\t%li\n", shmPtr[0], shmPtr[1]);
			
			//Request Access to Critical Section
			sem_wait(semId,0);
			
			/* Start of Critical Section */
			//swap shared values 
            temp = shmPtr[0];
            shmPtr[0]=shmPtr[1];
            shmPtr[1]=temp;
            
			//If values have some how become equal, an error has occurred
            if(shmPtr[0] == shmPtr[1]){
            	printf("Error: %li\n",shmPtr[0]);
            }
			/* End of Critical Section */
			
			//Signal Exit of Critical Section
			sem_signal(semId,0);
        }
    }
	
	/* Wait For Child Process Exit */
    wait (&status);
	
	/* Print Final Values */
    printf ("values: %li\t%li\n", shmPtr[0], shmPtr[1]);

	/* Detach Shared Memory */
    if (shmdt (shmPtr) < 0) {
        perror ("just can't let go\n");
        exit (1);
    }
	
	/* Deallocate Shared Memory */
    if (shmctl (shmId, IPC_RMID, 0) < 0) {
        perror ("can't deallocate\n");
        exit (1);
    }
	
	/* Delete Semaphore */
	sem_delete(semId);
	
	/* Exit Program */
    return 0;
}
/* End of Main*/
