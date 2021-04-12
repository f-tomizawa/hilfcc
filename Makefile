CFLAGS=-std=c11 -g -static

hilfcc: hilfcc.c

test: hilfcc
	./test.sh

clean:
	rm -f hilfcc *.o *~ tmp*

.PHONY: test clean