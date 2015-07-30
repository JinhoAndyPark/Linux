#include <stdio.h>      // printf()
#include <unistd.h>     // sleep()
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>

#define  KEY_NUM     9527
#define  MEM_SIZE    153600

int main(void)
{
	int shm_id;
//	void *shm_addr;
	char* shm_addr;
	int cnt=0;
	int i=0;
	char out[5]={22,2,3,4,5};
	char in[MEM_SIZE];
	char* ptr_sour = "falinux.forum.com";
	char* ptr_sour2 = &out[0];
	char* ptr_dest = &in[0];
	char* ptr_fbuf = &in[0];
//	memcpy(ptr_dest, ptr_sour2, 5);
	
//	for(i=0;i<5;i++)
//	{
//		printf("%d \n",*(ptr_dest+i));
//	}
	if ( -1 == ( shm_id = shmget( (key_t)KEY_NUM, MEM_SIZE, IPC_CREAT | 0666)))
	{
		printf( "공유 메모리 생성 실패\n");
		return -1;
	}
	if ( ( void *)-1 == ( shm_addr = shmat( shm_id, ( void *)0, 0)))
	{
		printf( "공유 메모리 첨부 실패\n");
		return -1;							     
	}

	memcpy(ptr_fbuf, shm_addr,MEM_SIZE );
	for(i=0 ;i<10; i++)
	printf("15359%d : %d \n",cnt++,*(ptr_fbuf+i));	
//	for(i=0; i<MEM_SIZE; i++)
//	printf("cnt:%d value:%d \n",cnt++,*(ptr_fbuf+i));
	
	/*
	for(i=0; i<MEM_SIZE; i++)
	{
	//	printf((char*)shm_addr, "%c", 97);
	//	printf("cnt:%d data d:%d data c:%c data s:%s \n",i ,*((char *)shm_addr+i),*((char*)shm_addr+i),*((char*)shm_addr+i));  
		printf("cnt:%d data d:%d data c:%c \n",i ,*((char*)shm_addr+i),*((char*)shm_addr+i));  
	
	}
*/
//		printf("cnt:%d data d:%d data c:%c \n",153599 ,*(shm_addr+153599),*(shm_addr+153599));  
	//	printf( "cnt:%d data d:%d \n",i ,*((char *)shm_addr+153599));  
	return 0;
}

