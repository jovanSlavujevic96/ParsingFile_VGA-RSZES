#include "functions.h"
  
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

#define MAX_PKT_SIZE (640*480*4)

#define BLUE 0x001f
#define BLACK 0x0000
#define YELLOW 0xffe0
#define RED 0xf800
#define GREEN 0x07e0

bool checkAndExecute(char commands[6][100])
{ 
  int i,incr=0;
  for(i=0; i<6; ++i)
    if(commands[i][0] != 0)
      ++incr;
    else
      break;

  if(!incr || incr == 1)
  {
    printf("Error.\nThere's no enough arguments");
    return false;
  }
  int color = retColor(commands[incr-1]);
  if(color == -1)
  {
      printf("Error.\nThere's no such a color\n");
      return false;
  }  
  int bckCmp = strcmp(commands[0], "BCKG:"), HlineCmp= strcmp(commands[0], "LINE_H:"), VlineCmp = strcmp(commands[0], "LINE_V:"), rectCmp = strcmp(commands[0], "RECT:");
  if(!bckCmp && incr == 2)
    return( fillBgr(color) );
  else if(!HlineCmp && incr == 5)
  { 
    if( !checkIfNumber(commands[1]) || !checkIfNumber(commands[2]) || !checkIfNumber(commands[3]) )
      return false;
    int x1 = atoi(commands[1]), x2 = atoi(commands[2]), y = atoi(commands[3]);
    return (fillHorizLine(color, x1, x2, y) );
  } 
  else if(!VlineCmp && incr == 5)
  {
    if( !checkIfNumber(commands[1]) || !checkIfNumber(commands[2]) || !checkIfNumber(commands[3]) )
      return false;
    int x = atoi(commands[1]), y1 = atoi(commands[2]), y2 = atoi(commands[3]);
    return (fillVertLine(color, x, y1, y2) );
  }
  else if(!rectCmp && incr == 6)
  {
    int x1 = atoi(commands[1]), x2 = atoi(commands[2]), y1 = atoi(commands[3]), y2 = atoi(commands[4]);
    return (fillRectangle(color, x1, x2, y1, y2) );
  }
  else
  {
    printf("Error.\nBad command or not enough arguments\n");
    return false;
  } 
}

bool checkIfNumber(const char *num_str)
{
	int i;
	for(i=0; i<strlen(num_str); ++i)
		if(num_str[i] < 48 || num_str[i] > 57)	
			return false;
	return true;
}

char** read_lines(FILE* txt, int* count) 
{
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

bool fillBgr(const int color)
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
		return false; 
	}
	p=(int*)mmap(0,640*480*4, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	memcpy(p, vectorColor, MAX_PKT_SIZE);
	munmap(p, MAX_PKT_SIZE);
	close(fd);
	if (fd < 0)
	{
		printf("Cannot close /dev/vga for write\n");
		return false; 
	}
  return true;
}

bool fillHorizLine(const int color, const int X1, const int X2, const int Y)
{
  int x,y;
  if(X1 > X2)
  {
    printf("Error\nX1: %d is bigger than X2 : %d\nExit Program!\n", X1, X2);
    return false;
  }
	FILE* fp;
	y = Y;
	for(x=X1; x<X2; ++x)
		if( !fill(x, y, color, fp) )
      return false;
  return true;
}

bool fillVertLine(const int color, const int X, const int Y1, const int Y2)
{
	if(Y1 > Y2)
  {
    printf("Error\nY1: %d is bigger than Y2: %d\nExit Program!\n", Y1, Y2);
    return false;
  }
  int x,y;
	FILE* fp;
	x = X;
	for(y=Y1; y<Y2; ++y)
		if( !fill(x, y, color, fp) )
      return false;
  return true;
}

bool fill(int x, int y, int color, FILE *fp)
{
	fp = fopen("/dev/vga_dma", "w");		
	if(fp == NULL)
	{
		printf("Cannot open /dev/vga for write\n");
		return false;
	}
	fprintf(fp,"%d,%d,%#04x\n",x,y,color);
	fclose(fp);
	if(fp == NULL)
	{
    printf("Cannot close /dev/vga\n");
		return false;
	}
  return true;
}


bool fillRectangle(const int color, const int X1, const int X2, const int Y1, const int Y2)
{
	if(!fillVertLine(color, X1, Y1, Y2) || !fillVertLine(color, X2, Y1, Y2) || 
      !fillHorizLine(color, X1, X2, Y1) || !fillHorizLine(color, X1, X2, Y2) )
    return false;
  return true;
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
