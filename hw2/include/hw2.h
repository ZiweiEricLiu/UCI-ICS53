#include<stdio.h>
#include<stdlib.h>

void createKey(char*, char* );
char morseToKey(char*, char* );
int toMorse(FILE*, char** );
int FMCEncrypt(FILE *, char* , FILE* );
int fromMorse(char* , FILE* );
int FMCDecrypt(FILE*, char* , FILE* );

