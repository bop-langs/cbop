ifneq ($(TRAVIS_CI), 1)
	CC = gcc
endif

NOOMR_LIB = noomr/libnoomr.a
ABS_NOOMR_LIB = $(realpath $(NOOMR_LIB))
NOOMR_SRC = $(wildcard noomr/*.c)
NOOMR_OBJS = $(NOOMR_SRC:.c=.o)
OBJS =  $(NOOMR_LIB) noomr_hooks.o ary_bitmap.o postwait.o bop_merge.o \
				range_tree/dtree.o bop_ppr.o utils.o external/malloc.o \
				bop_ppr_sync.o bop_io.o bop_ports.o bop_ordered.o libc_overrides.o key_value_checks.o
INCFLAGS = -I$(realpath ./noomr) -I$(realpath .)
CFLAGS_DEF = -Wall -g -fPIC -pthread -Wno-unused-function $(PLATFORM) $(CUSTOMDEF) $(CI_FLAGS) $(INCFLAGS) -march=native
CUSTOMDEF = -D USE_DL_PREFIX -D USE_LOCKS -D UNSUPPORTED_MALLOC
LDFLAGS = -Wl,--no-as-needed -ldl -static -rdynamic
OPITIMIZEFLAGS = -O0
DEBUG_FLAGS = -ggdb3 -g3 -pg -D CHECK_COUNTS
LIB = inst.a
DEPS = $(COBJS:.o=.d)

LIB_SO = inst.a
ABS_LIB = $(realpath $(LIB_SO))

CFLAGS = $(CFLAGS_DEF) $(DEBUG_FLAGS) $(OPITIMIZEFLAGS)

TEST_DIRS = add sleep str/BOP_string str/strsub str/strsub2

library: print_info $(LIB_SO)

%.d: %.c
	@echo "Creating dependency $@"
	@$(CC) $(CFLAGS) -DBOP -MM -o $@ $?
-include $(DEPS)

noomr/libnoomr.a:
	@$(MAKE) -C noomr libnoomr.a

.PHONY: noomr/libnoomr.a

print_info:
	@echo Build info debug build = $(DEBUG)
	@echo cc = $(CC)
	@echo CFLAGS = $(CFLAGS)
	@echo LDFLAGS = $(LDFLAGS)
	@echo OBJS = $(OBJS)
	@echo NOOMR_OBJS = $(NOOMR_OBJS)

$(LIB_SO): $(OBJS) noomr/libnoomr.a
	@echo building archive "$(LIB_SO)"
	ar r $(LIB_SO) $(OBJS)  $(NOOMR_OBJS)
	@ranlib $(LIB_SO)

all: $(OBJS)

%.o: %.c
	@echo compiling $^
	@$(CC) -rdynamic -c -o $@ $^ $(CFLAGS) -DBOP

$(BUILD_DIR)/%.h: %.h
	@cp  $^ $@

define \n


endef

tests: $(LIB_SO)
	$(foreach x,$(TEST_DIRS), @$(MAKE) -C tests/$(x)${\n})

test: tests
	$(foreach x,$(TEST_DIRS), @$(MAKE) -C tests/$(x) test ${\n})

clean:
	@rm -f $(OBJS) $(LIB_SO)
	$(MAKE) -C noomr clean
	$(foreach x,$(TEST_DIRS), @$(MAKE) -C tests/$(x) clean ${\n})

clobber: clean
	@rm -f $(DEPS)
	@$(MAKE) -C noomr clobber

export
