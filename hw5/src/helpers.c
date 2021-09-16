#include "helpers.h"

/* Helper function definitions go here */
int roundUp(int num, int mul){
	int remainder = num % mul;
	if(remainder == 0){return num;}
	return num + mul - remainder;
}

void* copy_payload(void* dst, void* src, int size){
	char* source = (char*)src;
	char* destination = (char*)dst;
	int i;
	for(i=0; i<size; i++){
		destination[i] = source[i];
	}
	return dst;
}
