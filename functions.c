#include "functions.h"
  
#include <string.h>    
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

#define MAX_PKT_SIZE (640*480*4)

#define BLUE 0x001f
#define BLACK 0x0000
#define YELLOW 0xffe0
#define RED 0xf800
#define GREEN 0x07e0

#define TextFilePath "/root/slavuj/RSZES_PrviZadatak/command.txt"

void loop_cycle(int *out)
{
	if(*out)
		*out = 0;
	
	FILE *text_file;
	int line_count;
	text_file = fopen(path, "r");
	if(!text_file) 
	{
		printf("Could not open file %s\n",TextFilePath);
		*out = -1;
		return;
	}
	char **commands = read_lines(text_file, &line_count);
	fclose(text_file);

	int i=0;
	while(i<line_count && *out != -1)
	{
		char arg[6][100] = {{0}};	
		int j,k, flag;
		bool key[6] = {false};
		for(j=0; j<strlen(commands[i]) ;++j)
		{
			if(!key[0] && commands[i][j] != 32)
				arg[0][j] = commands[i][j];
			else if(!key[0] && commands[i][j] == 32)
			{
				flag = j;
				key[0] = true;
			}
			for(k=1; k<6; k++)
			{
				if(key[k-1] && !key[k] && flag != j && commands[i][j] != 32)
					arg[k][j-flag-1] = commands[i][j];
				else if(key[k-1] && !key[k] && flag != j && commands[i][j] == 32)
				{
					flag = j;
					key[k] = true;
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
			if(color == -1)
			{
				printf("Error\nThis is not a color you can choose: \"%s\"\nExit\n", arg[1]);
				*out = -1;
			}
			fillBgr(out, color);
		}
		else if(!lineHCmp)
		{
			for(j = 1; j <= 3; ++j)
				if(!checkIfNumber(arg[j]) )
				{
					printf("Error\nThis is not a number: %s\nExit\n", arg[j]);
					*out = -1;
				}
			int x1 = atoi(arg[1]),
			x2 = atoi(arg[2]),
			y  = atoi(arg[3]);
			color = retColor(arg[4]);
			if(color == -1)
			{
				printf("Error\nThis is not a color you can choose: \"%s\"\nExit\n", arg[1]);
				*out = -1;
			}
			fillHorizLine(out, color, x1, x2, y);	
		}
		else if(!lineVCmp)
		{
			for(j = 1; j <= 3; ++j)
				if(!checkIfNumber(arg[j]) )
				{
					printf("Error\nThis is not a number: %s\nExit\n", arg[j]);
					*out = -1;
				}
			int x = atoi(arg[1]),
			y1 = atoi(arg[2]),
			y2 = atoi(arg[3]);
			color = retColor(arg[4]);
			if(color == -1)
			{
				printf("Error\nThis is not a color you can choose: \"%s\"\nExit\n", arg[1]);
				*out = -1;
			}
			fillVertLine(out, color, x, y1, y2);
		}
		else if(!rectCmp)
		{
			for(j = 1; j <= 4; ++j)
				if(!checkIfNumber(arg[j]) )
				{
					printf("Error\nThis is not a number: %s\nExit\n", arg[j]);
					*out = -1;
				}
			int x1 = atoi(arg[1]),
			x2 = atoi(arg[2]),
			y1 = atoi(arg[3]),
			y2 = atoi(arg[4]);
			color = retColor(arg[5]);
			if(color == -1)
			{
				printf("Error\nThis is not a color you can choose: \"%s\"\nExit\n", arg[1]);
				*out = -1;
			}
			fillRectangle(out, color, x1, x2, y1, y2);	
		}
		else
		{
			printf("Error\nBad command \"%s\"\nExit!\n", arg[0]);
			*out = -1;
		}
		printf("Executed\n");
		usleep(2000000); //delay 2sec 
		++i;
	}
	for(i=0; i<line_count; ++i)
		free(commands[i]);
	free(commands);
	commands = NULL;	
}

bool checkIfNumber(const char *num_str)
{
	int i, true_incr = 0;
	for(i=0; i<strlen(num_str); ++i)
	{
		if(num_str[i] >= 48 && num_str[i] <= 57)
			++true_incr;
		if(i == (strlen(num_str)-1) && true_incr == strlen(num_str) )
			return true;
		else if(i == (strlen(num_str)-1) && true_incr != strlen(num_str) )
			return false;
	}
}

char** read_lines(FILE* txt, int* count) {
    char** array = NULL;        
    int    i;                   
    char   line[100];           
    int    line_count;          
    int    line_length;         

    *count = 0;

    line_count = 0;
    while (fgets(line, sizeof(line), txt) != NULL)                             		
		line_count++;
    
    rewind(txt);

    array = malloc(line_count * sizeof(char *));
    if (array == NULL) 
        return NULL; 
    
    for (i = 0; i < line_count; i++) 
	{    
        fgets(line, sizeof(line), txt);
        line_length = strlen(line);        
        line[line_length - 1] = '\0';
        line_length--; 
        array[i] = malloc(line_length + 1);
        strcpy(array[i], line);
    }
    *count = line_count;
    return array;
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
	else if(!blackCmp) color = BLACK;
	else if(!yellowCmp) color = YELLOW;
	else if(!redCmp) color = RED;
	else if(!greenCmp) color = GREEN;
	else color = -1;
	
	return color;
}