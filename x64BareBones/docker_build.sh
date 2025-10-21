#!/bin/bash

CONTAINER_NAME="ARES"
IMAGE_NAME="agodio/itba-so:1.0"
PROJECT_DIR="$(pwd)"

# --- Build de la imagen ---
echo "[*] Construyendo imagen Docker..."
sudo docker build -t $IMAGE_NAME .

# --- Si ya existe el contenedor, eliminarlo ---
if [ "$(sudo docker ps -aq -f name=$CONTAINER_NAME)" ]; then
  echo "[*] Eliminando contenedor anterior..."
  sudo docker rm -f $CONTAINER_NAME > /dev/null 2>&1
fi

# --- Iniciar el contenedor con el proyecto montado ---
echo "[*] Iniciando contenedor $CONTAINER_NAME..."
sudo docker run -d \
  -v "$PROJECT_DIR":/root/x64BareBones \
  --security-opt seccomp:unconfined \
  -ti \
  --name $CONTAINER_NAME \
  $IMAGE_NAME

# --- Ejecutar la compilación ---
echo "[*] Compilando proyecto dentro del contenedor..."
sudo docker exec -it $CONTAINER_NAME bash -c "cd /root/x64BareBones && make clean && make all"

# --- Mensaje final ---
echo "[✔] Compilación finalizada. La imagen está disponible en $PROJECT_DIR/Image/"

# --- Corregir permisos del directorio del proyecto ---
sudo chown -R $(id -u):$(id -g) "$PROJECT_DIR/Image"
