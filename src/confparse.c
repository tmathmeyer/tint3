

#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dlist.h"
#include "confparse.h"

char *leading(char *line) {
    while(line && *line == ' ') {
        line++;
    }
    return line;
}

char firstchar(char *line) {
    line = leading(line);
    return line?*line:0;
}

int starts_with(char *source, char *check) {
    int p = 0;
    for(; check[p] != 0; p++) {
        if (check[p] != source[p]) {
            return 0;
        }
    }
    return 1;
}

dlist *intern_lines(FILE *filein) {
    dlist *lines = dlist_new();
    char line[100] = {0};
    while(fgets(line, 100, filein)) {
        int size = strlen(line);
        if (size>1) {
            if (line[0] != '#') {
                char *copy = malloc(size+1);
                memcpy(copy, line, size+1);
                memset(line, 0, size);
                dlist_add(lines, copy);
            }
        }
    }

    return lines;
}

dlist *itemize(dlist *lines) {
    dlist *items = dlist_new();
    char *line;

    dlist *block = NULL;
    each(lines, line){
        if (firstchar(line) == '[') {
            if (block) {
                dlist_add(items, block);
            }
            block = dlist_new();
        }
        if (block) {
            dlist_add(block, line);
        }
    }
    if (block) {
        dlist_add(items, block);
    }

    return items;
}

int first_space(char *str) {
    int ctr = 0;
    while(str && str[ctr] != ' ') {
        ctr++;
    }
    return ctr;
}

entry *get_map_entry(char *line) {
    int space=0, ctr=0;
    while(line[ctr]) {
        if (!space && line[ctr] == ' ') {
            space = ctr;
        }
        ctr++;
    }
    char *key = calloc(space+1, 1);
    char *value = calloc(ctr-space-1, 1);

    memcpy(key, line, space);
    memcpy(value, line+space+1, ctr-space-2);

    entry *result = malloc(sizeof(entry));
    result->key = key;
    result->value =value;

    return result;
}

block *as_block(dlist *chunk) {
    block *res = malloc(sizeof(block));
    res->id = NULL;
    res->name = NULL;
    res->type = NULL;
    res->format = NULL;
    res->source = NULL;
    res->map = dlist_new();
    
    char *elem;
    each(chunk, elem) {
        char *line = leading(elem);
        int size = strlen(line);
        int mem_subtract;
        int cpy_start;
        int cpy_end;
        char **write_to;
        //char *buf;

        if(starts_with(line, "[")) {
            mem_subtract = 2;
            cpy_start = 1;
            cpy_end = 3;
            write_to = &(res->name);
        } else {
            mem_subtract = first_space(line);
            cpy_start = mem_subtract+1;
            cpy_end = mem_subtract+2;
            if(starts_with(line, "id")) {
                write_to = &(res->id);
            } else if(starts_with(line, "format")) {
                write_to = &(res->format);
            } else if(starts_with(line, "type")) {
                write_to = &(res->type);
            } else if(starts_with(line, "source")) {
                write_to = &(res->source);
            } else if(starts_with(line, "forground")) {
                write_to = &(res->forground);
            } else if(starts_with(line, "background")) {
                write_to = &(res->background);
            } else {
                dlist_add(res->map, get_map_entry(line));
                write_to = 0;
            }
        }

        if (write_to) {
            (*write_to) = calloc(size-mem_subtract, 1);
            memcpy(*write_to, line+cpy_start, size-cpy_end);
        }
    }

    return res;
}

block *get_block(dlist *blocks, char *name){
    block *result;
    each(blocks, result) {
        if (result->name) {
            if (strcmp(result->name, name) == 0) {
                return result;
            }
        }
    }
    return NULL;
}

dlist *parse_options(char *from){
    int indx = 0;
    char *tmp = NULL;
    dlist *result = dlist_new();
    while(from[indx]) {
        if (from[indx] == ',' || from[indx] == '\n') {
            if (indx > 0) {
                tmp = calloc(indx, 1);
                memcpy(tmp, from, indx);
                from = leading(from+indx+1);
                dlist_add(result, tmp);
            }
        }
        indx++;
    }
    if (indx > 0) {
        tmp = calloc(indx, 1);
        memcpy(tmp, from, indx-1);
        dlist_add(result, tmp);
    }
    return result;
}

#define match(str) if (starts_with(line, (str)))
bar_config *as_bar(dlist *src, dlist *blocks) {
    bar_config *config = malloc(sizeof(bar_config));
    config -> border_size = 0;
    config -> padding_size = 0;
    config -> margin_size = 0;
    config -> location = TOP;
    config -> border_color = NULL;
    config -> background_color = NULL;
    config -> left = dlist_new();
    config -> center = dlist_new();
    config -> right = dlist_new();

    dlist **buffer = NULL;
    char *full_line;
    char *line;

    each(src, full_line) {
        line = leading(full_line);
        match("borderwidth") {
            config->border_size = atoi(line+12);
        }

        else match("padding") {
            config->padding_size = atoi(line+8);
        }

        else match("margin") {
            config->margin_size = atoi(line+7);
        }

        else match("location") {
            config->location = starts_with(line+9, "top") ?
                TOP : BOTTOM;
        }

        else match("bordercolor") {
            config->border_color = calloc(8, 1);
            memcpy(config->border_color, line+12, 7);
        }
        
        else match("background") {
            config->background_color = calloc(8, 1);
            memcpy(config->background_color, line+11, 7);
        }
        
        else match("left") {
            buffer = &(config->left);
        }
        
        else match("center") {
            buffer = &(config->center);
        }
        
        else match("right") {
            buffer = &(config->right);
        }

        else match("options") {
            config->options = parse_options(line+8);
            buffer = NULL;
        }

        else if (buffer) {
            line[strlen(line)-1] = 0;
            block *named = get_block(blocks, line);
            if (named) {
                dlist_add(*buffer, named);
            }
        }

        free(full_line);
    }

    return config;
}

int is_bar(dlist *block) {
    char *txt = leading((block->data)[0]);
    return starts_with(txt, "[[");
}

int is_block(dlist *block) {
    char *txt = leading((block->data)[0]);
    return starts_with(txt, "[");
}

bar_config *build_bar_config(FILE *rc) {
    dlist *lines = intern_lines(rc);
    dlist *chunks = itemize(lines);
    dlist *blocks = dlist_new();
    dlist *bar_chunk = NULL;
    
    char *freeme;
    dlist *iterate;

    each(chunks, iterate) {
        if (is_bar(iterate)) {
            bar_chunk = iterate;
        } else if (is_block(iterate)) {
            block *block = as_block(iterate);
            dlist_add(blocks, block);
            
            each(iterate, freeme) {
                free(freeme);
            }
            free(iterate);
        }
    }
    free(chunks);
    free(lines);

    return as_bar(bar_chunk, blocks);
}

int has_options(char *opt, bar_config *conf) {
    char *match;

    if (!conf->options) {
        return 0;
    }
    if (size(conf->options) == 0) {
        return 0;
    }

    each(conf->options, match) {
        if (!strcmp(opt, match)) {
            return 1;
        }
    }

    return 0;
}
