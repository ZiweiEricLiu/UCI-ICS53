// Eric Liu
// ziwel10



#include "hw0.h"

int main (int argc, char *argv[])
{

	//Comment/Delete this print statement and insert your hw0 code here 
	int i;
	for(i=argc; i>0; i--){
		printArg(argv[i-1], i-1);
	}

	return 0;
}

//Function to print out a single arugment to the screen
void printArg(char * arg_str, int pos){

	//Insert your code here
	printf("Argument[%d]: %s\n",pos,arg_str);
	return;
}
