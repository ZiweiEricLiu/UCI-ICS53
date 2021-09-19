#include "hw3_p2.h"
#include <stdio.h>
uint16_t verifyIPV4Checksum(IPV4_header* header) {
    //Insert Code Here
	unsigned short* half_word = (unsigned short*)header;
	int half_word_count = header->header_length*2;
	uint32_t sum=0;
	int i;
	for(i=0; i<half_word_count; i++){
		sum = sum + *(half_word+i);
		if(sum > 0xFFFF){
			sum = (sum&0xFFFF)+1;
		}
	}
	sum = ~sum;
    	return (uint16_t)sum;
}
