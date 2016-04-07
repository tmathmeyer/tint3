#!/bin/bash
echo "#include \"test.h\"" > .test_tmp.c
echo "#include \".test_tmp.h\"" >> .test_tmp.c
echo "void RUN_TESTS() { " >> .test_tmp.c
echo "#ifndef _SCUT_TMP_H_" > .test_tmp.h
echo "#define _SCUT_TMP_H_" >> .test_tmp.h
for k in $(gcc -E $1 ${@:2} | grep -o '__test_fork__[a-zA-Z0-9_]*')
do
    echo $k >> .test_tmp.c
    echo "char * $k();" >> .test_tmp.h
    echo "();" >> .test_tmp.c
done
echo "}" >> .test_tmp.c
echo "#endif" >> .test_tmp.h

gcc -c $1 -o $1.o ${@:2}
mv $1.o $1.o.o
gcc -c .test_tmp.c -o .test_tmp.o ${@:2}

ld -r $1.o.o .test_tmp.o -o $1.o

rm .test_tmp.c .test_tmp.h $1.o.o .test_tmp.o

