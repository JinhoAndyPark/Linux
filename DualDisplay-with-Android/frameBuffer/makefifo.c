#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define  FIFO_FILE   "/tmp/fifo"

int main(void)
{
	int   fd;

	if ( -1 == mkfifo( FIFO_FILE, 0666))
	{
		perror( "mkfifo() 실행에러");
		exit( 1);
	}

	return 0;
}
