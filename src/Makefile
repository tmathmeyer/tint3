
CC := gcc
CFLAGS := -std=c11 -pedantic -Wextra -Wall -I. -I/usr/include/freetype2
LFLAGS := -lpthread -lX11 -lXinerama -lXft -lXrender -lXrandr \
          -lfreetype -lz -lfontconfig -lxcb -lxcb-icccm -lxcb-ewmh \
		  -lXft

tint3: main.c tint3_main.h baritem.o colors.o confparse.o dlist.o dmap.o \
      format.o graph.o http.o json.o lwbi.o lwxt.o netinfo.o draw.o \
      mouse.o popup.o suggest.o system.o tint3.o vdesk.o weather.o
	@echo $(CC) $<
	@$(CC) $(CFLAGS) -o $@ $< $(filter %.o,$^) $(LFLAGS)

clean:
	rm *.o tint3

baritem.o: baritem/baritem.c baritem/baritem.h dlist.o \
           xftutils/colorset.h graph.o
	@echo $(CC) $<
	@$(CC) $(CFLAGS) -o $@ -c $<

colors.o: colors/colors.c colors/colors.h drawctx/dc.h
	@echo $(CC) $<
	@$(CC) $(CFLAGS) -o $@ -c $<

confparse.o: confparse/confparse.c confparse/confparse.h dlist.o
	@echo $(CC) $<
	@$(CC) $(CFLAGS) -o $@ -c $<

dlist.o: dlist/dlist.c dlist/dlist.h
	@echo $(CC) $<
	@$(CC) $(CFLAGS) -o $@ -c $<

dmap.o: dmap/dmap.c dmap/dmap.h
	@echo $(CC) $<
	@$(CC) $(CFLAGS) -o $@ -c $<

format.o: format/format.c format/format.h
	@echo $(CC) $<
	@$(CC) $(CFLAGS) -o $@ -c $<

graph.o: graph/graph.c graph/graph.h dmap.o xftutils/colorset.h
	@echo $(CC) $<
	@$(CC) $(CFLAGS) -o $@ -c $<

http.o: http/http.c http/http.h
	@echo $(CC) $<
	@$(CC) $(CFLAGS) -o $@ -c $<

json.o: json/json.c json/json.h
	@echo $(CC) $<
	@$(CC) $(CFLAGS) -o $@ -c $<

lwbi.o: lwi/lwbi.c lwi/lwbi.h
	@echo $(CC) $<
	@$(CC) $(CFLAGS) -o $@ -c $<

lwxt.o: lwi/lwxt.c lwi/lwxt.h
	@echo $(CC) $<
	@$(CC) $(CFLAGS) -o $@ -c $<

netinfo.o: netinfo/netinfo.c netinfo/netinfo.h graph.o dlist.o baritem.o
	@echo $(CC) $<
	@$(CC) $(CFLAGS) -o $@ -c $<

draw.o: draw.c draw.h tint3.h colors.o
	@echo $(CC) $<
	@$(CC) $(CFLAGS) -o $@ -c $<

mouse.o: mouse.c mouse.h tint3.h
	@echo $(CC) $<
	@$(CC) $(CFLAGS) -o $@ -c $<

popup.o: popup.c popup.h tint3.h draw.o
	@echo $(CC) $<
	@$(CC) $(CFLAGS) -o $@ -c $<

suggest.o: suggest.c suggest.h tint3.h vdesk.o system.o weather.o \
           vdesk.o netinfo.o
	@echo $(CC) $<
	@$(CC) $(CFLAGS) -o $@ -c $<

system.o: system.c system.h tint3.h lwxt.o lwbi.o draw.o format.o \
          graph.o dlist.o
	@echo $(CC) $<
	@$(CC) $(CFLAGS) -o $@ -c $<

tint3.o: tint3.c tint3.h tint3_main.h dlist.o baritem.o \
         xftutils/colorset.h drawctx/dc.h confparse.o netinfo.o \
         colors.o lwbi.o weather.o suggest.o system.o mouse.o \
         vdesk.o draw.o
	@echo $(CC) $<
	@$(CC) $(CFLAGS) -o $@ -c $<

vdesk.o: vdesk.c vdesk.h tint3.h format.o colors.o draw.o
	@echo $(CC) $<
	@$(CC) $(CFLAGS) -o $@ -c $<

weather.o: weather.c weather.h tint3.h format.o json.o http.o draw.o \
           popup.o tint3.h
	@echo $(CC) $<
	@$(CC) $(CFLAGS) -o $@ -c $<
