#include "hw3_p1.h"

int intComparator(void* p, void* q) {
    return ((int)p - (int)q);
}

int strComparator(void* str1, void* str2) {
    char* s1 = (char*)str1, *s2 = (char*)str2;
    for (; *s1 && *s2 && *s1 == *s2; ++s1, ++s2){}
    if (*s1 < *s2){
        return -1;
    }
    else if (*s1 > *s2){
        return 1;
    }
    return 0;
}

int idComparator(void *student1, void *student2){
	//Insert Code here
	int student1ID = ((Student*)student1)->id;
	int student2ID = ((Student*)student2)->id;
	if(student1ID < student2ID){return -1;}
	else if(student1ID > student2ID){return 1;}
	else{return 0;}
}	

int nameComparator(void *student1, void *student2){
	//Insert Code here
	struct person n1 = ((Student*)student1)->name;
	struct person n2 = ((Student*)student2)->name;
	
	const char* LN1 = n1.lastName; 
	const char* LN2 = n2.lastName;
	
	int check;

	check = strcmp(LN1, LN2);
	if(check < 0){return -1;}
	else if(check > 0){return 1;}
	
	const char* FN1 = n1.firstName;
	const char* FN2 = n2.firstName;
		
	check = strcmp(FN1, FN2);
	if(check < 0){return -1;}
	else if(check > 0){return 1;}
	
	const char* MN1 = n1.middleName;
	const char* MN2 = n2.middleName;

	check = strcmp(MN1, MN2);
	if(check < 0){return -1;}
	else if(check > 0){return 1;}

	int student1ID = ((Student*)student1)->id;
	int student2ID = ((Student*)student2)->id;

	if(student1ID < student2ID){return -1;}
	else if(student1ID > student2ID){return 1;}
}

int totalHWComparator(void *student1, void *student2){
	//Insert Code here
	int* s1_hw = ((Student*)student1)->hw_score;
	int* s2_hw = ((Student*)student2)->hw_score;

	int s1_hw_total = hw_sum(s1_hw);
	int s2_hw_total = hw_sum(s2_hw);
	
	if(s1_hw_total < s2_hw_total){return -1;}
	else if(s1_hw_total > s2_hw_total){return 1;}

	int student1ID = ((Student*)student1)->id;
	int student2ID = ((Student*)student2)->id;

	if(student1ID < student2ID){return -1;}
	else if(student1ID > student2ID){return 1;}
}

int totalMidtermComparator(void *student1, void *student2){
	//Insert Code Here
	int s1_mscore = ((Student*)student1)->m_scores->m1_score + 
			((Student*)student1)->m_scores->m2_score;
	int s2_mscore = ((Student*)student2)->m_scores->m1_score + 
			((Student*)student2)->m_scores->m2_score;
	
	if(s1_mscore < s2_mscore){return -1;}
	else if(s1_mscore > s2_mscore){return 1;}
	
	int student1ID = ((Student*)student1)->id;
	int student2ID = ((Student*)student2)->id;

	if(student1ID < student2ID){return -1;}
	else if(student1ID > student2ID){return 1;}
}

int totalPtsComparator(void *student1, void *student2){
	//Insert Code Here
	int* s1_hw = ((Student*)student1)->hw_score;
	int* s2_hw = ((Student*)student2)->hw_score;

	int s1_hw_total = hw_sum(s1_hw);
	int s2_hw_total = hw_sum(s2_hw);

	int s1_mscore = ((Student*)student1)->m_scores->m1_score + 
			((Student*)student1)->m_scores->m2_score;
	int s2_mscore = ((Student*)student2)->m_scores->m1_score + 
			((Student*)student2)->m_scores->m2_score;

	int s1_final = ((Student*)student1)->final;
	int s2_final = ((Student*)student2)->final;

	int s1_total = s1_hw_total + s1_mscore + s1_final;
	int s2_total = s2_hw_total + s2_mscore + s2_final;

	if(s1_total < s2_total){return -1;}
	else if(s1_total > s2_total){return 1;}

	int student1ID = ((Student*)student1)->id;
	int student2ID = ((Student*)student2)->id;

	if(student1ID < student2ID){return -1;}
	else if(student1ID > student2ID){return 1;}
}

void printCSVStudentList(List_t *list){
    //Insert Code Here
	int i;
	for(i=0; i<list->length; i++){
		int idNum, hw1, hw2, hw3, midterm1, midterm2, final;
		char* firstName, *lastName, *middleName;
		node_t* n = list->head;
		int j;
		for(j=0; j<i; j++){
			n = n->next;
		}
		Student* s = n->value;
		idNum = s->id;
		firstName = s->name.firstName;
		lastName = s->name.lastName;
		middleName = s->name.middleName;
		hw1 = s->hw_score[0];
		hw2 = s->hw_score[1];
		hw3 = s->hw_score[2];
		midterm1 = s->m_scores->m1_score;
		midterm2 = s->m_scores->m2_score;
		final = s->final;
		printf("%d,%s,%s,%s,%d,%d,%d,%d,%d,%d\n", idNum, firstName, lastName, middleName, hw1, hw2, hw3, midterm1, midterm2, final);
	}
}

