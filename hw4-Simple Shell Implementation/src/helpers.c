// Your helper functions need to be here.

#include "helpers.h"

int timeComparator(void* pe1, void* pe2){
	ProcessEntry_t* process1 = (ProcessEntry_t*)pe1;
	ProcessEntry_t* process2 = (ProcessEntry_t*)pe2;
	double dif = difftime(process1->seconds, process2->seconds);
	if(dif < 0){
		return -1;
	}else if(dif > 0){
		return 1;
	}
	return 0;
}

ProcessEntry_t* findPeFromList(List_t* l, int pid){
	node_t* n;
	for(n=l->head; n!=NULL; n=n->next){
		ProcessEntry_t* pe = (ProcessEntry_t*)n->value;
		if(pe->pid == pid){
			return pe;
		}
	}
	return NULL;
}
