#include "concurrency.h"

int readcnt;
sem_t mutex, w;

void snapshot_stats() {
    int i, sum = 0;
    int cfreq[HISTSIZE];
    statsnap s;
    s.n = 0;
    
    // part 2 put your locks here
	P(&mutex);
	readcnt++;
	if(readcnt == 1){
		P(&w);
	}
	V(&mutex);

    s.mode = -1;
    int maxfreq = -1;
    for (i = 0; i < HISTSIZE; i++) {
        s.n += histogram[i];
        cfreq[i] = s.n;
        sum += i * histogram[i];        
        if (maxfreq < histogram[i]) {
            s.mode = i;
            maxfreq = histogram[i];
        }
    }

    // part 2 put your locks here
	P(&mutex);
	readcnt--;
	if(readcnt == 0){
		V(&w);
	}
	V(&mutex);
	
    s.mean = calc_mean_median(sum, s.n, cfreq, &s.median);
    // send stats
    Write(statpipe[1], &s, sizeof(statsnap));
}

void snapshot_histogram() {
    int i;
    histsnap s;
    s.n = 0;
    
    // part 2 put your locks here
    	P(&mutex);
	readcnt++;
	if(readcnt == 1){
		P(&w);
	}
	V(&mutex);


    for (i = 0; i < HISTSIZE; i++) {
        s.n += histogram[i];
        s.hist[i] = histogram[i];
    }
    
    // part 2 put your locks here
    	P(&mutex);
	readcnt--;
	if(readcnt == 0){
		V(&w);
	}
	V(&mutex);

    // send n and histogram   
    Write(histpipe[1], &s, sizeof(histsnap));
}

void *readerwriter_stat_task(void *data) {
    while(1) {
        usleep(READSLEEP);
        snapshot_stats();
    }
    return NULL;
}

void *readerwriter_hist_task(void *data) {
    while(1) {
        usleep(READSLEEP);
        snapshot_histogram();
    }
    return NULL;
}

void start_readers(pthread_t *readers) {
    pthread_create(&readers[0], NULL, readerwriter_stat_task, NULL);
    pthread_create(&readers[1], NULL, readerwriter_hist_task, NULL);

}

void* writer(void* vargp){
	struct dirent* ep = (struct dirent*)vargp;
	int file = Open(ep->d_name, O_RDONLY);
	char c;
	while(Read(file, &c, 1) != 0){
		c -= '0';
		if(c < 0 || c >= HISTSIZE){
			fprintf(stderr, "skipping %c\n", c);
			continue;
		}
		P(&w);
		histogram[(int)c]++;
		V(&w);
	}
	Close(file);
	return NULL;
}

void readerwriter(char *dirname) {
    // start reader threads
    	pthread_t readers[2];
    	start_readers(readers);

    // your code here
    	readcnt = 0;
	Sem_init(&mutex, 0, 1);
	Sem_init(&w, 0, 1);

	Chdir(dirname);
	DIR* dp = Opendir("./");

	struct dirent* ep;

	int count = 4;
	pthread_t* all_tid = Malloc(count*sizeof(pthread_t));
	
	int i=0;

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
		Pthread_create(&all_tid[i], NULL, writer, ep);
		i++;
	}
	int j;
	for(j=0; j<i; j++){
		Pthread_join(all_tid[j], NULL);
	}
	free(all_tid);
	Closedir(dp);

    // remember to cancel the reader threads when you are done processing all the files
	pthread_cancel(readers[0]);
	pthread_cancel(readers[1]);
}
