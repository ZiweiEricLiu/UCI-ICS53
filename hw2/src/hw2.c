// NAME
// netid

#include "morsecode.h"
#include "hw2.h"

char* MorseCode[] = {MorseExclamation, MorseDblQoute, MorseHashtag, Morse$, MorsePercent, MorseAmp, MorseSglQoute, MorseOParen, MorseCParen, MorseStar, MorsePlus, MorseComma, MorseDash, MorsePeriod, MorseFSlash, Morse0, Morse1, Morse2, Morse3, Morse4, Morse5, Morse6, Morse7, Morse8, Morse9, MorseColon, MorseSemiColon, MorseLT, MorseEQ, MorseGT, MorseQuestion, MorseAt, MorseA, MorseB, MorseC, MorseD, MorseE, MorseF, MorseG, MorseH, MorseI, MorseJ, MorseK, MorseL, MorseM, MorseN, MorseO, MorseP, MorseQ, MorseR, MorseS, MorseT, MorseU, MorseV, MorseW, MorseX, MorseY, MorseZ};

/* Part 1 Functions */
int toMorse(FILE *instream, char **mcmsg_ptr){
        // Insert code here
       	int capacity = 40;
        *mcmsg_ptr = malloc(sizeof(char)*capacity);
	if(allocFail(*mcmsg_ptr)){
		return -1;
	}
	*(*mcmsg_ptr) = '\0';
	char c = toupper(fgetc(instream));
	if(c==EOF){
		strCopy((*mcmsg_ptr), "xx");
		return 1;
	}
	int offset=0;
	int lastIsSpace = 0;
	for(;c!=EOF&&c>=0x00&&c<=0x7F;c=toupper(fgetc(instream))){
		if(c>=0x21&&c<=0x5A){
			if(lastIsSpace){
				*(*mcmsg_ptr+offset)='x';
				offset = offset + 1;
			}
			lastIsSpace = 0;
			char* mc = *(MorseCode+c-0x21);
			if(*mc == '\0'){
				continue;
			}
			if(offset < capacity-10){
				strCopy((*mcmsg_ptr+offset), mc);
				offset = offset+strLen(mc);
				*(*mcmsg_ptr+offset)='x';
				offset++;
			}else{
				capacity = capacity*2;
				*mcmsg_ptr = realloc(*mcmsg_ptr, sizeof(char)*capacity);
				if(allocFail(*mcmsg_ptr)){
					return -1;
				}
				strCopy((*mcmsg_ptr+offset),mc);
				offset = offset+strLen(mc);
				*(*mcmsg_ptr+offset)='x';
				offset++;
			}
		}
		else if(c==' '){
			lastIsSpace = 1;
		} 
	}
	char* endOfFile = "x\0";
	strCopy((*mcmsg_ptr+offset),endOfFile);
	offset = offset + 2;
	if(c == EOF){
		return 1;
	}else{
        	return 0;
	}
}

void createKey(char* keyphrase, char* key){
    // Insert code here
	int index;
	int key_count=0;
	for(index=0;keyphrase!='\0'&&*(keyphrase+index)!='\0'&&key_count<26;index++){
		char c = *(keyphrase+index);
		if((c>=0x41&&c<=0x5A)||(c>=0x61&&c<=0x7A)){
			c = toupper(c);
			if(contains(c, key, key_count)){
				continue;	
			}else{
				*(key+key_count)=c;
				key_count++;
			}
		}
	}
	if(key_count<26){
		int remain;
		int i=0;
		for(remain=26-key_count;remain>0;remain=26-key_count){
			char c = 0x41+i;
			if(contains(c, key, key_count)){
				i++;
			}else{
				*(key+key_count) = c;
				key_count++;
				i++;
			}
		}
	}
}

