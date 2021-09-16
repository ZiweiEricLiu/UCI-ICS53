#include "hw3_p1.h"
#include "linkedList.h"

// 53csv implemenation 
int main(int argc, char* argv[]){
	//Insert Code here
	List_t* list = malloc(sizeof(List_t));
	list->length = 0;
	if(argc != 2){
		return -1;
	}
	int c = argv[1][0];
	switch(c){
		case '1':
			list->comparator=idComparator;
			break;
		case '2':
			list->comparator=nameComparator;
			break;
		case '3':
			list->comparator=totalHWComparator;
			break;
		case '4':
			list->comparator=totalMidtermComparator;
			break;
		case '5':
			list->comparator=totalPtsComparator;
			break;
	}
	char student[100];
	char* token;
	char* deli = ",";
	for(;fgets(student,100,stdin);){
		Student* s = malloc(sizeof(Student));
		token = strtok(student, deli);
		s->id = atoi(token);
		token = strtok(NULL, deli);
		s->name.firstName = malloc(strlen(token)*sizeof(char));
		strcpy(s->name.firstName,token);
		token = strtok(NULL, deli);
		s->name.lastName = malloc(strlen(token)*sizeof(char));
		strcpy(s->name.lastName,token);
		token = strtok(NULL, deli);
		if(atoi(token) == 0){
			s->name.middleName = malloc(strlen(token)*sizeof(char));
			strcpy(s->name.middleName,token);
			token = strtok(NULL,deli);
		}else{
			s->name.middleName = malloc(sizeof(char));
			strcpy(s->name.middleName, "");
		}
		s->hw_score[0] = atoi(token);
		token = strtok(NULL, deli);
		s->hw_score[1] = atoi(token);
		token = strtok(NULL, deli);
		s->hw_score[2] = atoi(token);
		token = strtok(NULL, deli);
		Midterms* m = malloc(sizeof(Midterms));
		m->m1_score = atoi(token);
		token = strtok(NULL, deli);
		m->m2_score = atoi(token);
		s->m_scores = m;
		token = strtok(NULL, deli);
		
		s->final = atoi(token);
		insertInOrder(list, s);
	}
	printCSVStudentList(list);
	return 0;
}

