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


BUILD_DIR ?= .
_OBJS = malloc_wrapper.o dmmalloc.o ary_bitmap.o postwait.o bop_merge.o \
				range_tree/dtree.o bop_ppr.o utils.o external/malloc.o \
				bop_ppr_sync.o bop_io.o bop_ports.o bop_ordered.o libc_overrides.o key_value_checks.o

CFLAGS_DEF = -Wall -g -fPIC -pthread -I. -Wno-unused-function $(PLATFORM) $(CUSTOMDEF) $(CI_FLAGS)
CUSTOMDEF = -D USE_DL_PREFIX -D BOP -D USE_LOCKS -D UNSUPPORTED_MALLOC
LDFLAGS = -Wl,--no-as-needed -ldl
OPITIMIZEFLAGS = -O3
DEBUG_FLAGS = -ggdb3 -g3 -pg -D CHECK_COUNTS -U NDEBUG
LIB = inst.a

LIB_SO = $(BUILD_DIR)/inst.a

OBJS = $(patsubst %,$(BUILD_DIR)/%,$(_OBJS))
_HEADERS = $(wildcard *.h) $(wildcard external/*.h) $(wildcard range_tree/*.h)
HEADERS = $(patsubst %,$(BUILD_DIR)/%,$(_HEADERS))

DEBUG ?= 1
ifeq ($(DEBUG), 1)
	CFLAGS = $(CFLAGS_DEF) $(DEBUG_FLAGS)
else
 	CFLAGS = $(CFLAGS_DEF) $(OPITIMIZEFLAGS)
endif

TEST_DIRS = add sleep str/BOP_string str/strsub str/strsub2

library: print_info $(LIB_SO) # $(HEADERS)

print_info:
	@echo Build info debug build = $(DEBUG)
	@echo cc = $(CC)
	@echo CFLAGS = $(CFLAGS)
	@echo LDFLAGS = $(LDFLAGS)
	@echo OBJS = $(OBJS)

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
	@ar r $(LIB_SO) $(OBJS)
	@ranlib $(LIB_SO)

all: $(OBJS)

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(@D)
	@echo compiling $^
	@$(CC) -c -o $@ $^ $(CFLAGS)

$(BUILD_DIR)/%.h: %.h
	@mkdir -p $(@D)
	@cp  $^ $@

clean:
	rm -f $(OBJS) $(LIB_SO)
	for dir in $(TEST_DIRS) ; do \
		echo $$dir ; \
		cd tests/$$dir ; \
		rake clean ; \
		cd -; \
	done
