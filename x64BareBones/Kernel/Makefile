
include Makefile.inc

KERNEL=kernel.bin
SOURCES=$(wildcard src/*.c) $(wildcard video/*.c) $(wildcard fonts/*.c)
SOURCES_ASM=$(wildcard asm/*.asm)
OBJECTS=$(SOURCES:.c=.o)
OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)
LOADERSRC=loader.asm
LOADEROBJECT=$(LOADERSRC:.asm=.o)
STATICLIBS=

GCCFLAGS += -I./include -I./fonts -I./video

all: $(KERNEL)

$(KERNEL): $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM) $(STATICLIBS)
	$(LD) $(LDFLAGS) -T kernel.ld -o $(KERNEL) $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM) $(STATICLIBS)

# --- Reglas por carpeta ---
src/%.o: src/%.c
	$(GCC) $(GCCFLAGS) -c $< -o $@

video/%.o: video/%.c
	$(GCC) $(GCCFLAGS) -c $< -o $@

fonts/%.o: fonts/%.c
	$(GCC) $(GCCFLAGS) -c $< -o $@

asm/%.o: asm/%.asm
	$(ASM) $(ASMFLAGS) $< -o $@

$(LOADEROBJECT):
	$(ASM) $(ASMFLAGS) $(LOADERSRC) -o $(LOADEROBJECT)

clean:
	rm -rf asm/*.o src/*.o video/*.o fonts/*.o *.bin

.PHONY: all clean

