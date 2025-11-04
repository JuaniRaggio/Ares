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

# Archivos de salida
KERNEL := $(BIN_DIR)/kernel.bin

# 1. LOADER - DEBE SER PRIMERO (punto de entrada)
LOADER_SRC := $(ARCH_DIR)/boot/loader.asm
LOADER_OBJ := $(BUILD_DIR)/$(ARCH_DIR)/boot/loader.o

# 2. ASM de interrupts (orden determinista)
INTERRUPTS_ASM := $(ARCH_DIR)/interrupts/interrupts.asm
INTERRUPTS_OBJ := $(BUILD_DIR)/$(ARCH_DIR)/interrupts/interrupts.o

# 3. ASM de syscalls (orden alfabético explícito)
SYSCALLS_ASM := asm/libasm.asm asm/sys_exit.asm asm/sys_write.asm asm/sys_read.asm asm/sys_clear.asm asm/sys_ticks.asm asm/sys_misc.asm asm/syscalls.asm asm/syscalls_table.asm asm/init_syscalls.asm asm/kernel_stack.asm asm/setup_user_segments.asm asm/jump_to_userland.asm asm/sys_set_bg_color.asm asm/sys_set_text_color.asm
SYSCALLS_OBJ := $(patsubst %.asm,$(BUILD_DIR)/%.o,$(SYSCALLS_ASM))

# 4. Código C (todos juntos, orden no crítico)
C_SOURCES := $(shell find $(ARCH_DIR) $(DRIVERS_DIR) $(CORE_DIR) $(LIB_DIR) -name '*.c')
C_OBJECTS := $(patsubst %.c,$(BUILD_DIR)/%.o,$(C_SOURCES))

ALL_OBJECTS := $(LOADER_OBJ) $(INTERRUPTS_OBJ) $(SYSCALLS_OBJ) $(C_OBJECTS)

# Archivos de dependencias automáticas
DEPS := $(C_OBJECTS:.o=.d)

# Flags de compilación con includes
GCCFLAGS += -I$(INCLUDE_DIR)
GCCFLAGS += -I$(INCLUDE_DIR)/arch/x86_64
GCCFLAGS += -I$(INCLUDE_DIR)/core
GCCFLAGS += -I$(INCLUDE_DIR)/drivers
GCCFLAGS += -I$(DRIVERS_DIR)/video
GCCFLAGS += -MMD -MP  # Generar dependencias automáticas

# Target principal
.PHONY: all clean info

all: $(KERNEL)
	@echo "✓ Kernel compilado: $(KERNEL)"

# Linkeo del kernel con orden explícito
$(KERNEL): $(ALL_OBJECTS) | $(BIN_DIR)
	@echo "Linkeando kernel..."
	@$(LD) $(LDFLAGS) -T kernel.ld -o $@ $(ALL_OBJECTS)

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

# Compilación de archivos C
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	@echo "CC  $<"
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
