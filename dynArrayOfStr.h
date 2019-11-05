#include <stdio.h>     
#include <stdlib.h>    
#include <string.h>    
#include <stdbool.h>

#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

char** read_lines(FILE* txt, int* count) {
    char** array = NULL;        /* Array of lines */
    int    i;                   /* Loop counter */
    char   line[100];           /* Buffer to read each line */
    int    line_count;          /* Total number of lines */
    int    line_length;         /* Length of a single line */

    *count = 0;

    line_count = 0;
    while (fgets(line, sizeof(line), txt) != NULL)                             		line_count++;
    
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

