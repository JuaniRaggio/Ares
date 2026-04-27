include ../Makefile.inc

MODULE=0000-userCodeModule.bin
MODULE_ELF=0000-userCodeModule.elf

# Fuentes C: main + todo en src/ + libc de userland + common
COMMON_DIR=../../Common
SOURCES=$(wildcard [^_]*.c) $(wildcard src/*.c) $(wildcard libc/src/*.c) $(wildcard $(COMMON_DIR)/src/*.c)

# Fuentes ASM
ASM_SOURCES=asm/syscall.asm asm/opcode.asm
ASM_OBJECTS=$(ASM_SOURCES:.asm=.o)

INCLUDE_FLAGS=-Ilibc/include -Iinclude -I$(COMMON_DIR)/include

ALL_C_SOURCES := _loader.c $(SOURCES)

all: $(MODULE)

# Compilación de archivos ASM (NASM)
%.o: %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

$(MODULE): $(SOURCES) $(ASM_OBJECTS)
	$(GCC) $(GCCFLAGS) $(INCLUDE_FLAGS) -T userCodeModule.ld _loader.c $(SOURCES) $(ASM_OBJECTS) -o ../$(MODULE) -nostartfiles -Wl,--unresolved-symbols=ignore-in-object-files -Wl,--oformat=binary

# Generar compile_commands.json para IDE/LSP
compile_commands.json:
	@printf '[\n' > $@
	@first=1; \
	for src in $(ALL_C_SOURCES); do \
		[ $$first -ne 1 ] && printf ',\n' >> $@; first=0; \
		abs=$$(cd "$$(dirname "$$src")" && pwd)/$$(basename "$$src"); \
		printf '  { "directory": "%s", "command": "%s %s -c %s", "file": "%s" }' \
			"$(CURDIR)" "$(GCC) $(GCCFLAGS)" "$(INCLUDE_FLAGS)" "$$src" "$$abs" >> $@; \
	done
	@printf '\n]\n' >> $@

clean:
	rm -rf *.o

.PHONY: all clean print compile_commands.json
