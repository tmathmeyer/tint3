
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct rot {
    char * strfull;
    short length;
    short start;
    short size;
    char swap;
    clock_t creation;
    clock_t speed;
} rot;

void swap(char * A, char * B) {
    *A ^= *B;
    *B ^= *A;
    *A ^= *B;
}

rot * make_rotation(char * msg, int size) {
    rot * result = malloc(sizeof(rot));
    result -> length = strlen(msg) + size;
    result -> start = size;
    result -> size = size;
    result -> swap = 0;

    result -> speed = 500000;

    result -> strfull = malloc(result -> length + size + size);
    memset(result -> strfull, ' ', result -> length + size);
    strncpy(result -> strfull + size, msg, result -> length);
    (result -> strfull)[result -> length + size - 1] = 0;

    //time((time_t *) (&(result -> creation)));
    result -> creation = clock();

    swap(&(result -> swap), (char *)((result -> strfull) + (result -> start) + (result -> size)));

    return result;
}

int update_rot(rot * rotation) {
    if (clock()-(rotation->creation) > rotation -> speed) {
        rotation -> creation = clock();
        swap(&(rotation -> swap), (char *)((rotation -> strfull) + (rotation -> start) + (rotation -> size)));
        rotation -> start = (rotation -> start + 1) % (rotation -> length);
        swap(&(rotation -> swap), (char *)((rotation -> strfull) + (rotation -> start) + (rotation -> size)));
        return 1;
    }
    return 0;
}

int next = 1;
void draw_rot(rot * drawme) {
    if (next)printf("%s\n", (drawme -> strfull) + (drawme -> start));
    next = update_rot(drawme);
}

int main(int argc, char ** argv) {
    rot * example = make_rotation("This is a test of the rotation system!!", 40);
    while(1) {
        draw_rot(example);
    }
}





