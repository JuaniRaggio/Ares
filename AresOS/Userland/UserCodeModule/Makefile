include ../Makefile.inc

MODULE=0000-userCodeModule.bin
MODULE_ELF=0000-userCodeModule.elf

# Fuentes C: main + todo en src/ + libc de userland
SOURCES=$(wildcard [^_]*.c) $(wildcard src/*.c) $(wildcard libc/src/*.c)

# Fuentes ASM
ASM_SOURCES=asm/syscall.asm
ASM_OBJECTS=$(ASM_SOURCES:.asm=.o)

INCLUDE_FLAGS=-Ilibc/include -Iinclude

all: $(MODULE)

# Compilación de archivos ASM (NASM)
%.o: %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

$(MODULE): $(SOURCES) $(ASM_OBJECTS)
	$(GCC) $(GCCFLAGS) $(INCLUDE_FLAGS) -T userCodeModule.ld _loader.c $(SOURCES) $(ASM_OBJECTS) -o ../$(MODULE) -nostartfiles -Wl,--unresolved-symbols=ignore-in-object-files -Wl,--oformat=binary

clean:
	rm -rf *.o

.PHONY: all clean print
