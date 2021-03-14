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
CSTD = -std=c99

rel: release

# Disables debugging
release: calc-rel.o
	@$(CC) $^ -o calc -DDEBUG=0 $(CFLAGS) $(OPTS) $(LINKER) $(ARCH) $(TUNE) $(CSTD) $(LDFLAGS)
	@echo "CC $<"

# Disables debugging
calc-rel.o: calc.c color.h platform.h optimizations.h compiler.h
	@$(CC) -c $< -o $@ -DDEBUG=0 $(CFLAGS) $(OPTS) $(LINKER) $(ARCH) $(TUNE) $(LDFLAGS)
	@echo "CC $<"

calc: calc.o
	@$(CC) $^ -o $@ $(CFLAGS) $(OPTS) $(LINKER) $(ARCH) $(TUNE) $(CSTD) $(LDFLAGS)
	@echo "CC $<"

calc.o: calc.c color.h platform.h optimizations.h compiler.h
	@$(CC) -c $< -o $@ $(CFLAGS) $(OPTS) $(LINKER) $(ARCH) $(TUNE) $(LDFLAGS)
	@echo "CC $<"

clean:
	rm -f *.o calc

.PHONY = calc clean rel release
