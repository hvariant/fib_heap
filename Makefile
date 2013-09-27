CC = cc
CFLAGS = -g

test: fib_heap.c fib_heap.h test.c
	${CC} fib_heap.c test.c -o $@ ${CFLAGS}

clean:
	rm -rf test
	rm -rf test.dSYM

