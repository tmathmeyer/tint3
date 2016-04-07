# See LICENSE file for copyright and license details.

include ../config.mk

all: tint3

tint3: tint3.o dmap.o mouse.o dlist.o netinfo.o graph.o vdesk.o draw.o system.o lwxt.o confparse.o lwbi.o format.o http.o weather.o json.o popup.o suggest.o

.c.o: config.mk
	@echo CC -c $< ${CFLAGS}
	@${CC} -c $< ${CFLAGS}

tint3 tint3_path:
	@echo ld -r *.o -o $@.o
	@ld -r $+ -o $@_all.o
	@rm $+

clean:
	@rm -f tint3 *.o

update: clean tint3
