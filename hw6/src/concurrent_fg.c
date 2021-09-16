#include "concurrency.h"

sem_t mutexs[HISTSIZE];

void* thread_fg(void* vargp){
	struct dirent* ep = (struct dirent*)vargp;
	int file = Open(ep->d_name, O_RDONLY);
	char c;
	while(Read(file, &c, 1) != 0){
		c -= '0';
		if(c < 0 || c >= HISTSIZE){
			fprintf(stderr, "skipping %c\n", c);
			continue;
		}
		sem_t* mutex = &mutexs[(int)c];
		P(mutex);
		histogram[(int)c]++;
		V(mutex);
	}
	Close(file);
	
	return NULL;
}

void concurrent_fg(char *dirname) {
    // your code here
	int i;
	for(i=0; i<HISTSIZE; i++){
		Sem_init(&(mutexs[i]), 0, 1);
	}

	Chdir(dirname);
	DIR* dp = Opendir("./");

	struct dirent* ep;

	int count = 4;
	pthread_t* all_tid = Malloc(count*sizeof(pthread_t));

	i=0;

	while((ep = readdir(dp)) != NULL){
		struct stat sb;
		Stat(ep->d_name, &sb);
		if((sb.st_mode & S_IFMT) != S_IFREG){
			continue;
		}

		//int fd = Open(ep->d_name, O_RDONLY);
		
		if(i >= count){
			count = count*2;
			all_tid = Realloc(all_tid, count*sizeof(pthread_t));
		}
		Pthread_create(&all_tid[i], NULL, thread_fg, ep);
		i++;
	}
	int j;
	for(j=0; j<i; j++){
		Pthread_join(all_tid[j], NULL);
	}
	free(all_tid);
	Closedir(dp);
}
