#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char * id;
    char * name;
    char * type;
    char * format;
    char * source;
    char forground [6];
    char background [6];
} block;

typedef struct l {
    struct l * next;
    block    * data;
} block_list;

bar_config * readblock(FILE * fp);

typedef struct {
    int border_size;
    int margin_size;
    char * border_color;
    char * background_color;
    char * location;
    block_list * left;
    block_list * center;
    block_list * right;
} bar_config;


int main() {
    FILE * fp = fopen("test", "r");
    print_block_list (readblock(fp));
}

int starts_with(char * source, char * check) {
    int p = 0;
    for(; check[p] != 0; p++) {
        if (check[p] != source[p]) {
            return 0;
        }
    }
    return 1;
}


bar_config * readblock (FILE * fp) {
    block_list * result = NULL;


    char * name = malloc(100);

    while(fgets(name, 100, fp) != NULL) {
        int length = strlen(name);

        if (length == 0 || name[0] == '#') {
            continue;
        }

        name[length-1] = 0;

        if (starts_with(name, "[[bar]]")) {
            
        }
        else if (name[0] == '[' && name[length-2] == ']') {
            block_list * first    = malloc(sizeof(block_list));
            first -> data         = malloc(sizeof(block));
            first -> data -> name = malloc(length-3);
            strncpy(first -> data -> name, name+1, length-3);
            first -> next = result;
            result = first;
        }

        if (starts_with(name, "  id ")) {
            result -> data -> id = malloc(length-5);
            strncpy(result -> data -> id, name+5, length-5);
        }
        if (starts_with(name, "  type ")) {
            result -> data -> type = malloc(length-7);
            strncpy(result -> data -> type, name+7, length-7);
        }
        if (starts_with(name, "  format ")) {
            result -> data -> format = malloc(length-9);
            strncpy(result -> data -> format, name+9, length-9);
        }
        if (starts_with(name, "  source ")) {
            result -> data -> source = malloc(length-9);
            strncpy(result -> data -> source, name+9, length-9);
        }

        if (starts_with(name, "  forground ")) {
            int c = 0;
            for(; c < 6; c++) {
                (result -> data -> forground)[c] = name[c+14];
            }
        }

        if (starts_with(name, "  background ")) {
            int c = 0;
            for(; c < 6; c++) {
                (result -> data -> background)[c] = name[c+14];
            }
        }
    }

    close(fp);

    return result;
}
