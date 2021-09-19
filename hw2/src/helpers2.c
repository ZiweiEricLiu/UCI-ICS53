// Define all helper functions for hw1 in this file

int allocFail(char* ptr){
	if(ptr == '\0'){
		return 1;
	}
	return 0;
}

int toupper(int c){
	if(c>=0x61&&c<=0x7A){
		return c-32;
	}
	return c;
}

int contains(char c, char* string, int len){
	int i;
	for(i=0;i<len;i++){
		if(*(string+i)==c){
			return 1;
		}
	}
	return 0;
}

int strLen(char* c){
	int len;
	if(c=='\0'){
		return -1;
	}
	for(len=0; *(c+len)!='\0'; len++);
	return len;
}

void strCopy(char* dest, char* src){
	char* s = src;
	for(;*s!='\0';*dest++=*s++);
}

int strCmp(char* str1, char* str2, int size){
	int i;
	for(i=0;i<size;i++){
		if((*(str1+i) - *(str2+i))!=0){
			return 1;
		}
	}
	return 0;
}
