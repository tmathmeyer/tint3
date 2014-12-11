
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "format.h"


int format(int place, char * string) {
	char * addition = "INterest";
	memcpy(string+place, addition, strlen(addition));
	return place + strlen(addition);
}

format_map * getformatmap() {
	format_map * result = malloc(sizeof(format_map));
	result -> next = 0;
	result -> formatter = format;
	result -> formatID = 'T';

	return result;
}


int main() {
	char out[100] = {0};

	format_string(out, "test<%T>test", getformatmap());

	puts(out);
}
