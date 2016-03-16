#include <stdio.h>
#define SETBLOCK(x, c) do{block*__b__=(x);(c);}while(0)
#define ID(z) if(!_strcmp(__b__->id, (#z)))


char _strcmp(const char *A, const char *B) {
    int i = 0;
    while(1) {
        char X = A[i];
        char Y = B[i];
        if (X == 0 && Y == 0) {
            return 0;
        }
        if (X != Y) {
            return X-Y;
        }
        i++;
    }
}


typedef struct block {
    char *id;
} block;

void infer(block *conf_inf) {

    SETBLOCK(conf_inf, {
        ID(workspace) {
            puts("workspace");
        }
        else ID(graph) {
            puts("graph");
        }
        else ID(magic) {
            puts("magic");
        }
        else ID(fuck) {
            puts("fuck");
        }
    });

}

void main() {
    block id;
    
    id.id = "workspace";
    infer(&id);
    
    id.id = "graph";
    infer(&id);

    id.id = "magic";
    infer(&id);

    id.id = "fuck";
    infer(&id);
}
