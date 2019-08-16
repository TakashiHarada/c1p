VPATH = include
#CFLAGS = -I include
CC = gcc -g3 -Wall -Wextra

c1p: c1p.o
	gcc -g3 -Wall -Wextra c1p.o -o c1p -lm

c1p.o c1p.d : c1p.c c1p.h list.h graph.h set.h matrix.h pair.h bool.h

clean:
	rm -rf *.o *.dSYM
