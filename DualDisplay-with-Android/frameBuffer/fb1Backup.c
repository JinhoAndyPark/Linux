
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

unsigned short bytepixel(ubyte rg, ubyte gb)
{
	return (unsigned short)((rg<<8) | (gb));
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
	unsigned char *pfbdata2;

	fbfd = open(FBDEVFILE, O_RDWR);	// /dev/fb1 RDed and WRite
	if(fbfd < 0)
	{
		perror("fbdev open");
		exit(1);
	}

	ret = ioctl(fbfd, FBIOGET_VSCREENINFO, &fbvar); // get info from fb_var_screeninfo,thr variable value
	if(ret < 0)
	{
		perror("fbdev ioctl");
		exit(1);
	}

	if(fbvar.bits_per_pixel != 16)	//make sure the fb packet
	{
		fprintf(stderr, "bpp is not 16n");
		exit(1);
	}
	fbsize = fbvar.xres*fbvar.yres*2;//fb size 320*240*2 = 153600 

	pfbdata2 = (unsigned char*)	//kernel fb memory -> user fb memory
		mmap(0, fbvar.xres*fbvar.yres*2,
				PROT_READ|PROT_WRITE, MAP_SHARED, fbfd, 0);

	if((unsigned)pfbdata == (unsigned)-1)
	{
		perror("fbdev mmap");
		exit(1);
	}


	for(i=0; i<1000 ; i+=2)
	{
		*(pfbdata2+i)=224;
		*(pfbdata2+i+1)=7;
//		*(pfbdata+i)=makepixel(0,0,255);
//		*(pfbdata+i)=bytepixel(0,7);
	}
	munmap(pfbdata, fbvar.xres*fbvar.yres*2);
	close(fbfd);
	exit(0);
	return 0;
}