char morseToKey(char* mcmsg, char* key){
    char* FMCarray = ".....-..x.-..--.-x.x..x-.xx-..-.--.x--.-----x-x.-x--xxx..x.-x.xx-.x--x-xxx.xx-";
        // Insert code here
        int index;
	int key_index;
	for(index=0, key_index=0;index<=75;index=index+3, key_index++){
		if(strCmp(mcmsg, FMCarray+index, 3)==0){
			return *(key+key_index);
		}
	}
	return -1;
}

int FMCEncrypt(FILE *instream, char* key, FILE *outstream){
        // Insert code here
	char* mcmsg=NULL;
	int check = toMorse(instream, &mcmsg);
	if(check == -1){
		return -1;
	}
	int offset;
	int len = strLen(mcmsg);
	for(offset=0;offset<=len-3;offset = offset+3){
		char c = morseToKey(mcmsg+offset, key);
		if(c!=-1){
			fprintf(outstream, "%c", c);
		}
	}
	if(check == 0){
		free(mcmsg);
		return 0;
	}
	free(mcmsg);
        return 1;
}

/* Part 2 Functions */
int fromMorse(char *mcmsg, FILE* outstream){
        // Insert code here
       	char* seq = malloc(10);
	int offset = 0;
	int seq_index = 0;
	int lastIsX = 0;
	for(;*(mcmsg+offset)!='\0';offset++){
		if(*(mcmsg+offset) == 'x'&&*(mcmsg+offset+1)!='\0'){
			if(lastIsX){
				fprintf(outstream, " ");
			}else{
				*(seq+seq_index)='\0';
				lastIsX = 1;
				int i;
				int len = strLen(seq);
				int found = 0;
				for(i=0;i<58;i++){
					if(len==strLen(*(MorseCode+i))&&strCmp(seq, *(MorseCode+i), len)==0){
						fprintf(outstream, "%c", i+0x21);
						found = 1;
						break;
					}
				}		
				seq_index = 0;
				if(found==0){
					free(seq);
					return 0;
				}
			}
		}
		else if(*(mcmsg+offset)!='x'){
			lastIsX = 0;
			*(seq+seq_index) = *(mcmsg+offset);
			seq_index++;
		}
	}
	if(seq_index != 0){
		*(seq+seq_index)='\0';
		int i;
		int len = strLen(seq);
		int found = 0;
		for(i=0;i<58;i++){
			if(len==strLen(*(MorseCode+i))&&strCmp(seq, *(MorseCode+i), len)==0){
				fprintf(outstream, "%c", i+0x21);
				found = 1;
				break;
			}
		}		
		seq_index = 0;
		if(found==0){
			free(seq);
			return 0;
		}
	}
	free(seq);
        return 1;
}

int FMCDecrypt(FILE *instream, char* key, FILE *outstream){
        // Insert code here
	char* FMCarray = ".....-..x.-..--.-x.x..x-.xx-..-.--.x--.-----x-x.-x--xxx..x.-x.xx-.x--x-xxx.xx-";
       	int capacity = 40;
	char* mcmsg = malloc(sizeof(char)*capacity);
	int offset = 0;
	if(allocFail(mcmsg)){
		return -1;
	}
	*mcmsg = '\0';
	char c;
	for(c=fgetc(instream);c!=EOF&&c>=0x00&&c<=0x7F;c=fgetc(instream)){
		if(c>=0x41&&c<=0x5A){
			if(offset > capacity-10){
				capacity = capacity*2;
				mcmsg = realloc(mcmsg, sizeof(char)*capacity);
				if(allocFail(mcmsg)){
					return -1;
				}
			}
			int i;
			for(i=0;i<26;i++){
				if(c==*(key+i)){
					int index = i*3;
					*(mcmsg+offset)=*(FMCarray+index);
					*(mcmsg+offset+1)=*(FMCarray+index+1);
					*(mcmsg+offset+2)=*(FMCarray+index+2);
					offset = offset+3;
				}
			}
		}
	}
	if(c==EOF){
		*(mcmsg+offset)='\0';
		int check = fromMorse(mcmsg, outstream);
		if(check == 0){
			return 0;
		}
		free(mcmsg);
		return 1;
	}
        return 0;
}

