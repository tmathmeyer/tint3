/*
 * Copyright (C) 2014 Ted Meyer
 *
 * see LICENSING for details
 *
 */

#ifndef _JSON_H_
#define _JSON_H_

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

#define $(cont, source) (get(cont -> object, source));
#define _(cont, source) (jndex(cont -> object -> fields, source));

json_object * obj_from_string(char ** string);
json_object * array_from_string(char ** string);
container * from_string (char ** string);
int * number_from_string(char ** string, int isNeg);
char * string_from_string(char ** string);
void free_container(container * container);
void free_json_object(json_object * json);
void free_nfl(named_field_list * nfl);
void forLoop(void (*a)(container *), json_object * json);
void forEach(void (*a)(container *, char *), json_object * json);
container * jndex(named_field_list * nfl, int number);
container * get(json_object * from, char * source);

#endif