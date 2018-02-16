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
    int status;
    long int i, loop, temp, *shmPtr;
    int shmId;
    pid_t pid;

	int semId;
	int semId2;
	int id = 'S';	
	char *path = " ";

	/*Get sem key*/
	key_t semkey;
	getcwd(path, CHAR_BUFFER);
	semkey = ftok(path, id);
	
	semId = sem_create(semkey,0);

	semId2 = sem_create(semkey,0);
	
    /*
     * TODO: get value of loop variable(from command - line
     * argument
     */
	if(argc != 2){
		exit(0);
	}
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
	
	sem_signal(semId);

    if (!(pid = fork ())) {
    /*child*/
        for (i = 0; i < loop; i++) {

            /*
             * TODO: swap the contents of shmPtr[0] and  shmPtr[1]
             */
			 sem_wait(semId2);
             temp = shmPtr[0];
             shmPtr[0]=shmPtr[1];
             shmPtr[1]=temp;
             //printf ("In Child Loop values: %li\t%li\n", shmPtr[0], shmPtr[1]);
             if(shmPtr[0] == shmPtr[1]){
             	printf("Error: %li\n",shmPtr[0]);
             }
			 sem_signal(semId);
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
			 sem_wait(semId);
             temp = shmPtr[0];
             shmPtr[0]=shmPtr[1];
             shmPtr[1]=temp;
			 sem_signal(semId2);
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
	
	sem_delete(semId);
	
	sem_delete(semId2);
    return 0;
}
