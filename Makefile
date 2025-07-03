VPATH = include
CC = gcc

c1p: c1p.o
	$(CC) -g3 -Wall -Wextra c1p.o -o c1p -lm

c1p.o: c1p.c c1p.h list.h graph.h set.h matrix.h pair.h bool.h
	$(CC) -c $< -o $@

circ1p: circ1p.o
	$(CC) -g3 -Wall -Wextra circ1p.o -o circ1p -lm

circ1p.o: circ1p.c circ1p.h c1p.h list.h graph.h set.h matrix.h pair.h bool.h
	$(CC) -c $< -o $@

clean:
	rm -rf *.o
