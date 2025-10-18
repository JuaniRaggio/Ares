# x64BareBones - Documentacion del Proyecto

## Descripcion General
x64BareBones es un proyecto base para el desarrollo de sistemas operativos en la arquitectura Intel 64 bits (x86-64). Proporciona un punto de entrada para un kernel y la capacidad de cargar modulos binarios separados del kernel principal.

Autores: Rodrigo Rearden (RowDaBoat), Augusto Nizzo McIntosh

## Estructura de Directorios

### /Bootloader
Contiene el codigo del cargador de arranque que inicializa el sistema antes de pasar el control al kernel.

#### Bootloader/Pure64
Pure64 es un cargador de arranque de 64 bits que inicializa el procesador y la memoria.
- `src/pure64.asm`: Punto de entrada principal del bootloader
- `src/init/`: Modulos de inicializacion
  - `acpi.asm`: Inicializacion de ACPI (Advanced Configuration and Power Interface)
  - `cpu.asm`: Configuracion del procesador
  - `smp.asm` y `smp_ap.asm`: Soporte para multiprocesamiento simetrico
  - `pic.asm`: Controlador de interrupciones programable
  - `isa.asm`: Bus ISA (Industry Standard Architecture)
- `src/interrupt.asm`: Manejo de interrupciones
- `src/syscalls.asm`: Llamadas al sistema
- `src/bootsectors/`: Sectores de arranque para diferentes modos (MBR, PXE)

#### Bootloader/BMFS
Sistema de archivos BMFS (BareMetal File System) utilizado por el bootloader.
- `bmfs.c`: Implementacion del sistema de archivos

### /Kernel
Contiene el codigo principal del kernel del sistema operativo.

#### Archivos principales:
- `kernel.c`: Punto de entrada principal del kernel
  - `initializeKernelBinary()`: Inicializa el binario del kernel, limpia BSS, carga modulos
  - `main()`: Funcion principal que ejecuta los modulos cargados
  - `clearBSS()`: Limpia el segmento BSS (variables no inicializadas)
  - `getStackBase()`: Calcula la base del stack

- `naiveConsole.c`: Implementacion simple de consola por VGA
  - Direccion de video: 0xB8000 (memoria de video en modo texto)
  - Resolucion: 80x25 caracteres
  - Funciones principales:
    - `ncPrint()`: Imprime strings
    - `ncPrintChar()`: Imprime un caracter
    - `ncNewline()`: Salto de linea
    - `ncPrintHex()`, `ncPrintDec()`, `ncPrintBin()`: Impresion en diferentes bases
    - `ncClear()`: Limpia la pantalla

- `moduleLoader.c`: Carga modulos binarios en memoria
  - `loadModules()`: Carga multiples modulos desde el payload
  - `loadModule()`: Carga un modulo individual en una direccion especifica
  - Formato: [cantidad_modulos][tamanio_modulo][datos_modulo]...

- `lib.c`: Funciones de libreria basicas
  - `memset()`: Rellena memoria con un valor
  - `memcpy()`: Copia bloques de memoria (optimizada para alineacion de 32 bits)

- `loader.asm`: Punto de entrada en ensamblador
  - Configura el stack llamando a `initializeKernelBinary()`
  - Llama a `main()` del kernel
  - Loop infinito si main retorna

#### Kernel/asm
- `libasm.asm`: Funciones de libreria en ensamblador
  - `cpuVendor()`: Obtiene el vendor ID del CPU usando instruccion CPUID

#### Kernel/include
Archivos header con declaraciones de funciones y estructuras.

### /Userland
Codigo de espacio de usuario que se ejecuta sobre el kernel.

#### Userland/SampleCodeModule
Modulo de ejemplo que se carga en memoria.
- `sampleCodeModule.c`:
  - Se carga en 0x400000
  - Escribe 'X' en la esquina superior derecha de la pantalla (posicion 79)
  - Verifica que BSS este correctamente inicializado
  - Retorna 0xDEADC0DE si BSS es correcto, 0xDEADBEEF si no

- `_loader.c`: Loader del modulo de usuario

### /Toolchain
Herramientas de construccion del proyecto.

#### Toolchain/ModulePacker
Herramienta que empaqueta multiples binarios (kernel + modulos) en una sola imagen.
- `main.c`:
  - `buildImage()`: Construye la imagen final combinando kernel y modulos
  - Formato de salida: [kernel][num_modulos][size_mod1][mod1][size_mod2][mod2]...
  - Permite especificar archivo de salida con `-o`

### /Image
Directorio donde se genera la imagen final del sistema operativo lista para ejecutar.

## Flujo de Ejecucion

1. **Bootloader (Pure64)**:
   - Inicializa el hardware (CPU, memoria, interrupciones)
   - Configura modo largo de 64 bits
   - Carga el kernel en memoria
   - Transfiere control al kernel

2. **Kernel (loader.asm)**:
   - Llama a `initializeKernelBinary()`
   - Configura el stack
   - Llama a `main()`

3. **Kernel (kernel.c - initializeKernelBinary)**:
   - Imprime informacion del sistema
   - Obtiene vendor del CPU
   - Carga modulos en direcciones especificas:
     - Modulo de codigo: 0x400000
     - Modulo de datos: 0x500000
   - Limpia el segmento BSS
   - Imprime ubicaciones de segmentos de memoria

4. **Kernel (kernel.c - main)**:
   - Ejecuta el modulo de codigo llamando a su punto de entrada
   - Imprime el valor retornado
   - Imprime el contenido del modulo de datos

## Direcciones de Memoria Importantes

- `0xB8000`: Memoria de video VGA en modo texto
- `0x400000`: Direccion de carga del modulo de codigo de ejemplo
- `0x500000`: Direccion de carga del modulo de datos de ejemplo

## Segmentos de Memoria del Kernel

- `text`: Segmento de codigo ejecutable
- `rodata`: Datos de solo lectura
- `data`: Datos inicializados
- `bss`: Datos no inicializados (se limpian a 0 en startup)
- `endOfKernelBinary`: Fin del binario del kernel
- `endOfKernel`: Fin total del kernel (incluye BSS)

## Construccion del Proyecto

1. Instalar dependencias: `nasm`, `qemu`, `gcc`, `make`
2. Construir Toolchain: `cd Toolchain && make all`
3. Construir todo: `make all` (desde raiz)
4. Ejecutar: `./run.sh`

## Archivos de Configuracion

- `Makefile`: Makefiles en cada directorio coordinan la construccion
- `run.sh`: Script para ejecutar el sistema en QEMU
- `Dockerfile`: Configuracion de contenedor para entorno de desarrollo
