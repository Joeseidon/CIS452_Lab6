#ifndef SEM_WRAPPER_H_
#define SEM_WRAPPER_H_

union semun
{
	int val; // value for SETVAL
	struct semid_ds* buf; // buffer for IPC_STAT, IPC_SET
	unsigned short* array; // array for GETALL, SETALL
	struct seminfo* __buf; // buffer for IPC_INFO
};

void sem_wait(int semId);

void sem_signal(int semId);

int sem_create(key_t semkey, int start_val);

void sem_delete(int semId);


#endif