# Estructura del Proyecto x64BareBones

## Resumen de Cambios Realizados

Este documento describe la estructura reorganizada del proyecto para mejorar la claridad y el soporte de IDEs.

### Cambios Principales

1. **Eliminación de headers duplicados**
   - Se eliminaron headers duplicados en `Kernel/drivers/video/fonts/`
   - Todos los headers públicos ahora están únicamente en `Kernel/include/`

2. **Estandarización de includes**
   - Todos los archivos `.c` y `.h` ahora usan includes con `<>` en lugar de paths relativos
   - Se eliminaron todos los `#include "../..."` del código
   - Ejemplo: `#include <font.h>` en lugar de `#include "../fonts/font.h"`

3. **Actualización de compile flags para IDEs**
   - `Kernel/compile_flags.txt` actualizado con todos los paths de include necesarios
   - `Userland/UserCodeModule/compile_flags.txt` actualizado
   - Generación de `Kernel/compile_commands.json` para soporte completo de LSP

4. **Makefile mejorado**
   - Paths de include más explícitos y organizados
   - Soporte para subdirectorios de `include/`

## Estructura de Directorios

```
x64BareBones/
├── Kernel/
│   ├── include/                    # Headers públicos (API del kernel)
│   │   ├── arch/
│   │   │   └── x86_64/
│   │   │       ├── interrupts.h
│   │   │       ├── idtLoader.h
│   │   │       └── syscalls.h
│   │   ├── core/
│   │   │   └── moduleLoader.h
│   │   ├── drivers/
│   │   │   ├── video_driver.h
│   │   │   ├── keyboard_driver.h
│   │   │   └── time.h
│   │   ├── font.h
│   │   ├── fontManager.h
│   │   ├── font_ubuntu_mono.h
│   │   ├── colors.h
│   │   ├── defs.h
│   │   ├── lib.h
│   │   ├── naiveConsole.h
│   │   └── syscallDispatcher.h
│   │
│   ├── arch/x86_64/               # Código específico de arquitectura
│   │   ├── boot/
│   │   └── interrupts/
│   │
│   ├── drivers/                   # Implementación de drivers
│   │   ├── video/
│   │   │   ├── video.h           # Header privado de implementación
│   │   │   ├── video.c
│   │   │   ├── video_driver.c
│   │   │   ├── naiveConsole.c
│   │   │   └── fonts/
│   │   │       ├── fontManager.c
│   │   │       └── font_ubuntu_mono.c
│   │   ├── keyboard/
│   │   │   └── keyboard_driver.c
│   │   └── timer/
│   │       └── time.c
│   │
│   ├── core/                      # Núcleo del kernel
│   │   ├── kernel.c
│   │   └── moduleLoader.c
│   │
│   ├── lib/                       # Bibliotecas del kernel
│   │   └── lib.c
│   │
│   ├── compile_flags.txt          # Flags para IDEs (clangd, etc.)
│   ├── compile_commands.json      # Database de compilación para LSP
│   ├── generate_compile_commands.py  # Script para regenerar compile_commands.json
│   ├── Makefile
│   └── Makefile.inc
│
├── Userland/
│   └── UserCodeModule/
│       ├── include/               # Headers del módulo de usuario
│       │   ├── shell.h
│       │   ├── parser.h
│       │   ├── commands.h
│       │   └── configuration.h
│       ├── src/                   # Código fuente
│       │   ├── shell.c
│       │   ├── parser.c
│       │   └── commands.c
│       ├── compile_flags.txt      # Flags para IDEs
│       ├── userCodeModule.c
│       └── Makefile
│
├── Bootloader/                    # Pure64 + BMFS
├── Image/                         # Generación de imágenes
├── Toolchain/                     # Herramientas de build
├── validate_structure.sh          # Script de validación
└── ESTRUCTURA.md                  # Este archivo
```

## Reglas de Includes

### Para archivos en el Kernel:

1. **Headers públicos**: Usar `#include <header.h>`
   ```c
   #include <font.h>
   #include <colors.h>
   #include <drivers/video_driver.h>
   #include <arch/x86_64/interrupts.h>
   ```

2. **Headers privados de implementación**: Usar `#include "header.h"` solo para headers en el mismo directorio
   ```c
   #include "video.h"  // Solo en drivers/video/*.c
   ```

3. **NUNCA usar paths relativos** como `../include/` o `../../`

