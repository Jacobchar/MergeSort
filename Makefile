DEPS = Mergesort.h

Mergesort: Mergesort.c
	gcc -std=c99 -D _XOPEN_SOURCE=600 -o Mergesort Mergesort.c -I.
