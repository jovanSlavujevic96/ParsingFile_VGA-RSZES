#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#define MAX_PKT_SIZE (640*480*4)

#define BLUE 0x001f
#define BLACK 0x0000
#define YELLOW 0xffe0
#define RED 0xf800
#define GREEN 0x07e0

void fill(int *error, int x, int y, int color, FILE *fp);
void fillBgr(int *error, const int color);
void fillHorizLine(int *error, const int color, const int X1, const int X2, const int Y);
void fillVertLine(int *error, const int color, const int X, const int Y1, const int Y2);
void fillRectangle(int *error, const int color, const int X1, const int X2, const int Y1, const int Y2);
int retColor(const char *argument);

int main(int argc, char **argv)
{
	bool background = false;
	int color, out;
	if(argc != 3 && argc != 6 && argc != 7)
	{
		printf("Pogresan unos.\nPrimer unosa:\n%s BCKG: BLACK\n",argv[0]);
		return -1;
	}	
	int bckgCmp = strcmp(argv[1], "BCKG:"),
	lineHCmp = strcmp(argv[1], "LINE_H:"),
	lineVCmp = strcmp(argv[1], "LINE_V:"),
	rectCmp = strcmp(argv[1], "RECT:");
	
	if(!bckgCmp)
	{
		color = retColor(argv[2]);
		fillBgr(&out, color);
	}
	
	if(!lineHCmp)
	{
		int x1 = atoi(argv[2]),
		x2 = atoi(argv[3]),
		y  = atoi(argv[4]);
		color = retColor(argv[5]);
		fillHorizLine(&out, color, x1, x2, y);	
	}

	if(!lineVCmp)
	{
		int x = atoi(argv[2]),
		y1 = atoi(argv[3]),
		y2 = atoi(argv[4]);
		color = retColor(argv[5]);
		fillVertLine(&out, color, x, y1, y2);
	}

	if(!rectCmp)
	{
		int x1 = atoi(argv[2]),
		x2 = atoi(argv[3]),
		y1 = atoi(argv[4]),
		y2 = atoi(argv[5]);
		color = retColor(argv[6]);
		fillRectangle(&out, color, x1, x2, y1, y2);	
	}

	return out;
}

void fillBgr(int *error, const int color)
{

	unsigned int vectorColor[480][640];
	int i,j;
	for(i=0; i<480; ++i)
		for(j=0; j<640; ++j)
			vectorColor[i][j] = color;

	FILE *fp;
	int fd;
	int *p;
	fd = open("/dev/vga_dma", O_RDWR|O_NDELAY);
	if (fd < 0)
	{
		printf("Cannot open /dev/vga for write\n");
		*error = -1;
		return; 
	}
	p=(int*)mmap(0,640*480*4, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	memcpy(p, vectorColor, MAX_PKT_SIZE);
	munmap(p, MAX_PKT_SIZE);
	close(fd);
	if (fd < 0)
	{
		printf("Cannot close /dev/vga for write\n");
		*error = -1;
		return; 
	}
}

void fillHorizLine(int *error, const int color, const int X1, const int X2, const int Y)
{
	int x,y;
	*error = 0;
	FILE* fp;
	y = Y;
	for(x=X1; x<X2; ++x)
		fill(error, x, y, color, fp);
}

void fillVertLine(int *error, const int color, const int X, const int Y1, const int Y2)
{
	int x,y;
	*error = 0;
	FILE* fp;
	x = X;
	for(y=Y1; y<Y2; ++y)
		fill(error, x, y, color, fp);
}

void fill(int *error, int x, int y, int color, FILE *fp)
{
	fp = fopen("/dev/vga_dma", "w");		
	if(fp == NULL)
	{
		printf("Cannot open /dev/vga for write\n");
		*error = -1;
		return;
	}
	fprintf(fp,"%d,%d,%#04x\n",x,y,color);
	fclose(fp);
	if(fp == NULL)
	{
		printf("Cannot close /dev/vga\n");
		*error = -1;
		return;
	}
}


void fillRectangle(int *error, const int color, const int X1, const int X2, const int Y1, const int Y2)
{
	fillVertLine(error, color, X1, Y1, Y2);
	fillVertLine(error, color, X2, Y1, Y2);
	fillHorizLine(error, color, X1, X2, Y1);
	fillHorizLine(error, color, X1, X2, Y2);
}

int retColor(const char *argument)
{
	int color;
	int blueCmp = strcmp(argument,"BLUE"), 
	blackCmp = strcmp(argument, "BLACK"), 
	yellowCmp = strcmp(argument,"YELLOW"),
	redCmp = strcmp(argument,"RED"),
	greenCmp = strcmp(argument,"GREEN");	

	if(!blueCmp) color = BLUE;
	if(!blackCmp) color = BLACK;
	if(!yellowCmp) color = YELLOW;
	if(!redCmp) color = RED;
	if(!greenCmp) color = GREEN;

	return color;
}
