# ARES: A Recursive Experimental System

TP2 de Sistemas Operativos (ITBA) — núcleo de un sistema operativo monolítico
de 64 bits construido sobre el TP de Arquitectura de Computadoras. Implementa
administración de memoria física (dos managers intercambiables), procesos con
scheduling Round Robin por prioridades, semáforos, pipes y un conjunto de
aplicaciones de usuario.

## Compilación y ejecución

La compilación es obligatoria dentro de la imagen provista por la cátedra:

```bash
# 1. Descargar la imagen (una sola vez)
docker pull agodio/itba-so-multiarch:3.1

# 2. Crear el contenedor montando el proyecto (desde Ares/AresOS)
cd AresOS
docker run -d -v ${PWD}:/root --security-opt seccomp:unconfined -it \
    --name ARES agodio/itba-so-multiarch:3.1

# 3. Compilar
./compile_in_container.sh ARES          # memory manager por defecto (first-fit)
./compile_in_container.sh ARES buddy    # buddy system
./compile_in_container.sh ARES clean    # limpiar

# 4. Ejecutar en QEMU (en el host)
./run.sh                                  # normal
./run.sh -d                               # modo debug (GDB en el puerto 1234)
```

Reglas de `make` (a correr dentro del contenedor):

- `make` / `make all` — compila con el memory manager por defecto.
- `make buddy` — compila con el buddy system.
- `make clean` — limpia los artefactos de compilación.

La selección del memory manager es en **tiempo de compilación**: ambos
implementan la misma interfaz (`mem_alloc`/`mem_free`/`mem_get_stats`, ver
`Kernel/include/lib/memory_manager.h`) y solo uno se linkea. La compilación con
`-Wall` no reporta warnings.

## Configuración del entorno de desarrollo (IDE)

`compile_commands.json` se genera en el **host** (no en docker) para que clangd
resuelva las rutas locales:

```bash
cd AresOS && make compile_commands
```

Re-ejecutar cuando se agregan, mueven o eliminan archivos fuente. La
configuración compartida de clangd está en `AresOS/.clangd`.

## Comandos y tests

### Aplicaciones (corren como procesos separados)

| Comando | Parámetros | Descripción |
|---|---|---|
| `mem` | — | Imprime el estado del memory manager (total, usado, libre, bloques). |
| `ps` | — | Lista los procesos: PID, nombre, prioridad, estado, foreground, RSP, base del stack. |
| `loop` | `[segundos]` | Imprime su PID cada N segundos mediante espera activa (default 2s). |
| `kill` | `<pid>` | Mata el proceso con el PID dado. |
| `nice` | `<pid> <prioridad>` | Cambia la prioridad (1 a 4) de un proceso. |
| `block` | `<pid>` | Alterna un proceso entre bloqueado y listo. |
| `cat` | — | Imprime su stdin tal como lo recibe. |
| `wc` | — | Cuenta la cantidad de líneas del input. |
| `filter` | — | Filtra las vocales del input. |
| `mvar` | `<escritores> <lectores>` | Múltiples lectores/escritores sobre una variable global (estilo MVar de Haskell). |
| `div` | `<a> <b>` | División entera de dos números (demuestra la excepción de división por cero de forma aislada). |
| `opcode` | — | Dispara una excepción de opcode inválido (aislada al proceso). |
| `tron` | — | Juego Tron de ciclos de luz (WASD vs IJKL). |
| `printmem` | `<dir-hex>` | Dump de 32 bytes desde una dirección. |
| `benchmark` | — | Corre benchmarks de rendimiento. |

La shell de usuario es el proceso `sh` (PID 0), siempre en ejecución.

### Comandos built-in (corren dentro del proceso `sh`)

`help` (lista comandos y tests), `man <cmd>` (ayuda de un comando), `time`,
`clear`, `inforeg` (registros capturados con Ctrl+R), `history` (historial de
comandos), `cursor <forma>` (block/hollow/line/underline), `textcolor <color>`,
`bgcolor <color>`, `exit`.

### Tests de la cátedra (corren como procesos de usuario)

| Test | Parámetros | Descripción |
|---|---|---|
| `test_mm` | `<bytes>` | Pide y libera bloques aleatorios verificando que no se solapen. |
| `test_proc` | `<max_procesos>` | Crea, bloquea, desbloquea y mata procesos dummy aleatoriamente. |
| `test_prio` | `<valor>` | 3 procesos que incrementan una variable; se ejecutan con igual y con distinta prioridad. |
| `test_sync` | `<n> <use_sem>` | Pares de procesos que incrementan/decrementan una variable global; con semáforos el resultado final es 0. |

Los tests solo imprimen ante errores (salvo el resultado final de `test_sync`).

## Caracteres especiales y atajos de teclado

- `comando &` — ejecuta `comando` en **background** (el `&` debe ir separado por
  espacios). Sin `&`, el comando corre en foreground y la shell espera.
