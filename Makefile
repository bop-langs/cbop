FLAGS = -Wall -Wno-unused-function -pthread -g3 -fPIC -I. -save-temps \
		-D USE_DL_PREFIX -D BOP -D USE_LOCKS -D UNSUPPORTED_MALLOC \
		-D DM_DEBUG -D CHECK_COUNTS
LIB = inst.a

CFLAGS_ALL = $(FLAGS) $(CFLAGS)
LDFLAGS_ALL = -Wl,--no-as-needed $(LDFLAGS)
LDLIBS_ALL = -ldl $(LDLIBS)

SRC := $(wildcard *.c) $(wildcard range_tree/*.c) $(wildcard external/*.c)
OBJS = $(SRC:.c=.o)

TEST_DIRS = add sleep str/BOP_string str/strsub str/strsub2 parallel_alloc

.PHONY: all
all: $(LIB)

.PHONY: test
test: $(LIB)
	@for dir in $(TEST_DIRS) ; do \
		echo $$dir ; \
		cd tests/$$dir ; \
		rake ; \
		rake run ; \
		cd -; \
	done

$(LIB): $(OBJS)
	ar r $(LIB) $(OBJS)
	ranlib $(LIB)

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS_ALL) -c -o $@ $<

.PHONY: clean
clean:
	find -type f -name '*.[ios]' -delete
