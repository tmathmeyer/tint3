# See LICENSE file for copyright and license details.

include config.mk

all: tint3

tint3: tint3.o draw.o

.c.o: config.mk
	@echo CC -c $<
	@${CC} -c $< ${CFLAGS}

tint3 tint3_path:
	@echo CC -o $@
	@${CC} -o $@ $+ ${LDFLAGS}

clean:
	@echo cleaning
	@rm -f tint3 tint3.o draw.o tint3-${VERSION}.tar.gz
