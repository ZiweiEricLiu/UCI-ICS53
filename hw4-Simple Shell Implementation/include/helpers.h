// A header file for helpers.c
// Declare any additional functions in this file
#include "shell_util.h"
#include "linkedList.h"

int timeComparator(void* pe1, void* pe2);
ProcessEntry_t* findPeFromList(List_t* l, int pid); 
