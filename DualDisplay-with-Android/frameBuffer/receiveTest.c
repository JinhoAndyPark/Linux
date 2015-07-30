#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h> 
#define FIFO_FILE "/tmp/fifo"
#define BUFF_SIZE 2048 

int main(void)
{
	int i, j, k=0, fd;
	char buff[BUFF_SIZE];
	int cnt=0;
	char fbuff[75][BUFF_SIZE];
	char ffbuff[153600];
	int ccnt=75;
	if( (fd = open(FIFO_FILE, O_RDWR)) == -1 )
	{
		perror("open() start error");
		exit(1);
	}
				
	memset(buff, 0, BUFF_SIZE);
	
	for(i=0; i<75; i++)
	{
		read(fd, buff, BUFF_SIZE);
		
		for(j=0; j<BUFF_SIZE; j++)
		{
			fbuff[i][j] = buff[j];
			ffbuff[k++] = fbuff[i][j]; 
		//	printf("buff %d : %d  \n" , cnt++, buff[i]);
		}
//		printf("fbuff %d: \n",fbuff[cnt]);
//		if(cnt==153599) break;
	}
		printf("fbuff[%d]=%d \n", i, fbuff[74][2000]);
		k=0;
		for(k=0; k<153600; k++)
		{
			printf("ffbuff[%d] = %d \n", k, ffbuff[k]);
		}
		close(fd);
}
