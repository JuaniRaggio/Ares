FROM debian:bookworm-slim

# --- Instalamos las dependencias necesarias ---
RUN apt update -y && apt install -y \
    gcc \
    make \
    nasm \
    gcc-x86-64-linux-gnu \
    binutils-x86-64-linux-gnu \
    qemu-utils \
    build-essential

# --- Directorio de trabajo ---
WORKDIR /root

# --- Copiamos y compilamos el ModulePacker ---
# Esto recompila mp.bin con la misma glibc del contenedor
COPY Toolchain/ModulePacker /root/Toolchain/ModulePacker
RUN cd /root/Toolchain/ModulePacker && make clean && make
