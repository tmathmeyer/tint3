/*
 * Copyright (C) 2014 Ted Meyer
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "confparse.h"

int starts_with(char * source, char * check) {
    int p = 0;
    for(; check[p] != 0; p++) {
        if (check[p] != source[p]) {
            return 0;
        }
    }
    return 1;
}

void name_blocks(block_list * list) {
    while(list) {
        printf("found (%s)\n", list -> data -> name);
        list = list -> next;
    }
}

block * lookup_block(block_list * from, char * name_query) {
    while(from != NULL) {
        if (!strcmp(name_query, from -> data -> name)) {
            printf("found (%s)\n", name_query);
            return from -> data;
        }
        from = from -> next;
    }
    printf("cant find (%s)\n", name_query);
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

        if (starts_with(name, "  borderwidth")) {
            sscanf(name, "  borderwidth %i", &(storage -> border_size));
        }

        if (starts_with(name, "  margin")) {
            sscanf(name, "  margin %i", &(storage -> margin_size));
        }

        if (starts_with(name, "  padding")) {
            sscanf(name, "  padding %i", &(storage -> padding_size));
        }

        if (starts_with(name, "  background")) {
            int c = 12;
            storage -> background_color = calloc(0, 8);
            while(name[c++] != '\n') {
                (storage -> background_color)[c-14] = name[c-1];
            }
        }

        if (starts_with(name, "  bordercolor")) {
            int c = 13;
            storage -> border_color = calloc(0, 8);
            while(name[c++] != '\n') {
                (storage -> border_color)[c-15] = name[c-1];
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
            result -> border_size = 0;
            result -> margin_size = 0;
            result -> padding_size = 0;
            do_bar_read(fp, result, blocks);
        }
        else if (name[0] == '[' && name[length-2] == ']') {
            block_list * first    = malloc(sizeof(block_list));
            first -> data         = malloc(sizeof(block));
            first -> data -> name = calloc(0,length);
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

        if (starts_with(name, "  forground ")) {
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

    return result;
}
