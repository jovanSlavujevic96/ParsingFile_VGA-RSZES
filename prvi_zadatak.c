#include "dynArrayOfStr.h"

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

int main(void)
{
	int out, line_count;
	FILE *text_file;
	const char *path = "/root/slavuj/RSZES_PrviZadatak/command.txt";
	
	text_file = fopen(path, "r");
	if(text_file == NULL) 
	{
		printf("Could not open file %s\n",path);
		return -1;
	}
	char **commands = read_lines(text_file, &line_count);
	fclose(text_file);	
	int i=0;
	while(i<line_count && out != -1)
	{
		char arg[6][100] = {{0}};	
		int j,k, flag;
		bool kljuc[6] = {false};
		for(j=0; j<strlen(commands[i]) ;++j)
		{
			if(!kljuc[0] && commands[i][j] != 32)
				arg[0][j] = commands[i][j];
			else if(!kljuc[0] && commands[i][j] == 32)
			{
				flag = j;
				kljuc[0] = true;
			}
			for(k=1; k<6; k++)
			{
				if(kljuc[k-1] && !kljuc[k] && flag != j && commands[i][j] != 32)
					arg[k][j-flag-1] = commands[i][j];
				else if(kljuc[k-1] && !kljuc[k] && flag != j && commands[i][j] == 32)
				{
					flag = j;
					kljuc[k] = true;
				}
			}	
		}
		int color;
		int bckgCmp = strcmp(arg[0], "BCKG:"),
		lineHCmp = strcmp(arg[0], "LINE_H:"),
		lineVCmp = strcmp(arg[0], "LINE_V:"),
		rectCmp = strcmp(arg[0], "RECT:");
	
		printf("[%d]: %s\n", i, commands[i]);

		if(!bckgCmp)
		{
			color = retColor(arg[1]);
			fillBgr(&out, color);
		}
		else if(!lineHCmp)
		{
			int x1 = atoi(arg[1]),
			x2 = atoi(arg[2]),
			y  = atoi(arg[3]);
			color = retColor(arg[4]);
			fillHorizLine(&out, color, x1, x2, y);	
		}
		else if(!lineVCmp)
		{
			int x = atoi(arg[1]),
			y1 = atoi(arg[2]),
			y2 = atoi(arg[3]);
			color = retColor(arg[4]);
			fillVertLine(&out, color, x, y1, y2);
		}
		else if(!rectCmp)
		{
			int x1 = atoi(arg[1]),
			x2 = atoi(arg[2]),
			y1 = atoi(arg[3]),
			y2 = atoi(arg[4]);
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
	for(i=0; i<line_count; ++i)
		free(commands[i]);
	free(commands);
	commands = NULL;

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
