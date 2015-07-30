#include <stdio.h>	//printf()
#include <stdlib.h>
#include <string.h>	//strlen()
#include <fcntl.h>	//O_WRONLY
#include <unistd.h>	//write(), close()

#define FIFO_FILE "/tmp/fifo"
#define BUFF_SIZE 50 

int main( void)
{
	int	fd;
	int i;
	unsigned char sstr[BUFF_SIZE]; 
	unsigned char* str = sstr;


	if ( -1 == ( fd = open( FIFO_FILE, O_WRONLY)))
	{
		perror("open() 실행에러");
		exit(1);
	}
	for(i=0; i<BUFF_SIZE; i++)
	{
		sstr[i] = 80;
//		write( fd, str, BUFF_SIZE);
	}		
	for(i=0; i<BUFF_SIZE; i++)
	{
		write( fd, str, BUFF_SIZE);	//save data fd, write data using str, size of BUFF_SIZE
	}
		close(fd);
}
