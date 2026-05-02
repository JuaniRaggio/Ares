# Kernel Makefile simplificado con orden de linkeo explícito
include Makefile.inc

# Directorios
ARCH_DIR := arch/x86_64
DRIVERS_DIR := drivers
CORE_DIR := core
LIB_DIR := lib
INCLUDE_DIR := include
BUILD_DIR := build
BIN_DIR := bin
COMMON_DIR := ../Common

# Archivos de salida
KERNEL_ELF := $(BIN_DIR)/kernel.elf
KERNEL := $(BIN_DIR)/kernel.bin

# 1. LOADER - DEBE SER PRIMERO (punto de entrada)
LOADER_SRC := $(ARCH_DIR)/boot/loader.asm
LOADER_OBJ := $(BUILD_DIR)/$(ARCH_DIR)/boot/loader.o

# 2. ASM de interrupts (orden determinista)
INTERRUPTS_ASM := $(ARCH_DIR)/interrupts/interrupts.asm
INTERRUPTS_OBJ := $(BUILD_DIR)/$(ARCH_DIR)/interrupts/interrupts.o

# 3. ASM de syscalls (orden alfabético explícito)
SYSCALLS_ASM := asm/libasm.asm asm/sys_exit.asm asm/sys_read.asm asm/sys_clear.asm asm/sys_ticks.asm asm/sys_misc.asm asm/syscalls.asm asm/syscalls_table.asm asm/init_syscalls.asm asm/kernel_stack.asm asm/setup_user_segments.asm asm/jump_to_userland.asm asm/sys_set_bg_color.asm asm/sys_set_text_color.asm asm/sys_get_cursor_pos.asm asm/sys_redraw_screen.asm asm/tss.asm asm/sys_sound.asm
SYSCALLS_OBJ := $(patsubst %.asm,$(BUILD_DIR)/%.o,$(SYSCALLS_ASM))

# Memory manager: por defecto multi_region_heap, override con MM=buddy
MM_SRC ?= $(LIB_DIR)/multi_region_heap.c

# 4. Código C (todos juntos, orden no crítico)
# Se excluyen los memory managers del find y se agrega el seleccionado
C_SOURCES := $(shell find $(ARCH_DIR) $(DRIVERS_DIR) $(CORE_DIR) $(LIB_DIR) -name '*.c' ! -name 'multi_region_heap.c' ! -name 'buddy.c')
C_SOURCES += $(MM_SRC)
COMMON_SOURCES := $(shell find $(COMMON_DIR)/src -name '*.c')
C_OBJECTS := $(patsubst %.c,$(BUILD_DIR)/%.o,$(C_SOURCES)) $(patsubst $(COMMON_DIR)/src/%.c,$(BUILD_DIR)/common/%.o,$(COMMON_SOURCES))

ALL_OBJECTS := $(LOADER_OBJ) $(INTERRUPTS_OBJ) $(SYSCALLS_OBJ) $(C_OBJECTS)

# Archivos de dependencias automáticas
DEPS := $(C_OBJECTS:.o=.d)

# Flags de compilación con includes
GCCFLAGS += -I$(INCLUDE_DIR)
GCCFLAGS += -I$(INCLUDE_DIR)/lib
GCCFLAGS += -I$(INCLUDE_DIR)/arch/x86_64
GCCFLAGS += -I$(INCLUDE_DIR)/core
GCCFLAGS += -I$(INCLUDE_DIR)/drivers
GCCFLAGS += -I$(DRIVERS_DIR)/video
GCCFLAGS += -I$(COMMON_DIR)/include
GCCFLAGS += -MMD -MP  # Generar dependencias automáticas

# Target principal
.PHONY: all clean info compile_commands.json

all: $(KERNEL)
	@echo "✓ Kernel compilado: $(KERNEL)"
	@echo "✓ Símbolos de debug: $(KERNEL_ELF)"

# Linkeo del kernel ELF con símbolos de debug
$(KERNEL_ELF): $(ALL_OBJECTS) | $(BIN_DIR)
	@echo "Linkeando kernel ELF..."
	@$(LD) $(LDFLAGS) --oformat=elf64-x86-64 -T kernel.ld -o $@ $(ALL_OBJECTS)

# Extraer binario plano del ELF
$(KERNEL): $(KERNEL_ELF)
	@echo "Generando binario plano..."
	@objcopy -O binary $< $@

# Compilación de loader (PRIMERO)
$(LOADER_OBJ): $(LOADER_SRC) | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	@echo "ASM (loader) $<"
	@$(ASM) $(ASMFLAGS) $< -o $@

# Compilación de interrupts ASM
$(INTERRUPTS_OBJ): $(INTERRUPTS_ASM) | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	@echo "ASM (interrupts) $<"
	@$(ASM) $(ASMFLAGS) $< -o $@

# Compilación de syscalls ASM
$(BUILD_DIR)/asm/%.o: asm/%.asm | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	@echo "ASM (syscalls) $<"
	@$(ASM) $(ASMFLAGS) $< -o $@

# Compilación de archivos C del Kernel
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	@echo "CC  $<"
	@$(GCC) $(GCCFLAGS) -c $< -o $@

# Compilación de archivos C de Common
$(BUILD_DIR)/common/%.o: $(COMMON_DIR)/src/%.c | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	@echo "CC (common) $<"
	@$(GCC) $(GCCFLAGS) -c $< -o $@


# Crear directorios si no existen
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

# Incluir dependencias automáticas (si existen)
-include $(DEPS)

# Limpiar
clean:
	@echo "Limpiando archivos de compilación..."
	@rm -rf $(BUILD_DIR) $(BIN_DIR)
	@echo "✓ Limpieza completa"

# Generar compile_commands.json para IDE/LSP
compile_commands.json:
	@printf '[\n' > $@
	@first=1; \
	for src in $(C_SOURCES); do \
		[ $$first -ne 1 ] && printf ',\n' >> $@; first=0; \
		printf '  { "directory": "%s", "command": "%s -c %s", "file": "%s/%s" }' \
			"$(CURDIR)" "$(GCC) $(GCCFLAGS)" "$$src" "$(CURDIR)" "$$src" >> $@; \
	done; \
	for src in $(COMMON_SOURCES); do \
		printf ',\n' >> $@; \
		abs=$$(cd "$$(dirname "$$src")" && pwd)/$$(basename "$$src"); \
		printf '  { "directory": "%s", "command": "%s -c %s", "file": "%s" }' \
			"$(CURDIR)" "$(GCC) $(GCCFLAGS)" "$$src" "$$abs" >> $@; \
	done
	@printf '\n]\n' >> $@

# Compilar con buddy system: make buddy
.PHONY: buddy
buddy:
	$(MAKE) MM_SRC=$(LIB_DIR)/buddy.c all

# Info del proyecto
info:
	@echo "Loader:       $(LOADER_SRC)"
	@echo "Interrupts:   $(INTERRUPTS_ASM)"
	@echo "Syscalls ASM: $(words $(SYSCALLS_ASM)) archivos"
	@echo "Archivos C:   $(words $(C_SOURCES))"
	@echo "Total objetos: $(words $(ALL_OBJECTS))"
	@echo ""
	@echo "Orden de linkeo:"
	@echo "  1. $(LOADER_OBJ)"
	@echo "  2. $(INTERRUPTS_OBJ)"
	@echo "  3. $(SYSCALLS_OBJ)"
	@echo "  4. [C objects...]"
