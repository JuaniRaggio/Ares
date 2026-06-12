RED    := \033[0;31m
GREEN  := \033[0;32m
YELLOW := \033[0;33m
NC     := \033[0m

all: toolchain bootloader kernel userland image
	@echo -e "$(GREEN)Build complete$(NC)"

toolchain:
	@echo -e "$(YELLOW)Building toolchain...$(NC)"
	@cd Toolchain; make all

bootloader:
	@echo -e "$(YELLOW)Building bootloader...$(NC)"
	@cd Bootloader; make all

kernel:
	@echo -e "$(YELLOW)Building kernel...$(NC)"
	@cd Kernel; make all

userland:
	@echo -e "$(YELLOW)Building userland...$(NC)"
	@cd Userland; make all

image: toolchain kernel bootloader userland
	@echo -e "$(YELLOW)Building image...$(NC)"
	@cd Image; make all

# Generar compile_commands.json unificado para IDE/LSP.
# IMPORTANTE: correr en el HOST, no dentro del contenedor. Las entradas
# usan rutas absolutas ($(CURDIR)) y si se generan en docker quedan
# apuntando a /root y clangd no encuentra ningun archivo.
compile_commands:
	@cd Kernel && $(MAKE) compile_commands.json
	@cd Userland/UserCodeModule && $(MAKE) compile_commands.json
	@python3 -c "import json; \
a = json.load(open('Kernel/compile_commands.json')); \
b = json.load(open('Userland/UserCodeModule/compile_commands.json')); \
json.dump(a + b, open('compile_commands.json', 'w'), indent=2)"
	@echo -e "$(GREEN)compile_commands.json regenerated$(NC)"

clean:
	@echo -e "$(YELLOW)Cleaning...$(NC)"
	@cd Toolchain; make clean
	@cd Bootloader; make clean
	@cd Image; make clean
	@cd Kernel; make clean
	@cd Userland; make clean
	@echo -e "$(GREEN)Clean complete$(NC)"

buddy:
	@echo -e "$(YELLOW)Building with buddy allocator...$(NC)"
	@cd Toolchain; make all
	@cd Bootloader; make all
	@cd Kernel; make buddy
	@cd Userland; make all
	@cd Image; make all
	@echo -e "$(GREEN)Buddy build complete$(NC)"

.PHONY: toolchain bootloader image kernel userland all clean compile_commands buddy
