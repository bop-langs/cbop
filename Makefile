CC ?= gcc
ifeq ($(CC), cc)
  CC = gcc
endif

CI ?= false
ifeq ($(CI), true)
  CI_FLAGS = -D CI_BUILD
else
  CI_FLAGS = -U CI_BUILD
endif

NOOMR_LIB = noomr/libnoomr.a
_NOOMR_SRC = $(wildcard noomr/*.c)
NOOMR_OBJS = $(_NOOMR_SRC:.c=.o)
OBJS = noomr_hooks.o $(NOOMR_LIB) ary_bitmap.o postwait.o bop_merge.o \
				range_tree/dtree.o bop_ppr.o utils.o external/malloc.o \
				bop_ppr_sync.o bop_io.o bop_ports.o bop_ordered.o libc_overrides.o key_value_checks.o $(NOOMR_OBJS)


CFLAGS_DEF = -Wall -g -fPIC -pthread -I. -Wno-unused-function $(PLATFORM) $(CUSTOMDEF) $(CI_FLAGS) -I./noomr -march=native
CUSTOMDEF = -D USE_DL_PREFIX -D BOP -D USE_LOCKS -D UNSUPPORTED_MALLOC
LDFLAGS = -Wl,--no-as-needed -ldl -static -rdynamic
OPITIMIZEFLAGS = -O0
DEBUG_FLAGS = -ggdb3 -g3 -pg -D CHECK_COUNTS -U NDEBUG
LIB = inst.a

LIB_SO = inst.a

DEBUG ?= 1
ifeq ($(DEBUG), 1)
	CFLAGS = $(CFLAGS_DEF) $(DEBUG_FLAGS)
else
 	CFLAGS = $(CFLAGS_DEF) $(OPITIMIZEFLAGS)
endif

TEST_DIRS = add sleep str/BOP_string str/strsub str/strsub2

library: print_info $(LIB_SO) # $(HEADERS)


noomr/libnoomr.a:
	@$(MAKE) -C noomr libnoomr.a

print_info:
	@echo Build info debug build = $(DEBUG)
	@echo cc = $(CC)
	@echo CFLAGS = $(CFLAGS)
	@echo LDFLAGS = $(LDFLAGS)
	@echo OBJS = $(OBJS)
	@echo NOOMR_OBJS = $(NOOMR_OBJS)

test: library
	for dir in $(TEST_DIRS) ; do \
		echo $$dir ; \
		cd tests/$$dir ; \
		rake ; \
		rake run ; \
		cd -; \
	done

$(LIB_SO): $(OBJS)
	@echo building archive "$(LIB_SO)"
	ar r $(LIB_SO) $(OBJS)
	@ranlib $(LIB_SO)

all: $(OBJS)

%.o: %.c
	@echo compiling $^
	@$(CC) -rdynamic -c -o $@ $^ $(CFLAGS)

$(BUILD_DIR)/%.h: %.h
	@cp  $^ $@

clean:
	rm -f $(OBJS) $(LIB_SO)
	$(MAKE) -C noomr clean
	for dir in $(TEST_DIRS) ; do \
		echo $$dir ; \
		cd tests/$$dir ; \
		rake clean ; \
		cd -; \
	done
