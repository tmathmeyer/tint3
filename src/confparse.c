/*
 * Copyright (C) 2015 Ted Meyer
 *
 * see LICENSING for details
 *
 */

#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "confparse.h"
#define case(in, text) if (match_with_padding((in), (text)))

int starts_with(char *source, char *check) {
    int p = 0;
    for(; check[p] != 0; p++) {
        if (check[p] != source[p]) {
            return 0;
        }
    }
    return 1;
}

int match_with_padding(char *source, char *check) {
    while(*source==' ') {
        source++;
    }
    return starts_with(source, check);
}

void name_blocks(block_list * list) {
    while(list) {
        list = list -> next;
    }
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
    char * result = malloc(6);
    int j = 0;
    for(; j < 6; j++) {
        result[5-j] = "0123456789ABCDEF"[i%16];
        i = i / 16;
    }
    return result;
}

char **parse_options(char *line) {
    while(*line == ' ') {
        line++;
    }
    line += 8;

    int cc = 0;
    int ccc = 0;
    while(line[ccc]) {
        if (line[ccc] == ',') {
            cc++;
        }
        ccc++;
    }
    if (!cc) {
        return NULL;
    }
    char **result = malloc(cc * sizeof(char *));
    char **res = result;
    ccc = 0;
    while(line[ccc]) {
        if (line[ccc] == ',') {
            *res = calloc(ccc+1, sizeof(char));
            memcpy(*res, line, ccc);
            line += (ccc + 1);
            while(*line == ' ') {
                line++;
            }
            ccc = 0;
        }
        ccc++;
    }

    return res;
}

int has_options(char *opt, bar_config *conf) {
    if(conf -> options) {
        int ctr = 0;
        while((conf->options)[ctr]) {
            if(match_with_padding(opt, (conf->options)[ctr])) {
                return 1;
            }
            ctr ++;
        }
    }
    return 0;
}



void do_bar_read(FILE * from, bar_config * storage, block_list * modules) {
    char * name = malloc(100);
    while(fgets(name, 100, from)) {
        int length = strlen(name);

        case(name, "borderwidth") {
            sscanf(name, "  borderwidth %i", &(storage -> border_size));
        }

        case(name, "margin") {
            sscanf(name, "  margin %i", &(storage -> margin_size));
        }

        case(name, "padding") {
            sscanf(name, "  padding %i", &(storage -> padding_size));
        }

        case(name, "options") {
            storage -> options = parse_options(name);
        }

        case(name, "background") {
            int c = 12;
            storage -> background_color = calloc(1, 8);
            while(name[c++] != '\n') {
                (storage -> background_color)[c-14] = name[c-1];
            }
        }

        case(name, "bordercolor") {
            int c = 13;
            storage -> border_color = calloc(1, 8);
            while(name[c++] != '\n') {
                (storage -> border_color)[c-15] = name[c-1];
            }
        }

        case(name, "location") {
            storage -> location = malloc(10);
            strncpy(storage -> location, name+11, length>10?10:length);
        }

        case(name, "left") {
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

        case(name, "center") {
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

        case(name, "right") {
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

block_list *reverse(block_list *blocks) {
    block_list *nres = NULL;
    block_list *tmp;
    while(blocks) {
        tmp = blocks->next;
        blocks->next= nres;
        nres = blocks;
        blocks = tmp;
    }
    return nres;
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
            result -> options = NULL;
            result -> border_size = 0;
            result -> margin_size = 0;
            result -> padding_size = 0;
            do_bar_read(fp, result, blocks);
        }
        else if (name[0] == '[' && name[length-2] == ']') {
            block_list * first    = malloc(sizeof(block_list));
            first -> data         = malloc(sizeof(block));
            first -> data -> name = calloc(1,length);
            name[length-2] = 0;
            sscanf(name, "[%s]", first -> data -> name);
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

        if (starts_with(name, "  forground ") || starts_with(name, "  fontcolor ")) {
            int c = 0;
            (blocks -> data -> forground)[0] = '#';
            (blocks -> data -> forground)[7] = 0;

            for(; c < 6; c++) {
                (blocks -> data -> forground)[c+1] = name[c+13];
            }
        }

        if (starts_with(name, "  background ")) {
            int c = 0;
            (blocks -> data -> background)[0] = '#';
            (blocks -> data -> background)[7] = 0;

            for(; c < 6; c++) {
                (blocks -> data -> background)[c+1] = name[c+14];
            }
        }
    }

    fclose(fp);

    result->center = reverse(result->center);
    result->right = reverse(result->right);
    result->left = reverse(result->left);

    return result;
}
