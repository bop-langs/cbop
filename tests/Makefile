LIBRARY = $(realpath ../inst.a)
LDFLAGS = -lm -Wl,--no-as-needed -ldl -pthread -rdynamic -fPIC

OBJECTS_ORIG = add_orig.o add_mem_test_orig.o add_ppr_only_orig.o big_alloc_orig.o mem_orig.o sleep_orig.o small_alloc_orig.o strcmp_orig.o string_orig.o strsub2_orig.o strsub_orig.o
OBJECTS_BOP = add_bop.o add_mem_test_bop.o add_ppr_only_bop.o big_alloc_bop.o mem_bop.o sleep_bop.o small_alloc_bop.o strcmp_bop.o string_bop.o strsub2_bop.o strsub_bop.o
OBJECTS = $(OBJECTS_ORIG) $(OBJECTS_BOP)
BINARIES_ORIG = add_orig add_mem_test_orig add_ppr_only_orig big_alloc_orig mem_orig sleep_orig small_alloc_orig strcmp_orig string_orig strsub2_orig strsub_orig
BINARIES_BOP = add_bop add_mem_test_bop add_ppr_only_bop big_alloc_bop mem_bop sleep_bop small_alloc_bop strcmp_bop string_bop strsub2_bop strsub_bop
BINARIES = $(BINARIES_ORIG) $(BINARIES_BOP)



default: $(BINARIES) $(OBJECTS)

test:
	pwd
	ls -l
	./test_runner.rb $(BINARIES)
	#$(foreach x,$(BASE), ls -l${\n}./$(x)_orig $(ARGUMENTS)${\n}./$(x)_bop $(ARGUMENTS)${\n})

clean:
	@rm -f $(OBJECTS) $(BINARIES)

%_orig.o: %.c
	@echo compiling original test object $@
	@$(CC) $(CFLAGS) $(LDFLAGS) -c -o $@ $? -lm

%_bop.o: %.c
	@echo compiling test object $@
	@$(CC) $(CFLAGS) $(LDFLAGS) -DBOP -c -o $@ $? -lm


%_orig: %_orig.o $(LIBRARY)
	@echo Linking original test binary $@
	@$(CC) $(CFLAGS) -o $@ $? $(ABS_LIB) $(LDFLAGS)

%_bop: %_bop.o
	@echo Linking bop test binary $@
	@$(CC) $(CFLAGS) -DBOP -o $@ $? $(ABS_LIB) $(LDFLAGS)
