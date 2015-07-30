#include<stdio.h>
#include<stdlib.h>	//for exit
#include<unistd.h>	//for open/close/lseek
#include<fcntl.h>		//for O_RDWR O_RDONLY
#include<sys/ioctl.h>	//for ioctl
#include<sys/types.h> //lseek-> using when move file pointeri
#include<linux/fb.h>	//for fb_var&fix_screeninfo,FBIOGET_VSCREENINFO
#include<sys/mman.h>

#define FBDEVFILE "/dev/fb1"


void getScreenInfo(void);
unsigned short makePixel(unsigned char r, unsigned char g, unsigned char b);


int fbfd;
int ret;
struct fb_var_screeninfo fbvar;
struct fb_fix_screeninfo fbfix;

int main(void)
{	
	unsigned short pixel;
	int offset;
	int i;

	fbfd = open(FBDEVFILE, O_RDWR);//get fb driver file point
	if(fbfd < 0)
	{
		perror("fbdev open");
		exit(1);
	}
	ret = ioctl(fbfd, FBIOGET_VSCREENINFO, &fbvar);//get fb_var_info from kernel, save to fbvar
	if(ret < 0)
	{
		perror("fbdev ioctl");
		exit(1);
	}
	if(fbvar.bits_per_pixel != 16)//check whether bpp is 16 or not
	{
		fprintf(stderr, "bpp is not 16\n");
		exit(1);
	}
	
	if(lseek(fbfd, 0, SEEK_SET) < 0)//file point move (0,0)  
		//off_t lseek(int fildes, off_t offset, int start_flag) fildes->file offset->whenever start on first point(0,0) 
	{
		perror("fbdev lseek");
		exit(1);
	}
	pixel = makePixel(255,0,0); //red pixel write(fbfd, &pixel, 2); //write 2byte(16bit) 
	offset = 50*fbvar.xres*(16/8)+100*(16/8);//(100.50) //fvar.xres = 320 
	if(lseek(fbfd, offset, SEEK_SET) < 0)	//move file point to 0+offset {
	{
		perror("fbdev lseek");
		exit(1);
	}
	pixel = makePixel(0,255,0); //green pixel
	write(fbfd, &pixel, 2); //write 2byte(16bit)
	
	offset = 50*(16/8)+100*fbvar.xres*(16/8); //(50,100)
	if(lseek(fbfd, offset, SEEK_SET) < 0)
	{
		perror("fbdev lseek");
		exit(1);
	}
	for(i=0;i<1200;i++)
	{
	pixel = makePixel(0,0,255);//blue pixel
	write(fbfd, &pixel, 2);
	}
	close(fbfd);
	exit(0);

	return 0;
}
	
unsigned short makePixel(unsigned char r, unsigned char g, unsigned char b)
{
	return (unsigned short)(((r>>3)<<11)|((g>>2)<<5)|(b>>3));
}

void getScreenInfo(void)
{
	fbfd = open(FBDEVFILE,O_RDWR);//fb node's /dev/fb open
	if(fbfd < 0)
	{
		 perror("fbdev open");
	 	 exit(1);
	}
	
	ret = ioctl(fbfd, FBIOGET_VSCREENINFO, &fbvar);//get info fb_var_info from kernel, save to fbvar
	if(ret < 0)
	{
		perror("fbdev ioctl(FSCREENINFO)");
		exit(1);
	}

	ret = ioctl(fbfd, FBIOGET_FSCREENINFO, &fbfix);//get info fb_fix_info from kernel, save to fbfix
	if(ret < 0)
	{
		perror("fbdev ioctl(FSCREENINFO)");
		exit(1);
	}
	
	printf("x-resolution: %d\n",fbvar.xres);
	printf("y-resolution: %d\n",fbvar.yres);
	printf("x-resolution(virtual): %d\n",fbvar.xres_virtual);
	printf("y-resolution(virtual): %d\n",fbvar.yres_virtual);
	printf("bpp: %d\n",fbvar.bits_per_pixel);
	printf("length of frame buffer memory: %d\n",fbfix.smem_len);


}
/* i dont know the function makePixel(idontknow,r,g,b) 
	
unsigned short makePixel(unsigned char r, unsigned char g, unsigned char b);
{
	return (unsigned short)(((r>>3)<<11)|((g>>2)<<5)|(b>>3));
}

unsigned short pixel;
	int offset;
	
	fbfd = open(FBDEVFILE, O_RDWR);//get fb driver file point
	if(fbfd < 0)
	{
		perror("fbdev open");
		exit(1)
	}
	ret = ioctl(fbfd, FBIOGET_VSCREENINFO, &fbvar);//get fb_var_info from kernel, save to fbvar
	if(ret < 0)
	{
		perror("fbdev ioctl");
		exit(1);
	}
	if(fbvar.bits_per_pixel != 16)//check whether bpp is 16 or not
	{
		fprintf(stderr, "bpp is not 16\n");
		exit(1);
	}
	
	if(lseek(fbfd, 0, SEEK_SET) < 0)//file point move (0,0)  off_t lseek(int fildes, off_t offset, int start_flag) fildes->file offset->whenever start on first point(0,0) {
		perror("fbdev lseek");
		exit(1);
	}
	pixel = makePixel(&fbvar,255,0,0); //red pixel write(fbfd, &pixel, 2); //write 2byte(16bit) offset = 50*fbvar.xres*(16/8)+100*(16/8);//(100.50) //fvar.xres = 320 if(lseek(fbfd, offset, SEEK_SET) < 0)	//move file point to 0+offset {
		perror("fbdev lseek");
		exit(1);
	}
	pixel = makePixel(&fbvar,0,255,0); //green pixel
	write(fbfd, &pixel, 2); //write 2byte(16bit)
	
	offset = 50*(16/8)+100*fbvar.xres*(16/8); //(50,100)
	if(lseek(fbfd, offset, SEEK_SET) < 0)
	{
		perror("fbdev lseek");
		exit(1);
	}
	pixel = makePixel(&fbvar,0,0,255);//blue pixel
	write(fbfd, &pixel, 2);

	offset = 100*(16/8) + 100*fbvar.xres*(16/8); //(100,100)
	if(lseek(fbfd, offset, SEEK_SET) < 0)
	{
		perror("fbdev lseek");
		exit(1);
	}
	pixel = makePixel(&fbvar,255,255,255);//white pixel
	write(fbfd, &pixel, 2);//write 2byte(16bit)
*/	
