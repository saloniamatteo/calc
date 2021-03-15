# CC: can be gcc, clang, or a compiler of your choice

# Cross-compiler
#CC = aarch64-linux-gnu-gcc

# Normal compiler
CC = gcc

# Statically linked builds must be made using musl-gcc,
# because the static libraries (readline, ncurses) do not
# work with glibc
STATIC_CC = musl-gcc

# Linker flags used for Dynamic linking
LDFLAGS = -lreadline -lm
# Linker flags used for Static linking
LDFLAGS_STATIC = ./libs/readline/lib/libreadline.a ./libs/ncurses/lib/libncurses.a -lm
INCLUDE_PATHS = -I./libs/readline/include -I./libs/ncurses/include

# Optimizations. Can be -O0, -O1, -O2, -O3, -Os, -Ofast
OPTS = -Ofast

# Architecture, Tune. Set to 1 to disable.
NOARCHTUNE = 0

# Disable ARCH & TUNE if using cross-compiler,
# or if compiling on ARM.
ifeq ($(CC), aarch64-linux-gnu-gcc)
NOARCHTUNE = 1
else ifeq ($(shell uname -m), "aarch64")
NOARCHTUNE = 1
endif

# Check if NOARCHTUNE is 0 (enable ARCH & TUNE)
ifeq (NOARCHTUNE, 0)
	ARCH = -march=native
	TUNE = -mtune=native
endif

# CFLAGS: additional compiler flags
CFLAGS = -Wall
# LINKER: choose a linker to use; can be bfd, gold, lld
# comment to use the default linker, uncomment to use a custom linker
#LINKER = -fuse-ld=gold
# CSTD: which C revision to use
CSTD = -std=c99

# Release (Disables Debugging)
rel: release

release: calc-rel.o
	@$(CC) $^ -o calc -DDEBUG=0 $(CFLAGS) $(OPTS) $(LINKER) $(ARCH) $(TUNE) $(CSTD) $(LDFLAGS)
	@echo "CC $<"

calc-rel.o: calc.c color.h platform.h optimizations.h compiler.h
	@echo "[RELEASE]"
	@$(CC) -c $< -o $@ -DDEBUG=0 $(CFLAGS) $(OPTS) $(LINKER) $(ARCH) $(TUNE)
	@echo "CC $<"

# Debug (Debugging Enabled)
deb: debug

debug: calc-debug.o
	@$(CC) $^ -o calc $(CFLAGS) $(OPTS) $(LINKER) $(ARCH) $(TUNE) $(CSTD) $(LDFLAGS)
	@echo "CC $<"

calc-debug.o: calc.c color.h platform.h optimizations.h compiler.h
	@echo "[DEBUG]"
	@$(CC) -c $< -o $@ $(CFLAGS) $(OPTS) $(LINKER) $(ARCH) $(TUNE)
	@echo "CC $<"

# Statically-Linked Debug (Debugging Enabled, Portable)
static-deb: static-debug

static-debug: calc-debstatic.o
	@$(STATIC_CC) $^ -o calc -DDEBUG=1 -DSTATIC_BUILD=1 --static $(CFLAGS) $(OPTS) $(LINKER) $(ARCH) $(TUNE) $(CSTD) $(INCLUDE_PATHS) $(LDFLAGS_STATIC)
	@echo "CC $<"

calc-debstatic.o: calc.c color.h platform.h optimizations.h compiler.h
	@echo "[STATIC DEBUG]"
	@$(STATIC_CC) -c $< -o $@ -DDEBUG=1 -DSTATIC_BUILD=1 --static $(CFLAGS) $(OPTS) $(LINKER) $(ARCH) $(TUNE) $(INCLUDE_PATHS) $(LDFLAGS_STATIC)
	@echo "CC $<"

clean:
	rm -f *.o calc

.PHONY = clean deb debug rel release static-deb static-debug
