# CC: can be gcc, clang, or a compiler of your choice
CC = gcc
LIBS = -lreadline
# OPTS: can be -O0, -O1, -O2, -O3, -Os, -Ofast
OPTS = -Ofast
# ARCH & TUNE: modify to compile exclusively for a CPU 
ARCH = native
TUNE = native
# CFLAGS: additional compiler flags
CFLAGS = -Wall
# LINKER: choose a linker to use; can be bfd, gold, lld
# comment to use the default linker, uncomment to use a custom linker
#LINKER = -fuse-ld=gold
# CSTD: which C revision to use
CSTD = c99

all: calc

calc: calc.o
	@$(CC) $^ -o $@ $(CFLAGS) $(LIBS) $(OPTS) $(LINKER) -march=$(ARCH) -mtune=$(TUNE) -std=$(CSTD)
	@echo "CC $<"

calc.o: calc.c optimizations.h compiler.h
	@$(CC) -c calc.c -o $@ $(CFLAGS) $(LIBS) $(OPTS) $(LINKER) -march=$(ARCH) -mtune=$(TUNE) -std=$(CSTD)
	@echo "CC $<"

clean:
	rm -f *.o calc

.PHONY = all calc clean
