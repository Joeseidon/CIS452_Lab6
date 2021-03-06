#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "sem_wrapper.h"


void sem_wait(int semId, int num)
{
	struct sembuf sem_op;
	
	sem_op.sem_num = num;
	sem_op.sem_op = -1;
	sem_op.sem_flg = SEM_UNDO;
	semop(semId, &sem_op, 1);
}

void sem_signal(int semId, int num)
{
	struct sembuf sem_op;
	sem_op.sem_num = num;
	sem_op.sem_op = 1;
	sem_op.sem_flg = SEM_UNDO;
	
	semop(semId, &sem_op, 1);
}

int sem_create(int num, unsigned short *start_val)
{
	union semun arg;
	int semId;
	
	if ((semId = semget(IPC_PRIVATE, 1, IPC_CREAT|S_IRUSR|S_IWUSR)) < 0) {
		perror("Get Error\n");
		exit(1);
	}
	
	arg.array = start_val;
	if(semctl(semId, 0, SETALL, arg) < 0){
		perror("Init Error\n");
		exit(1);	
	}
	
	return semId;
}

void sem_delete(int semId)
{
	if( semctl(semId, 0, IPC_RMID) == -1)
	{
		perror("Could Not Remove Semaphore!");
		exit(1);
	}
}
