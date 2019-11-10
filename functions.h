#include <stdio.h>     
#include <stdlib.h> 
#include <stdbool.h>
#include <string.h>

bool checkAndExecute(char commands[6][100]);
bool checkIfNumber(const char *num_str);
char** read_lines(FILE* txt, int* count);
bool  fill(int x, int y, int color, FILE *fp);
bool fillBgr(const int color);
bool fillHorizLine(const int color, const int X1, const int X2, const int Y);
bool fillVertLine(const int color, const int X, const int Y1, const int Y2);
bool fillRectangle(const int color, const int X1, const int X2, const int Y1, const int Y2);
int retColor(const char *argument);

