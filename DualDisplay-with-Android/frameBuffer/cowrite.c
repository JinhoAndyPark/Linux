#include <stdio.h>      // printf()
#include <unistd.h>     // sleep()
#include <sys/ipc.h>
#include <sys/shm.h>

#define  KEY_NUM     9527 
#define  MEM_SIZE    10 

int main(void)
{
	int   shm_id;
	void *shm_addr;
	int   count;
	char data = 97;
	int i,j;
	
	if ( -1 == ( shm_id = shmget( (key_t)KEY_NUM, MEM_SIZE, IPC_CREAT | 0666)))
	{
		printf( "공유 메모리 생성 실패\n");
		return -1;
	}

	if(( void *)-1 == ( shm_addr = shmat( shm_id, (void*)0, 0)))
	{
		printf( "공유 메모리 첨부 실패\n");
		return -1;
	}

	count = 0;
	for(i=0; i<MEM_SIZE; i++)
	{
		sprintf((char*)shm_addr+i, "%c", data);       // 공유 메모리에 카운터 출력
	}
	
	return 0;
}
