

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define graphlength 10

int graph[graphlength] = {0};
int max = 0;
int start = 0;
char * map = "▁▂▃▄▅▆▇";

void recalc_max() {
	int ctr = 0, i = 0;
	for(; i < graphlength; i++) {
		if (graph[i] > ctr) {
			ctr = graph[i];
		}
	}
	max = ctr;
}

int add_to_graph(int i) {
	int old = graph[start];
	graph[start] = i;
	if (i > max) {
		max = i;
	}
	if (old == max) {
		recalc_max();
	}
	start = (start+1)%graphlength;
}

char * graph_to_string() {
	char * result = malloc(graphlength*3+1);
	int ctr = 0, i = 0;
	for(; i < graphlength; i++) {
		int val = graph[(start+i)%graphlength]*7/(max+1) + 1;
		result[ctr++] = map[(val-1)*3+0];
		result[ctr++] = map[(val-1)*3+1];
		result[ctr++] = map[(val-1)*3+2];
	}
	printf("%i", max);
	result[graphlength*3] = 0;
	return result;
}



int main() {
	srand (time(NULL));



	while(1) {
		add_to_graph((rand()%10) * (rand()%10));
		char * gr = graph_to_string();
		puts(gr);
		free(gr);

		sleep(1);
	}

	return 0;
}





