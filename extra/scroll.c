
#include <stdio.h>
#include <stdlib.h>



typedef struct rot {
    char * strfull;
    short start;
    short length;
    char swap;
    unsigned long creation;
} rot;

rot * make_rotation(char * msg, int size) {
    rot * result = malloc(sizeof rot);
    result -> strfull = msg;
    result -> start = 0;
    result -> length = size;
    result -> swap




int main(int argc char ** argv) {
    rot * example = make_rotation("hello world", 7);
    while(1) {
        draw_rot(example);
    }
}





