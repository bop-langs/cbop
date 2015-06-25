CC = gcc
OBJS = malloc_wrapper.o
SPECIAL_OBJS = dmmalloc.o
all: $(OBJS) $(SPECIAL_OBJS)
#
CFLAGS = -Wall -fPIC -ggdb3 -g3 -I.
LFLAGS = -ldl

$(SPECIAL_OBJS): EXTRA_FLAGS := -O3

%.o: %.c
	$(CC) -c -o $@ $^ $(CFLAGS) $(EXTRA_FLAGS)  
	
library: malloc_wrapper.o dmmalloc.o
	
test: $(OBJS) $(SPECIAL_OBS)
	$(CC) $(CFLAGS) $(EXTRA_FLAGS) $(OBJS) $(SPECIAL_OBJS) $(LFLAGS) -o wrapper wrapper_test.c
	./wrapper

clean:
	rm -f $(SPECIAL_OBJS) $(OBJS) wrapper
