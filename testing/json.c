#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct json_object {
    struct named_field_list * fields;
} json_object;

typedef struct container {
    int id;
    union {
        char * string;
        int  * number;
        int  * boolean;
        json_object * object;
    };
} container;

typedef struct named_field_list {
    char * name;
    container * field;
    struct named_field_list * next;
} named_field_list;

json_object * obj_from_string(char ** string);
json_object * array_from_string(char ** string);
int * number_from_string(char ** string);
char * string_from_string(char ** string);
void free_container(container * container);
void free_json_object(json_object * json);
void free_nfl(named_field_list * nfl);

container * get_by_name(named_field_list * nfl, char * name) {
    if (nfl) {
        if (strncmp(nfl -> name, name, strlen(nfl -> name))) {
            return get_by_name(nfl -> next, name);
        }
        return nfl -> field;
    }
    return 0;
}

container * get_by_number(named_field_list * nfl, int number) {
    if (nfl) {
        if (number) {
            return get_by_number(nfl -> next, number -1);
        }
        return nfl -> field;
    }
    return 0;
}

container * get(json_object * from, char * source) {
    if (from) {
        return get_by_name(from -> fields, source);
    }
    return 0;
}

container * from_string (char ** string) {
    container * result = malloc(sizeof(container));
    if (**string == '{') {
        (*string)++;
        result -> object = obj_from_string(string);
        result -> id = 3;
    } else if (**string>='0' && **string<='9') {
        result -> number = number_from_string(string);
        result -> id = 1;
    } else if (**string == '-') {
        (*string)++;
    	result -> number = number_from_string(string);
    	*(result -> number) *= -1;
        result -> id = 1;
    } else if (**string == '[') {
        (*string)++;
        result -> object = array_from_string(string);
        result -> id = 3;
    } else if (**string == 't') {
    	result -> boolean = malloc(sizeof(int));
        *(result -> boolean) = 1;
        result -> id = 2;
        (*string)+=4;
    } else if (**string == 'f') {
  		result -> boolean = malloc(sizeof(int));
        *(result -> boolean) = 0;
        result -> id = 2;
        (*string)+=5;
    } else if (**string == '"') {
  		(*string)++;
        result -> string = string_from_string(string);
        result -> id = 0;
    }
    while(**string==' ') {
        (*string)++;
    }
    return result;
}

int * number_from_string(char ** string) {
    int counter = 0;
    int resres = 0;
    for(; (**string>='0') && (**string<='9'); (*string)++) {
        resres *= 10;
        resres += (**string-'0');
    }
    if (**string=='.') {
    	(*string)++;
    	for(; (**string>='0') && (**string<='9'); (*string)++);
    }
    int * result = malloc(sizeof(int));
    *result = resres;
    return result;
}

json_object * obj_from_string(char ** string) {
    json_object * result = malloc(sizeof(json_object));
    result -> fields     = NULL;

    while(**string!='}') {
        char buffer[100] = {0};
        int pos = 0;
        int strip = (**string == '"');
        for(;**string!=':';(*string)++) {
            buffer[pos++] = **string;
        }
        string[0]++;
        while(**string==' ') {
            (*string)++;
        }
        named_field_list * next = malloc(sizeof(named_field_list));
        next -> name = malloc(pos+1);
        memcpy(next -> name, buffer+strip, pos+1-(3*strip));
        next -> field = from_string(string);
        next -> next = result -> fields;
        result -> fields = next;

        if (!(**string==',' || **string=='}') && **string != 0) {
            return 0;
        }
        if (**string==',') {
            (*string)++;
            while(**string==' ') {
                (*string)++;
            }
        }
    }
    (*string)++;
    return result;
}

char * string_from_string(char ** string) {
    int length = 0;
    for(;(*string)[length]!='"';length++) {
    	if ((*string)[length]=='\'') {
    		length++;
    	}
    }

    char * result = malloc(length+1);
    memcpy(result, *string, length);
    result[length+1] = 0;
    (*string)+=(length+1);
    return result;
}

json_object * array_from_string(char ** string) {
    json_object * result = malloc(sizeof(json_object));
    named_field_list * master = malloc(sizeof(named_field_list));
    named_field_list * update = master;

    while(**string!=']') {
        update -> next = malloc(sizeof(named_field_list));
        update = update -> next;
        update -> name = 0;
        update -> field = from_string(string);
        update -> next = result -> fields;

        if (!(**string==',' || **string==']')) {
            return 0;
        }
        if (**string==',') {
            (*string)++;
            while(**string==' ') {
                (*string)++;
            }
        }
    }

    result -> fields = master -> next;
    free(master);
    (*string)++;
    return result;
}


void free_container(container * container) {
	if (container) {
		switch(container -> id) {
			case 0: case 1: case 2:
			    if (container -> string) {
			    	free(container -> string);
			    }
			    break;
			case 3:
				free_json_object(container -> object);
		}
		free(container);
	}
}

void free_json_object(json_object * json) {
	if (json) {
		if (json -> fields) {
			free_nfl(json -> fields);
		}
		free(json);
	}
}

void free_nfl(named_field_list * nfl) {
	while(nfl) {
		named_field_list * old = nfl;
		nfl = nfl -> next;
		if (old -> name) {
			free(old -> name);
		}
		if (old -> field) {
			free_container(old -> field);
		}
		free(old);
	}
}

void forLoop(void (*a)(container *), json_object * json) {
	named_field_list * cur = json -> fields;
	while(cur) {
		a(cur -> field);
		cur = cur -> next;
	}
}

void forEach(void (*a)(container *, char * name), json_object * json) {
	named_field_list * cur = json -> fields;
	while(cur) {
		a(cur -> field, cur -> name);
		cur = cur -> next;
	}
}