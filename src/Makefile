
include config.mk

all: tint3.o
	@mkdir -p build
	@cc -o build/tint3 source/tint3_all.o ${LDFLAGS}

tint3.o:
	cd source && make all

clean:
	cd source && make clean
	rm -rf build
