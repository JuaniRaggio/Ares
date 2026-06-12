RED    := \033[0;31m
GREEN  := \033[0;32m
YELLOW := \033[0;33m
NC     := \033[0m

all: toolchain bootloader kernel userland image
	@printf "$(GREEN)Build complete$(NC)\n"

toolchain:
	@printf "$(YELLOW)Building toolchain...$(NC)\n"
	@cd Toolchain; make all

bootloader:
	@printf "$(YELLOW)Building bootloader...$(NC)\n"
	@cd Bootloader; make all

kernel:
	@printf "$(YELLOW)Building kernel...$(NC)\n"
	@cd Kernel; make all

userland:
	@printf "$(YELLOW)Building userland...$(NC)\n"
	@cd Userland; make all

image: toolchain kernel bootloader userland
	@printf "$(YELLOW)Building image...$(NC)\n"
	@cd Image; make all

# Generate the unified compile_commands.json for IDE/LSP usage.
# IMPORTANT: run this on the HOST, not inside the container. Entries use
# absolute paths ($(CURDIR)); generated inside docker they point to /root
# and clangd cannot resolve any file.
compile_commands:
	@cd Kernel && $(MAKE) compile_commands.json
	@cd Userland/UserCodeModule && $(MAKE) compile_commands.json
	@python3 -c "import json; \
a = json.load(open('Kernel/compile_commands.json')); \
b = json.load(open('Userland/UserCodeModule/compile_commands.json')); \
json.dump(a + b, open('compile_commands.json', 'w'), indent=2)"
	@printf "$(GREEN)compile_commands.json regenerated$(NC)\n"

clean:
	@printf "$(YELLOW)Cleaning...$(NC)\n"
	@cd Toolchain; make clean
	@cd Bootloader; make clean
	@cd Image; make clean
	@cd Kernel; make clean
	@cd Userland; make clean
	@printf "$(GREEN)Clean complete$(NC)\n"

buddy:
	@printf "$(YELLOW)Building with buddy allocator...$(NC)\n"
	@cd Toolchain; make all
	@cd Bootloader; make all
	@cd Kernel; make buddy
	@cd Userland; make all
	@cd Image; make all
	@printf "$(GREEN)Buddy build complete$(NC)\n"

.PHONY: toolchain bootloader image kernel userland all clean compile_commands buddy
