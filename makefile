VPATH = include
#CFLAGS = -I include
CC = gcc -g3 -Wall -Wextra

c1p: c1p.o
	gcc -g3 -Wall -Wextra c1p.o -o c1p -lm

c1p.o c1p.d : c1p.c c1p.h list.h graph.h set.h matrix.h pair.h bool.h

circ1p: circ1p.o
	gcc -g3 -Wall -Wextra circ1p.o -o circ1p -lm

circ1p.o circ1p.d : circ1p.c circ1p.h c1p.h

clean:
	rm -rf *.o *.dSYM
