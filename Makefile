CC ?= gcc
ifeq ($(CC), cc)
  CC = gcc
endif


CFLAGS = -Wall -g -fPIC -pthread -I. -Wno-unused-function -save-temps $(CUSTOMDEF)
CUSTOMDEF = -D USE_DL_PREFIX -D BOP -D USE_LOCKS -D UNSUPPORTED_MALLOC -D DM_DEBUG
LDFLAGS = -Wl,--no-as-needed -ldl
OPITIMIZEFLAGS = -O3
DEBUG_FLAGS = -ggdb3 -g3 -pg -D CHECK_COUNTS -U NDEBUG
LIB = inst.a


SRC := $(wildcard *.c) $(wildcard external/*.c) $(wildcard range_tree/*.c)
OBJS = $(SRC:.c=.o)


TEST_DIRS = add sleep str/BOP_string str/strsub str/strsub2 parallel_alloc

library: print_info $(LIB)

print_info:
	@echo cc = $(CC)
	@echo CFLAGS = $(CFLAGS)
	@echo LDFLAGS = $(LDFLAGS)
	@echo OBJS = $(OBJS)


test: library
	@for dir in $(TEST_DIRS) ; do \
		echo $$dir ; \
		cd tests/$$dir ; \
		rake ; \
		rake run ; \
		cd -; \
	done

$(LIB): $(OBJS)
	@echo building archive "$(LIB)"
	@ar r $(LIB) $(OBJS)
	@ranlib $(LIB)

all: $(OBJS)

# -include $(OBJS:.o=.d)

%.o: %.c
	@echo compiling $^
	@gcc -MM $(CFLAGS) $^ > $*.d
	@gcc -c -o $@ $^ $(CFLAGS)

clean:
	rm -f $(OBJS) $(LIB) *.d *.d.tmp *.[ios]
	for dir in $(TEST_DIRS) ; do \
		echo $$dir ; \
		cd tests/$$dir ; \
		rake clean ; \
		cd -; \
	done
