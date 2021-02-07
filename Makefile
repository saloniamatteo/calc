# CC: can be gcc, clang, or a compiler of your choice
CC = gcc
LIBS = -lreadline
# OPTS: can be 0, 1, 2, 3, s, fast
OPTS = 3
# ARCH & TUNE: modify to compile exclusively for a CPU 
ARCH = native
TUNE = native
# CFLAGS: additional compiler flags
#CFLAGS = -Wall

output: calc.o optimizations.o compiler.o
	$(CC) calc.o -o calc $(CFLAGS) $(LIBS) -O$(OPTS) -march=$(ARCH) -mtune=$(TUNE)

calc.o: calc.c calc.h
	$(CC) -c calc.c $(CFLAGS) $(LIBS) -O$(OPTS) -march=$(ARCH) -mtune=$(TUNE)

optimizations.o: optimizations.h
	$(CC) -c optimizations.h $(CFLAGS) $(LIBS) -O$(OPTS) -march=$(ARCH) -mtune=$(TUNE)

compiler.o: compiler.h
	$(CC) -c compiler.h $(CFLAGS) $(LIBS) -O$(OPTS) -march=$(ARCH) -mtune=$(TUNE)

clean:
	rm -f *.o *.gch calc
