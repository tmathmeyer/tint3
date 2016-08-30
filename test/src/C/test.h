#ifndef _test_h_
#define _test_h_
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>

#define testcase __attribute((constructor))

#define START_TEST(name) \
    char *__test__##name(); \
    void testcase __test__call__##name() { \
        printf("====" #name "====\n"); \
        char *__test__call = __test__##name(); \
        if (__test__call) { \
            printf("\x1B[31m%s\x1B[0m\n", __test__call); \
        } else { \
            printf("\x1B[32m   test passed\x1B[0m\n"); \
        } \
        printf("====" #name "====\n\n"); \
        return; \
    } \
    char *__test__##name() 


#define ASSERT_REF(a, b, fail) do{if(a!=b){return fail;}}while(0)
#define ASSERT(test, fail) do{if(!test){return fail}}while(0)
#define ASSERT_SUCCESS() do{return NULL;}while(0)
#define ASSERT_STR(a, b, fail) \
    do{if(strcmp(a, b)){return fail;}}while(0)

void RUN_TESTS();

#endif
