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

#define MAX_STR_SIZE 1000
#define NUM_OF_LINES 5 

void fill(int *error, int x, int y, int color, FILE *fp);
void fillBgr(int *error, const int color);
void fillHorizLine(int *error, const int color, const int X1, const int X2, const int Y);
void fillVertLine(int *error, const int color, const int X, const int Y1, const int Y2);
void fillRectangle(int *error, const int color, const int X1, const int X2, const int Y1, const int Y2);
int retColor(const char *argument);

int main(void)
{
	int out;
	FILE *text_file;
	const char *path = "/root/RSZES_PrviZadatak/command.txt";
	char str[MAX_STR_SIZE];
	
	text_file = fopen(path, "r");
	if(text_file == NULL) 
	{
		printf("Could not open file %s\n",path);
		return -1;
	}
	int i=0;
	char commands[NUM_OF_LINES][MAX_STR_SIZE];
	while(fgets(str,MAX_STR_SIZE, text_file) != NULL)
	{
		printf("%d: %s",i, str);
		strcpy(commands[i], str);
		++i;
	}
	fclose(text_file);

	i = 0;
	while(i < NUM_OF_LINES && out != -1)
	{
		char arg[6][100] = {{0}};	
		int j,k, flag[5];
		bool kljuc[6] = {false};
		for(j=0; j<strlen(commands[i]);++j)
		{
			if(!kljuc[0] && commands[i][j] != 32)
				arg[0][j] = commands[i][j];
			else if(!kljuc[0] && commands[i][j] == 32)
			{
				flag[0] = j;
				kljuc[0] = true;
			}
			for(k=1; k<5; k++)
			{
				if(kljuc[k-1] && !kljuc[k] && flag[k-1] != j && commands[i][j] != 32)
					arg[k][j-flag[k-1]-1] = commands[i][j];
				else if(kljuc[k-1] && !kljuc[k] && flag[k-1] != j && commands[i][j] == 32)
				{
					flag[k] = j;
					kljuc[k] = true;
				}
			}	
			if(kljuc[4] && !kljuc[5] && flag[4] != j && commands[i][j] != 32 && commands[i][j] != 10)
				arg[5][j-flag[4]-1] = commands[i][j];
		}
		int color;
		int bckgCmp = strcmp(arg[0], "BCKG:"),
		lineHCmp = strcmp(arg[0], "LINE_H:"),
		lineVCmp = strcmp(arg[0], "LINE_V:"),
		rectCmp = strcmp(arg[0], "RECT:");
	
		if(!bckgCmp)
		{
			printf("%s", arg[1]);
			arg[1][strlen(arg[1])-1] = 0;
			printf("%s", arg[1]);
			color = retColor(arg[1]);
			fillBgr(&out, color);
		}
		else if(!lineHCmp)
		{
			int x1 = atoi(arg[1]),
			x2 = atoi(arg[2]),
			y  = atoi(arg[3]);
			arg[4][strlen(arg[4])-1] = 0;
			color = retColor(arg[4]);
			fillHorizLine(&out, color, x1, x2, y);	
		}
		else if(!lineVCmp)
		{
			int x = atoi(arg[1]),
			y1 = atoi(arg[2]),
			y2 = atoi(arg[3]);
			arg[4][strlen(arg[4])-1] = 0;
			color = retColor(arg[4]);
			fillVertLine(&out, color, x, y1, y2);
		}
		else if(!rectCmp)
		{
			int x1 = atoi(arg[1]),
			x2 = atoi(arg[2]),
			y1 = atoi(arg[3]),
			y2 = atoi(arg[4]);
			arg[5][strlen(arg[5])-1] = 0;
			color = retColor(arg[5]);
			fillRectangle(&out, color, x1, x2, y1, y2);	
		}
		else
		{
			printf("Na %d liniji %s fajla nepravilan unos komande\n", i, path);
			out = -1;
		}
		printf("Izvrseno\n");
		usleep(2000000); 
		++i;
	}
	return 0;
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
	printf("%s", argument);
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
