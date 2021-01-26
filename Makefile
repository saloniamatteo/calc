CC = gcc
LIBS = -lreadline
OPTS = fast
ARCH = native
TUNE = native
CFLAGS = -Wall

all:
	$(CC) calc.c -o calc $(CFLAGS) $(LIBS) -O$(OPTS) -march=$(ARCH) -mtune=$(TUNE)

clean:
	rm ./calc
