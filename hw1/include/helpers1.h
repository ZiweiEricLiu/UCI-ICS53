// Declare all helper functions for hw1 in this file

void printArg(char * arg_str, int pos);
int optionIsValid(int optCount, char* opts[]);
int wordCount(int c, char firstOpt, int secondOpt, int* flag);
int swordCount(int c, int secondOpt);
int nwordCount(int c, int secondOpt, int* lastIsNum);
int lwordCount(int c, int secondOpt, int* isleading);
int ewordCount(int c, int secondOpt);
int cwordCount(int c, int secondOpt, int* numSpace);
