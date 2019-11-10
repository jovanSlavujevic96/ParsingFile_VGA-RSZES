#include "functions.h"

#define TextFilePath "/root/slavuj/RSZES_PrviZadatak/command.txt"

void loop_cycle(int *out);

int main(void)
{
	int out;
	loop_cycle(&out);
	return out;
}

void loop_cycle(int *out)
{
	if(*out)
		*out = 0;
	
	FILE *text_file;
	int line_count;
	text_file = fopen(TextFilePath, "r");
	if(!text_file) 
	{
		printf("Could not open file %s\n",TextFilePath);
		*out = -1;
		return;
	}
	char **commands = read_lines(text_file, &line_count);
	fclose(text_file);

	int i=0;
	while(i<line_count && *out != -1 )
	{
		char arg[6][100] = {{0}};	
		int j,k, flag;
		bool key[6] = {false};
		for(j=0; j<strlen(commands[i])-1 ;++j)
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
    
		printf("[%d]: %s\n", i, commands[i]);
    if(!checkAndExecute(arg) )
      *out = -1;

    else
    {
		  printf("Executed\n");
		  usleep(2000000); //delay 2sec 
    }
    ++i;
	}
	
	for(i=0; i<line_count; ++i)
		free(commands[i]);
	free(commands);
	commands = NULL;	
}
