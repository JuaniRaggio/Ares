
# ORDEN CRÍTICO: Toolchain DEBE compilarse primero
all: toolchain bootloader kernel userland image compile_commands

toolchain:
	cd Toolchain; make all

bootloader:
	cd Bootloader; make all

kernel:
	cd Kernel; make all

userland:
	cd Userland; make all

image: toolchain kernel bootloader userland
	cd Image; make all

# Generar compile_commands.json unificado para IDE/LSP
compile_commands:
	@cd Kernel && $(MAKE) compile_commands.json
	@cd Userland/UserCodeModule && $(MAKE) compile_commands.json
	@echo '[' > compile_commands.json
	@tail -n +2 Kernel/compile_commands.json | sed '$$d' >> compile_commands.json
	@echo ',' >> compile_commands.json
	@tail -n +2 Userland/UserCodeModule/compile_commands.json | sed '$$d' >> compile_commands.json
	@echo ']' >> compile_commands.json

clean:
	cd Toolchain; make clean
	cd Bootloader; make clean
	cd Image; make clean
	cd Kernel; make clean
	cd Userland; make clean

.PHONY: toolchain bootloader image kernel userland all clean compile_commands
