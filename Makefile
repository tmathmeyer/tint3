include config.mk



dependancies := $(wildcard deps/*)
dependancies_target := $(patsubst deps/%, \
	deps/%/build/exported, $(dependancies))
dependancies_links := $(patsubst deps/%, \
	deps/%/build/library, $(dependancies))
dependancies_clean := $(patsubst deps/%, \
	clean/recurse/%, $(dependancies))
dependancies_unlink := $(patsubst deps/%, \
	unlink/%, $(dependancies))

c_source_files := $(wildcard src/C/*.c)
compiled_source_files := $(patsubst src/C/%.c, \
	build/obj/%.o, $(c_source_files))


build: copy_exported library export_headers $(dependancies_unlink)

#build a binary if you can
binary: build
#	@echo "$(shell pwd) binary"
	@mkdir -p build/binary
	@gcc -o build/binary/$(project) build/library/*.o $(link_libs)


#build staticly linkable object file
library: $(compiled_source_files)
#	@echo "$(shell pwd) library"
	@mkdir -p build/library
	@ld -o build/library/$(project).o -r $^
build/obj/%.o: src/C/%.c
	@mkdir -p build/obj
	@gcc -c $< -o $@ $(CFLAGS)
ifdef CFLAGS_PRINT
	@echo "CC -c $< -o $@ $(CFLAGS_PRINT)"
endif



# copy in dependancies headers
FORCE:

deps/%/build/exported: deps/% FORCE
	@mkdir -p build/exported
	@ln -fs ../../$@ src/C/$(shell basename $<)
	@ln -fs ../../$@ build/exported/$(shell basename $<)
deps/%/build/library: deps/%
	@mkdir -p build/library
	@$(MAKE) -C $< build
	@cp $@/*.o build/library/
copy_exported: $(dependancies_target) $(dependancies_links)
#	@echo "$(shell pwd) copy exported"



# export header files
C/%.h: 
	@mkdir -p build/exported
	@cp src/$@ build/exported/
export_headers: $(exported)
#	@echo "$(shell pwd) export headers"



# recursively clean all dependancies
unlink/%:
	@rm -f src/C/$(shell basename $@)
clean/recurse/%:
	@$(MAKE) -C deps/$(shell basename $@) clean
clean: $(dependancies_clean) $(dependancies_unlink)
	@rm -rf build
