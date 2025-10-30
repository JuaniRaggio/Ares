include ../Makefile.inc

MODULE=0000-userCodeModule.bin
SOURCES=$(wildcard [^_]*.c) $(wildcard src/*.c)

INCLUDE_FLAGS=-I../../Kernel/include -Iinclude

all: $(MODULE)

$(MODULE): $(SOURCES)
	$(GCC) $(GCCFLAGS) $(INCLUDE_FLAGS) -T userCodeModule.ld _loader.c $(SOURCES) -o ../$(MODULE)

clean:
	rm -rf *.o

.PHONY: all clean print
