
include config.mk

all: tint3.o
	@mkdir -p build
	@cc -o build/tint3 source/tint3_all.o main.c ${LDFLAGS}

tests: all
	@cd test && make tests
	@cc -o build/tint3_tests source/tint3_all.o test/tests.o ${LDFLAGS}

tint3.o:
	@cd source && make all

clean:
	@cd source && make clean
	@rm -rf build
