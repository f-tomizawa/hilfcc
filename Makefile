CFLAGS=-std=c11 -g -static
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

hilfcc: $(OBJS)
	$(CC) -o hilfcc $(OBJS) $(LDFLAGS)

$(OBJS): hilfcc.h

test: hilfcc
	./test.sh

clean:
	rm -f hilfcc *.o *~ tmp*

.PHONY: test clean