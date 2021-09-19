// Eric Liu
// ziwel10


#include "hw1.h"
// You may define any helper functions you want. Place them in helpers.c/.h


// main program
int main (int argc, char *argv[])
{

	//Insert your hw1 code here
	int i;
	char** opts=argv+1;
	int optCount=argc-1;
	if(optionIsValid(optCount, opts)){
		int wc = 0;
		char firstOpt = argv[1][1];
		int secondOpt = -1; //indicate there is no second Option
		int flag = 0;  //flag for -L and -C Option
		if(optCount==2||optCount==3){
			secondOpt = atoi(argv[2]);  //0 indicate the -O option
		}
		int c=getchar();
		if(c==EOF){
			return 1;
		}
		for(;c!=EOF&&c>=0&&c<=127;c=getchar()){
			wc += wordCount(c, firstOpt, secondOpt, &flag);	
		}
		//non-ASCII character found
		if(c != EOF){
			printf("%d\n",wc);
			return 1;
		}
		printf("%d\n",wc);
		return 0;
	}else{
		return 1;
	}
	return 0;
}
