// Define all helper functions for hw1 in this file

#include <stdio.h>
#include <stdlib.h>

//Function to print out a single arugment to the screen
//Use for Debugging
void printArg(char * arg_str, int pos){
	printf("Argument[%d]: %s\n",pos,arg_str);
	return;
}

//check the options are valid
int  optionIsValid(int optCount, char* opts[]){
	if(optCount == 0 || optCount > 3){
		return 0;
	}
	if(optCount == 1){
		char firstOpt = opts[0][1];
		if(firstOpt != 'S' && firstOpt != 'N' && firstOpt != 'L'){
			return 0;
		}
	}
	else if(optCount == 2){
		char firstOpt = opts[0][1];
		if(firstOpt == 'S' || firstOpt == 'N' || firstOpt == 'L'){
			char secondOpt = opts[1][1];
			if(secondOpt != 'O'){
				return 0;
			}
		}
		else if(firstOpt == 'E' || firstOpt == 'C'){
			int secondOpt = atoi(opts[1]);
			if(secondOpt == 0){
				return 0;
			}
		}else{
			return 0;
		}
	}
	else if(optCount == 3){
		char firstOpt = opts[0][1];
		int secondOpt = atoi(opts[1]);
		char thirdOpt = opts[2][1];
		if(firstOpt != 'E' && firstOpt != 'C'){
			return 0;
		}else{
			if(secondOpt == 0 || thirdOpt != 'O'){
				return 0;
			}
		}
	}
	return 1;
}

//Execute the options
int wordCount(int c, char firstOpt, int secondOpt, int* flag){
	if(firstOpt == 'S'){
		return swordCount(c, secondOpt);
	}
	if(firstOpt == 'N'){
		return nwordCount(c, secondOpt, flag);
	}
	if(firstOpt == 'L'){
		return lwordCount(c, secondOpt, flag);
	}
	if(firstOpt == 'E'){
		return ewordCount(c, secondOpt);
	}
	if(firstOpt == 'C'){
		return cwordCount(c, secondOpt, flag);
	}
	return 0;
}

int swordCount(int c, int secondOpt){
	if((c>=33&&c<=47) || (c>=58&&c<=64) || 
           (c>=91&&c<=96) || (c>=123&&c<=126)){
		return 1;
	}
	if(secondOpt==0){
		fprintf(stderr, "%c", c);
	}
	return 0;
}

int nwordCount(int c, int secondOpt, int* lastIsNum){
	if(*lastIsNum){
		if(c>=48&&c<=57){
			*lastIsNum = 1;
			return 0;
		}
		*lastIsNum = 0;
	}else{
		if(c>=48&&c<=57){
			*lastIsNum = 1;
			return 1;
		}
	}
	if(secondOpt==0){
		fprintf(stderr, "%c", c);
	}
	return 0;
}

//*isleading==0 means is leading character
//*isleading==1 means is not leading character
int lwordCount(int c, int secondOpt, int* isleading){ 
	if(c=='\n'){
		*isleading = 0;
		if(secondOpt==0){
			fprintf(stderr, "%c", c);
		}
		return 0;
	}
	if(*isleading == 0){
		if(c==' '||c=='\t'||c=='\r'||c=='\v'||c=='\f'){
			*isleading = 1;
			return 1;
		}else{
			*isleading = 2;
			if(secondOpt==0){
				fprintf(stderr, "%c", c);
			}
			return 0;
		}
	}
	else if(*isleading == 1){
		if(c!=' '&&c!='\t'&&c!='\r'&&c!='\v'&&c!='\f'&&secondOpt==0){
			*isleading = 2;
			fprintf(stderr, "%c", c);
		}
	}
	else if(*isleading == 2){
		if(secondOpt==0){
			fprintf(stderr, "%c", c);
		}
	}
	return 0;	
}

int ewordCount(int c, int secondOpt){
	if(c == '\t'){
		int i;
		for(i=0;i<secondOpt;i++){
			fprintf(stderr, " ");
		}
		return 1;
	}
	fprintf(stderr, "%c", c);
	return 0;
}

int cwordCount(int c, int secondOpt, int* numSpace){
	if(c!=' '){
		int i;
		for(i=0;i<*numSpace;i++){
			fprintf(stderr, " ");
		}
		*numSpace = 0;
		fprintf(stderr, "%c", c);
		return 0;
	}else{
		*numSpace += 1;
		if(*numSpace == secondOpt){
			fprintf(stderr, "%c", '\t');
			*numSpace = 0;
			return 1;
		}
		return 0;
	}
}
