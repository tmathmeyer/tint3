
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

struct printlist {
	char * print;
	struct printlist * next;
};
struct printlist * list = NULL;
pthread_mutex_t lock;

void init() {
	pthread_mutex_init(&lock, NULL);
}

void append_list(char * data) {
	struct printlist * next = malloc(sizeof(struct printlist));
	next -> print = data;

	pthread_mutex_lock(&lock);
	next -> next = list;
	list = next;
	pthread_mutex_unlock(&lock);
}

char * readlist() {
	for(;;) {
		pthread_mutex_lock(&lock);
		if (list != NULL) {
			char * result = list -> print;
			list = list -> next;

			pthread_mutex_unlock(&lock);
			return result;
		}
		pthread_mutex_unlock(&lock);
	}
	return NULL;
}

void * print_function(char * data) {
	puts(data);
	free(data);
}

void * thread_function(void * data) {
	int i = 10000;
	while(i-->0) {
		char * value = malloc(128);
		sprintf(value, "testing: %i", i);
		print_function(value);
		usleep(20000);
	}
}






int main() {
	init();

	pthread_t print, thread;

	//pthread_create(&print, NULL, print_function, NULL);

	pthread_create(&thread, NULL, thread_function, NULL);



	//pthread_join(print, NULL);
	pthread_join(thread, NULL);
}
