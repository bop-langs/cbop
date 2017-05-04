ifneq ($(TRAVIS_CI), 1)
	CC = gcc
endif

IPA_LIB = ipa/libipa.a
ABS_IPA_LIB = $(realpath $(IPA_LIB))
IPA_SRC = $(wildcard ipa/*.c)
IPA_OBJS = $(IPA_SRC:.c=.o)
OBJS =  $(IPA_LIB) ipa_hooks.o ary_bitmap.o postwait.o bop_merge.o \
				range_tree/dtree.o bop_ppr.o utils.o external/malloc.o \
				bop_ppr_sync.o bop_io.o bop_ports.o bop_ordered.o libc_overrides.o key_value_checks.o
INCFLAGS = -I$(realpath ./ipa) -I$(realpath .)
CFLAGS_DEF = -Wall -g -fPIC -pthread -Wno-unused-function $(PLATFORM) $(CUSTOMDEF) $(CI_FLAGS) $(INCFLAGS) -march=native
CUSTOMDEF = -D USE_DL_PREFIX -D USE_LOCKS -D UNSUPPORTED_MALLOC
LDFLAGS = -Wl,--no-as-needed -ldl -static -rdynamic
OPITIMIZEFLAGS = -O0
DEBUG_FLAGS = -ggdb3 -g3 -pg -D CHECK_COUNTS
LIB = inst.a
DEPS = $(COBJS:.o=.d)

LIB_SO = libcbop.a
ABS_LIB = $(realpath $(LIB_SO))

CFLAGS = $(CFLAGS_DEF) $(DEBUG_FLAGS) $(OPITIMIZEFLAGS)

TEST_DIRS = add sleep str/BOP_string str/strsub str/strsub2 mem

library: print_info $(LIB_SO)

%.d: %.c
	@echo "Creating dependency $@"
	@$(CC) $(CFLAGS) -DBOP -MM -o $@ $?
-include $(DEPS)

ipa/libipa.a:
	@$(MAKE) -C ipa libipa.a

.PHONY: ipa/libipa.a

print_info:
	@echo Build info debug build = $(DEBUG)
	@echo cc = $(CC)
	@echo CFLAGS = $(CFLAGS)
	@echo LDFLAGS = $(LDFLAGS)
	@echo OBJS = $(OBJS)
	@echo IPA_OBJS = $(IPA_OBJS)

$(LIB_SO): $(OBJS) ipa/libipa.a
	@echo building archive "$(LIB_SO)"
	ar r $(LIB_SO) $(OBJS)  $(IPA_OBJS)
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
	$(MAKE) -C ipa clean
	$(foreach x,$(TEST_DIRS), @$(MAKE) -C tests/$(x) clean ${\n})

clobber: clean
	@rm -f $(DEPS)
	@$(MAKE) -C ipa clobber

export
