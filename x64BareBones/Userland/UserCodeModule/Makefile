include ../Makefile.inc

MODULE=0000-userCodeModule.bin
MODULE_ELF=0000-userCodeModule.elf
SOURCES=$(wildcard [^_]*.c) $(wildcard src/*.c)

INCLUDE_FLAGS=-I../../Kernel/include -Iinclude

all: $(MODULE)

$(MODULE_ELF): $(SOURCES)
	$(GCC) $(GCCFLAGS) $(INCLUDE_FLAGS) -T userCodeModule_elf.ld _loader.c $(SOURCES) -o $(MODULE_ELF) -nostartfiles -Wl,--unresolved-symbols=ignore-in-object-files

$(MODULE): $(MODULE_ELF)
	objcopy -O binary $(MODULE_ELF) ../$(MODULE)
	rm -f $(MODULE_ELF)

clean:
	rm -rf *.o

.PHONY: all clean print