### Para archivos en Userland:

1. Headers de userland: `#include <shell.h>`
2. Headers del kernel: `#include <drivers/video_driver.h>`

## Compile Flags

### Kernel (`Kernel/compile_flags.txt`)
```
-m64
-std=c99
-ffreestanding
-nostdlib
-fno-builtin-malloc
-fno-builtin-free
-fno-builtin-realloc
-fno-pie
-mno-red-zone
-mno-mmx
-mno-sse
-mno-sse2
-Wall
-Wextra
-Iinclude
-Iinclude/arch/x86_64
-Iinclude/core
-Iinclude/drivers
-Idrivers/video
```

### Userland (`Userland/UserCodeModule/compile_flags.txt`)
```
-m64
-std=c99
-ffreestanding
-nostdlib
-fno-builtin-malloc
-fno-builtin-free
-fno-builtin-realloc
-fno-pie
-mno-red-zone
-mno-mmx
-mno-sse
-mno-sse2
-Wall
-Wextra
-Iinclude
-I../../Kernel/include
-I../../Kernel/include/arch/x86_64
-I../../Kernel/include/drivers
```

## Soporte para IDEs

### Visual Studio Code / clangd

Los archivos `compile_flags.txt` y `compile_commands.json` proporcionan:
- Autocompletado correcto
- Navegación a definiciones
- Detección de errores en tiempo real
- IntelliSense mejorado

### Regenerar compile_commands.json

Si añades o eliminas archivos .c del kernel:
```bash
cd Kernel
python3 generate_compile_commands.py
```

## Validación de la Estructura

Para verificar que la estructura esté correcta:
```bash
./validate_structure.sh
```

El script verifica:
- No existan headers duplicados
- No existan includes con paths relativos (`../`)
- Existan los archivos `compile_flags.txt`
- Exista `compile_commands.json`
- Existan todos los directorios requeridos

## Makefile

El Makefile del kernel ahora incluye:
- Compilación incremental con dependencias automáticas
- Detección automática de archivos fuente
- Include paths explícitos y organizados
- Colores en la salida para mejor legibilidad

### Targets principales:
```bash
make all     # Compila todo el proyecto
make clean   # Limpia archivos de compilación
make info    # Muestra información del proyecto
```

## Buenas Prácticas

1. **Nunca duplicar headers**: Los headers públicos van en `include/`, los privados en su directorio de implementación

2. **Usar includes consistentes**: Siempre `<header.h>` para headers públicos

3. **Mantener compile_flags.txt actualizado**: Si añades nuevos directorios de include, actualiza estos archivos

4. **Ejecutar validación**: Antes de commits importantes, ejecuta `./validate_structure.sh`

5. **Regenerar compile_commands.json**: Después de añadir/eliminar archivos .c

## Compilación en Docker (ARES)

La estructura es totalmente compatible con el contenedor Docker ARES que solo tiene Makefile.

### Opción 1: Compilación rápida en contenedor existente (RECOMENDADO)

Si ya tenés el contenedor corriendo:

```bash
# Compilar usando el contenedor ARES (nombre por defecto)
./compile_in_container.sh

# Compilar usando un contenedor con otro nombre
./compile_in_container.sh MI_CONTENEDOR

# Limpiar el proyecto
./clean_in_container.sh

# Compilar un target específico
./compile_in_container.sh ARES kernel
```

**Ventajas:**
- Mucho más rápido (no reconstruye la imagen ni reinicia el contenedor)
- Más simple (solo ejecuta make dentro del contenedor)
- Inicia el contenedor automáticamente si está parado
- Corrige permisos automáticamente

### Opción 2: Compilación completa (rebuild completo)

Para rebuild completo de imagen y contenedor:

```bash
./compile.sh  # Reconstruye imagen, contenedor y compila
./run.sh      # Ejecuta en QEMU
```

**Nota:** Solo usar esto cuando cambien dependencias del Dockerfile

## Beneficios de la Nueva Estructura

1. **IDEs funcionan correctamente**: No más errores falsos de includes no encontrados
2. **Estructura más clara**: Separación entre API pública e implementación
3. **Mantenibilidad**: Más fácil agregar nuevos drivers o módulos
4. **Validación automática**: Script que verifica la consistencia
5. **Compatible con herramientas modernas**: LSP, clangd, etc.
