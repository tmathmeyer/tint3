#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "dmap.h"

struct map *map_init(unsigned int size) {
    struct map *result = calloc(sizeof(struct map), 1);
    result->body = calloc(sizeof(struct kvp**), size);
    result->size = size;
    return result;
}

unsigned int hash(char *key) {
    unsigned int result = 0;
    while(*key) {
        result += *key;
        key++;
    }
    return result;
}

char *_copy(char *c) {
    char *res = calloc(1, strlen(c)+1);
    char *r = res;
    while(*c) {
        *(r++) = *(c++);
    }
    return res;
}

void *put(struct map *map, char *key, void *val) {
    key = _copy(key);
    unsigned int h = hash(key);
    struct kvpl *c_list = (map->body)[h % map->size];
    struct kvpl *c_temp = c_list;
    while(c_temp) {
        if (strcmp(key, c_temp->kvp->key)==0) {
            void *old = c_temp->kvp->val;
            c_temp->kvp->val = val;
            return old;
        }
        c_temp = c_temp->next;
    }

    struct kvp *new = malloc(sizeof(struct kvp));
    new->key = key;
    new->val = val;
    
    struct kvpl *newl = malloc(sizeof(struct kvpl));
    newl->kvp = new;
    newl->next = c_list;
    (map->body)[h % map->size] = c_list = newl;

    return NULL;
}

void *map_get(struct map *map, char *key) {
    unsigned int h = hash(key);
    struct kvpl *c_temp = (map->body)[h % map->size];
    while(c_temp) {
        if (strcmp(key, c_temp->kvp->key)==0) {
            return c_temp->kvp->val;
        }
        c_temp = c_temp->next;
    }
    return NULL;
}

struct map *map_new() {
    return map_init(DEFAULT_SIZE);
}