- `p1 | p2` — conecta la salida de `p1` con la entrada de `p2` mediante un
  **pipe** (el `|` debe ir separado por espacios). Ejemplo: `cat | wc`.
- **Ctrl+C** — mata el/los proceso(s) en foreground (no afecta a la shell).
- **Ctrl+D** — envía fin de archivo (EOF) al proceso que lee de teclado.
- **Ctrl+R** — captura el snapshot de registros (ver con `inforeg`).
- **Ctrl + / Ctrl -** — aumenta / reduce el tamaño de fuente.

## Ejemplos por requerimiento

```text
# Memoria
mem                       # estado del heap

# Procesos y scheduling
loop &                    # un loop en background
loop &                    # otro
ps                        # se ven ambos loops + shell + idle
nice 3 4                  # sube la prioridad del pid 3
block 3                   # lo bloquea; block 3 de nuevo lo desbloquea
kill 3                    # lo mata

# Sincronizacion (resultado final 0 con semaforos)
test_sync 100 1           # con semaforos -> "Final value: 0"
test_sync 100 0           # sin semaforos -> valor variable (race condition)

# IPC: pipes (transparencia terminal/pipe)
cat | wc                  # escribir lineas, Ctrl+D -> "Lines: N"
cat | filter              # escribir texto, Ctrl+D -> texto sin vocales

# MVar
mvar 2 2                  # imprime ABABAB... (2 escritores, 2 lectores)

# Tests en background
test_mm 1000000 &
test_proc 5 &
```

## Requerimientos faltantes o parcialmente implementados

Todos los requerimientos de la consigna están implementados. Las aplicaciones
heredadas del TP de Arquitectura (`tron`, `benchmark`, etc.) corren como
built-ins dentro de la shell, no como procesos; no son requeridas por la
consigna.

## Limitaciones

- **Memoria de procesos matados**: matar un proceso a mitad de ejecución filtra
  los bloques de heap que tuviera reservados, porque el kernel no registra qué
  bloques pertenecen a cada proceso. El ciclo de vida normal (crear/terminar) no
  filtra: los stacks y el área de FPU se reciclan.
- **Pipes**: un solo pipe por línea (`p1 | p2`, no `p1 | p2 | p3`); `|` y `&`
  deben ir separados por espacios; no se soportan pipes en background.
- La shell (PID 0) muestra `STACK BASE 0x0` en `ps` porque corre sobre el stack
  estático del kernel, no sobre uno reservado del heap.

## Consideraciones de diseño

- El único mecanismo de comunicación de los procesos con el kernel son las
  system calls (instrucción `syscall`/`sysret`).
- El sistema está libre de busy waiting (salvo donde la consigna lo requiere:
  `loop` y `test_sync` sin semáforos). Los caminos bloqueantes (pipes,
  semáforos, waitpid, lectura de teclado) duermen con `_hlt` y se despiertan por
  evento.
- La sincronización usa spinlocks con la instrucción atómica `xchg`. Los
  semáforos llevan reference counting (solo se destruyen en el último `close`).
- El context switch preserva los registros de propósito general y el estado
  FPU/SSE (fxsave/fxrstor por proceso).
- Los bugs encontrados durante el desarrollo y cómo se resolvieron están
  documentados en `AresOS/bugfixes/` (ver `troubleshooting.md`).

## Uso de inteligencia artificial

Durante el desarrollo se utilizó IA (Claude, de Anthropic) como asistente,
principalmente para:

- Diagnóstico de bugs de bajo nivel (entre ellos: el problema de `sysret` con la
  versión de NASM de la imagen de la cátedra, el orden de los selectores de la
  GDT que provocaba triple faults, races de concurrencia en el heap y los pipes,
  el reference counting de semáforos que hacía fallar `test_sync`, y la
  construcción atómica del PCB que hacía fallar `test_proc`).
- Revisión de código y sugerencias de mejora de calidad.
- Redacción de documentación.

Las decisiones de diseño, la verificación y la integración fueron realizadas por
el grupo. El detalle técnico de los problemas asistidos está en
`AresOS/bugfixes/troubleshooting.md`.

## Estructura del proyecto

```
Ares/
├── AresOS/
│   ├── Bootloader/   # Pure64 + BMFS
│   ├── Kernel/       # core (procesos, scheduler, semaforos, pipes),
│   │                 # lib (memory managers, slab), drivers, arch/x86_64
│   ├── Userland/     # shell, aplicaciones, libc, tests de la catedra
│   ├── Toolchain/    # ModulePacker
│   ├── Image/        # generacion de la imagen booteable
│   └── bugfixes/     # documentacion de bugs resueltos
└── doc/              # consigna y documentacion
```

## Setup de git hooks (opcional, para desarrollo)

```bash
./setup-hooks.sh    # configura core.hooksPath a .githooks/ (pre-commit: clang-format)
```

---

Desarrollado como parte del trabajo académico en el ITBA (Instituto Tecnológico
de Buenos Aires).
