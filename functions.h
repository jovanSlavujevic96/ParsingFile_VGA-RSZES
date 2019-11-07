#include <stdio.h>     
#include <stdlib.h> 
#include <stdbool.h>

void loop_cycle(int *out);
bool checkIfNumber(const char *num_str);
char** read_lines(FILE* txt, int* count);
void fill(int *error, int x, int y, int color, FILE *fp);
void fillBgr(int *error, const int color);
void fillHorizLine(int *error, const int color, const int X1, const int X2, const int Y);
void fillVertLine(int *error, const int color, const int X, const int Y1, const int Y2);
void fillRectangle(int *error, const int color, const int X1, const int X2, const int Y1, const int Y2);
int retColor(const char *argument);

