#include "helpers.h"

// helper definitions here

hw_sum(int* hw_scores){
	int sum = 0;
	int i;
	for(i=0; i<3; i++){
		sum = sum + hw_scores[i];
	}
	return sum;
}
