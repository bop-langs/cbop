CFLAGS = -g3 -fPIC -pthread -I.. -DBOP=1
LDFLAGS = $(CFLAGS) -L..
LDLIBS = -linst -lm -ldl

.PHONY: all
all: ../libinst.a add

../libinst.a:
	$(MAKE) -C ..

add: add.o

.PHONY: clean
clean:
	rm -f *.o add
