# See LICENSE file for copyright and license details.

include config.mk

all: tint3

tint3: tint3.o mouse.o netinfo.o graph.o vdesk.o draw.o system.o lwxt.o confparse.o lwbi.o format.o http.o weather.o json.o

.c.o: config.mk
	@echo CC -c $<
	@${CC} -c $< ${CFLAGS}

tint3 tint3_path:
	@echo CC -o $@
	@${CC} -o $@ $+ ${LDFLAGS}

clean:
	@echo cleaning
	@rm -f tint3 *.o

install: tint3
	cp tint3 /usr/bin/tint3

uninstall:
	rm -f /usr/bin/tint3

update: clean tint3
