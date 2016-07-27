CFLAGS = -g3 -fPIC -pthread -I.. -DBOP=1
LDFLAGS = $(CFLAGS) -L..
LDLIBS = -linst -lm -ldl

.PHONY: all
all: add

../libinst.a: $(wildcard ../*.[ch])
	$(MAKE) -C ..

add: add.o ../libinst.a

.PHONY: clean
clean:
	rm -f *.o add
