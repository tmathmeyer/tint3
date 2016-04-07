
include config.mk

all: tint3.o
	@mkdir -p build
	@cc -o build/tint3 src/tint3_all.o ${LDFLAGS}

tint3.o:
	cd src && make all

clean:
	cd src && make clean
	rm -rf build
