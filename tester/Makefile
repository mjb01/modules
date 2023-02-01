CFLAGS=-Wall -pedantic -std=c11 -I.

all:	tqueue thash

queue.o:	queue.c
					gcc $(CFLAGS) -c queue.c

hash.o:		hash.c
					gcc $(CFLAGS) -c hash.c

tqueue:		tqueue.o tutils.o queue.o
					gcc $(CFLAGS) queue.o tqueue.o tutils.o -o $@

thash:		thash.o tutils.o queue.o hash.o
					gcc $(CFLAGS) queue.o hash.o thash.o tutils.o -o $@

test:
					./queue.test.sh
					./hash.test.sh

clean:
			rm -f hash.o queue.o thash tqueue
