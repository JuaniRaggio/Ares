# Kernel Makefile con compilación incremental
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

# Archivos fuente
C_SOURCES := $(shell find $(ARCH_DIR) $(DRIVERS_DIR) $(CORE_DIR) $(LIB_DIR) -name '*.c')
ASM_SOURCES := $(shell find $(ARCH_DIR) -name '*.asm')

# Objetos en build directory
C_OBJECTS := $(patsubst %.c,$(BUILD_DIR)/%.o,$(C_SOURCES))
ASM_OBJECTS := $(patsubst %.asm,$(BUILD_DIR)/%.o,$(ASM_SOURCES))
ALL_OBJECTS := $(C_OBJECTS) $(ASM_OBJECTS)

# Archivos de dependencias automáticas
DEPS := $(C_OBJECTS:.o=.d)

# Flags de compilación con includes
GCCFLAGS += -I$(INCLUDE_DIR)
GCCFLAGS += -I$(DRIVERS_DIR)/video
GCCFLAGS += -I$(DRIVERS_DIR)/video/fonts
GCCFLAGS += -MMD -MP  # Generar dependencias automáticas

# Colores para output
COLOR_RESET := \033[0m
COLOR_GREEN := \033[32m
COLOR_YELLOW := \033[33m
COLOR_BLUE := \033[34m

# Target principal
.PHONY: all clean info

all: $(KERNEL)
	@echo "$(COLOR_GREEN)✓ Kernel compilado exitosamente: $(KERNEL)$(COLOR_RESET)"

# Linkeo del kernel
$(KERNEL): $(ALL_OBJECTS) | $(BIN_DIR)
	@echo "$(COLOR_YELLOW)Linkeando kernel...$(COLOR_RESET)"
	@$(LD) $(LDFLAGS) -T kernel.ld -o $@ $(ALL_OBJECTS)

# Compilación de archivos C
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	@echo "$(COLOR_BLUE)CC$(COLOR_RESET)  $<"
	@$(GCC) $(GCCFLAGS) -c $< -o $@

# Compilación de archivos ASM
$(BUILD_DIR)/%.o: %.asm | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	@echo "$(COLOR_BLUE)ASM$(COLOR_RESET) $<"
	@$(ASM) $(ASMFLAGS) $< -o $@

# Crear directorios si no existen
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

# Incluir dependencias automáticas (si existen)
-include $(DEPS)

# Limpiar
clean:
	@echo "$(COLOR_YELLOW)Limpiando archivos de compilación...$(COLOR_RESET)"
	@rm -rf $(BUILD_DIR) $(BIN_DIR)
	@echo "$(COLOR_GREEN)✓ Limpieza completa$(COLOR_RESET)"

# Info del proyecto
info:
	@echo "Archivos C:   $(words $(C_SOURCES))"
	@echo "Archivos ASM: $(words $(ASM_SOURCES))"
	@echo "Total objetos: $(words $(ALL_OBJECTS))"
	@echo ""
	@echo "Estructura:"
	@echo "  - Architecture: $(ARCH_DIR)"
	@echo "  - Drivers:      $(DRIVERS_DIR)"
	@echo "  - Core:         $(CORE_DIR)"
	@echo "  - Library:      $(LIB_DIR)"
	@echo "  - Build dir:    $(BUILD_DIR)"
	@echo "  - Output:       $(KERNEL)"
