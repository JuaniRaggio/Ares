include ../Makefile.inc

MODULE=0000-userCodeModule.bin
MODULE_ELF=0000-userCodeModule.elf
SOURCES=$(wildcard [^_]*.c) $(wildcard src/*.c)

ASM_SOURCES=asm/syscall.asm
ASM_OBJECTS=$(ASM_SOURCES:.asm=.o)

INCLUDE_FLAGS=-I../../Kernel/include -Iinclude

all: $(MODULE)

# Compilación de archivos ASM (NASM)
%.o: %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

$(MODULE_ELF): $(SOURCES) $(ASM_OBJECTS)
	$(GCC) $(GCCFLAGS) $(INCLUDE_FLAGS) -T userCodeModule_elf.ld _loader.c $(SOURCES) $(ASM_OBJECTS) -o $(MODULE_ELF) -nostartfiles -Wl,--unresolved-symbols=ignore-in-object-files

$(MODULE): $(MODULE_ELF)
	objcopy -O binary $(MODULE_ELF) ../$(MODULE)
	rm -f $(MODULE_ELF)

clean:
	rm -rf *.o

.PHONY: all clean print
