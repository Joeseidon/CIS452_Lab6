#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/sem.h>



#define SIZE 16
#define CHAR_BUFFER 256

void sem_wait(int semId)
{
	struct sembuf sem_op;
	
	sem_op.sem_num = 0;
	sem_op.sem_op = -1;
	sem_op.sem_flg = SEM_UNDO;
	semop(semId, &sem_op, 1);
	
	/*subtract one*/
	/*sem_op.sem_num = 0;
	sem_op.sem_op = -1;
	sem_op.sem_flg = 0;
	semop(semId, &sem_op, 1);*/
}

void sem_signal(int semId)
{
	struct sembuf sem_op;
	sem_op.sem_num = 0;
	sem_op.sem_op = 1;
	sem_op.sem_flg = SEM_UNDO;
	
	semop(semId, &sem_op, 1);
}

int main (int argc, char *argv[]) {
    int status;
    long int i, loop, temp, *shmPtr;
    int shmId;
    pid_t pid;

	int semId;
	int semId2;
	int id = 'S';	
	char *path;

	//struct sembuf sem;

	/*Get sem key*/
	key_t semkey;
	getcwd(path, CHAR_BUFFER);
	semkey = ftok(path, id);
	
	/*semget*/
	if((semId = semget(semkey, 1, IPC_CREAT|S_IRUSR|S_IWUSR)) < 0){
		perror("Get Error\n");
		exit(1);
	}
	
	/*sem initialize*/
	if(semctl(semId, 0, SETVAL, 0) < 0){
		perror("Init Error\n");
		exit(1);	
	}
	
	if((semId2 = semget(semkey, 1, IPC_CREAT|S_IRUSR|S_IWUSR)) < 0){
		perror("Get Error\n");
		exit(1);
	}
	
	/*sem initialize*/
	if(semctl(semId2, 0, SETVAL, 0) < 0){
		perror("Init Error\n");
		exit(1);	
	}
	
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

    if (!(pid = fork ())) {
    /*child*/
        for (i = 0; i < loop; i++) {

            /*
             * TODO: swap the contents of shmPtr[0] and  shmPtr[1]
             */
			 sem_wait(shmId2);
             temp = shmPtr[0];
             shmPtr[0]=shmPtr[1];
             shmPtr[1]=temp;
             //printf ("In Child Loop values: %li\t%li\n", shmPtr[0], shmPtr[1]);
             if(shmPtr[0] == shmPtr[1]){
             	printf("Error: %li\n",shmPtr[0]);
             }
			 sem_signal(shmId);
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
			 sem_wait(shmId);
             temp = shmPtr[0];
             shmPtr[0]=shmPtr[1];
             shmPtr[1]=temp;
			 sem_signal(shmId2);
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
