# CC: can be gcc, clang, or a compiler of your choice
CC = gcc
LDFLAGS = -lreadline -lm
# OPTS: can be -O0, -O1, -O2, -O3, -Os, -Ofast
OPTS = -Ofast
# ARCH & TUNE: modify to compile exclusively for a CPU 
ARCH = -march=native
TUNE = -mtune=native
# CFLAGS: additional compiler flags
CFLAGS = -Wall
# LINKER: choose a linker to use; can be bfd, gold, lld
# comment to use the default linker, uncomment to use a custom linker
#LINKER = -fuse-ld=gold
# CSTD: which C revision to use
CSTD = c99

all: calc

calc.o: calc.c platform.h optimizations.h compiler.h
	@$(CC) -c calc.c -o $@ $(CFLAGS) $(OPTS) $(LINKER) $(ARCH) $(TUNE) -std=$(CSTD)
	@echo "CC $<"

calc: calc.o
	@$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS) $(OPTS) $(LINKER) $(ARCH) $(TUNE) -std=$(CSTD)
	@echo "CC $<"

clean:
	rm -f *.o calc

.PHONY = all calc clean
