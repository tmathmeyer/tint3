#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "confparse.h"

int main() {
    FILE * fp = fopen("test", "r");
    readblock(fp);
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

block * lookup_block(block_list * from, char * name_query) {
    while(from != NULL) {
        if (!strcmp(name_query, from -> data -> name)) {
            return from -> data;
        }
        from = from -> next;
    }
    return NULL;
}

char * as_hex_string(unsigned int i) {
    printf("%i\n", i);
    char * result = malloc(6);
    int j = 0;
    for(; j < 6; j++) {
        result[5-j] = "0123456789ABCDEF"[i%16];
        printf("%c\n", result[5-j]);
        i = i / 16;
    }
    return result;
}

void do_bar_read(FILE * from, bar_config * storage, block_list * modules) {
    char * name = malloc(100);
    while(fgets(name, 100, from)) {
        int length = strlen(name);

        if (starts_with(name, "  border")) {
            int c = 8;
            int hash = 0;
            storage -> border_color = malloc(6);
            while(name[c++] != '\n') {
                if (!hash && name[c] != ' ') {
                    storage -> border_size *= 10;
                    storage -> border_size += (name[c]-'0');
                } else {
                    (storage -> border_color)[hash++] = name[c];
                }
            }
        }

        if (starts_with(name, "  margin")) {
            int c = 8;
            while(name[c++] != '\n') {
                storage -> margin_size *= 10;
                storage -> margin_size += (name[c]-'0');
            }
        }

        if (starts_with(name, "  background")) {
            int c = 13;
            storage -> background_color = malloc(6);
            while(name[c++] != '\n') {
                (storage -> background_color)[c-14] = name[c];
            }
        }

        if (starts_with(name, "  location")) {
            storage -> location = malloc(10);
            strncpy(storage -> location, name+11, length>10?10:length);
        }

        if (starts_with(name, "  left")) {
            int count;
            sscanf(name, "  left %i", &count);

            for(;count-->0;) {
                fgets(name, 100, from);
                name[strlen(name)-1] = 0;
                block * b = lookup_block(modules, name+4);
                if (b != NULL) {
                    block_list * cur = malloc(sizeof(block_list));
                    cur -> next = storage -> left;
                    cur -> data = b;
                    storage -> left = cur;
                }
            }
        }

        if (starts_with(name, "  center")) {
            int count;
            sscanf(name, "  center %i", &count);

            for(;count-->0;) {
                fgets(name, 100, from);
                name[strlen(name)-1] = 0;
                block * b = lookup_block(modules, name+4);
                if (b != NULL) {
                    block_list * cur = malloc(sizeof(block_list));
                    cur -> next = storage -> center;
                    cur -> data = b;
                    storage -> center = cur;
                }
            }
        }

        if (starts_with(name, "  right")) {
            int count;
            sscanf(name, "  right %i", &count);

            for(;count-->0;) {
                fgets(name, 100, from);
                name[strlen(name)-1] = 0;
                block * b = lookup_block(modules, name+4);
                if (b != NULL) {
                    block_list * cur = malloc(sizeof(block_list));
                    cur -> next = storage -> right;
                    cur -> data = b;
                    storage -> right = cur;
                }
            }
        }


        if (length == 1) {
            return;
        }
    }
}

bar_config * readblock (FILE * fp) {
    block_list * blocks = NULL;
    bar_config * result = NULL;

    char * name = malloc(100);

    while(fgets(name, 100, fp) != NULL) {
        int length = strlen(name);

        if (length == 0 || name[0] == '#') {
            continue;
        }

        name[length-1] = 0;

        if (starts_with(name, "[[bar]]")) {
            result = malloc(sizeof(bar_config));
            do_bar_read(fp, result, blocks);
        }
        else if (name[0] == '[' && name[length-2] == ']') {
            block_list * first    = malloc(sizeof(block_list));
            first -> data         = malloc(sizeof(block));
            first -> data -> name = malloc(length-3);
            strncpy(first -> data -> name, name+1, length-3);
            first -> next = blocks;
            blocks = first;
        }

        if (starts_with(name, "  id ")) {
            blocks -> data -> id = malloc(length-5);
            strncpy(blocks -> data -> id, name+5, length-5);
        }
        if (starts_with(name, "  type ")) {
            blocks -> data -> type = malloc(length-7);
            strncpy(blocks -> data -> type, name+7, length-7);
        }
        if (starts_with(name, "  format ")) {
            blocks -> data -> format = malloc(length-9);
            strncpy(blocks -> data -> format, name+9, length-9);
        }
        if (starts_with(name, "  source ")) {
            blocks -> data -> source = malloc(length-9);
            strncpy(blocks -> data -> source, name+9, length-9);
        }

        if (starts_with(name, "  forground ")) {
            int c = 0;
            for(; c < 6; c++) {
                (blocks -> data -> forground)[c] = name[c+14];
            }
        }

        if (starts_with(name, "  background ")) {
            int c = 0;
            for(; c < 6; c++) {
                (blocks -> data -> background)[c] = name[c+14];
            }
        }
    }

    close(fp);

    return result;
}
