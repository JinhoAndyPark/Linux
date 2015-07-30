

#include<stdio.h> 
#include<stdlib.h> /* for exit */
#include<unistd.h> /* for open/close .. */
#include<fcntl.h> /* for O_RDWR */
#include<sys/ioctl.h> /* for ioctl */
#include<sys/mman.h> /* for mmap */
#include<sys/types.h> //lseek ->using when move file pointer
#include<linux/fb.h> /* for fb_var_screeninfo, FBIOGET_VSCREENINFO */

#define FBDEVFILE "/dev/fb1"

typedef unsigned char ubyte;

unsigned short makepixel(ubyte r, ubyte g, ubyte b)
{
	return (unsigned short)(((r>>3)<<11)|((g>>2)<<5)|(b>>3));
}

int main()
{
	int fbfd;
	int ret;
	int i,fbsize;
	struct fb_var_screeninfo fbvar;
	unsigned short pixel;
	int offset;
	unsigned short *pfbdata;

	fbfd = open(FBDEVFILE, O_RDWR);
	if(fbfd < 0)
	{
		perror("fbdev open");
		exit(1);
	}

	ret = ioctl(fbfd, FBIOGET_VSCREENINFO, &fbvar);
	if(ret < 0)
	{
		perror("fbdev ioctl");
		exit(1);
	}

	if(fbvar.bits_per_pixel != 16)
	{
		fprintf(stderr, "bpp is not 16n");
		exit(1);
	}
	fbsize = fbvar.xres*fbvar.yres*16/8;


	pfbdata = (unsigned short *)
		mmap(0, fbvar.xres*fbvar.yres*16/8,
				PROT_READ|PROT_WRITE, MAP_SHARED, fbfd, 0);

	if((unsigned)pfbdata == (unsigned)-1)
	{
		perror("fbdev mmap");
		exit(1);
	}
	for(i=0; i<fbsize ; i++)
	{
		*(pfbdata+i)=0x00;
	}
	munmap(pfbdata, fbvar.xres*fbvar.yres*16/8);
	close(fbfd);
	exit(0);
	return 0;
}
