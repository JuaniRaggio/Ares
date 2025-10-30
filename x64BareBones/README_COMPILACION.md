# Guía Rápida de Compilación

## Compilar el proyecto (método rápido)

```bash
# 1. Asegurate de que el contenedor Docker esté corriendo
docker ps  # Verificar que ARES esté en la lista

# 2. Compilar
./compile_in_container.sh

# 3. (Opcional) Limpiar antes de compilar
./clean_in_container.sh
./compile_in_container.sh
```

## Ejecutar el OS

```bash
# Ejecutar (usa automáticamente .qcow2 si existe, sino .img)
./run.sh

# Ejecutar en modo debug
./run.sh -d
```

## Generar todos los formatos de imagen (opcional)

Por defecto solo se genera `.img` (formato raw). Si querés generar también `.qcow2` y `.vmdk`:

```bash
# Después de compilar, ejecuta:
./build_all_images.sh
```

**Requisito:** Necesitás tener `qemu-img` instalado localmente:
- macOS: `brew install qemu`
- Ubuntu: `sudo apt-get install qemu-utils`

**Diferencias:**
- `.img` - Formato raw, funciona en Docker, más simple
- `.qcow2` - Comprimido, soporta snapshots, más eficiente (recomendado para desarrollo)
- `.vmdk` - Para VMware

## Usar un contenedor con otro nombre

```bash
# Si tu contenedor se llama diferente a "ARES"
./compile_in_container.sh MI_CONTENEDOR
```

## Compilar solo el kernel

```bash
./compile_in_container.sh ARES kernel
```

## El contenedor no existe o está parado?

Si el contenedor no existe, usá el método completo:

```bash
./compile.sh  # Crea imagen, contenedor y compila
```

Si el contenedor existe pero está parado, el script lo iniciará automáticamente.

## Verificar la estructura del proyecto

```bash
./validate_structure.sh
```

## Regenerar compile_commands.json (para IDEs)

**¿Qué es?** Un archivo que los IDEs usan para autocompletado, navegación y detección de errores.

**¿Cuándo regenerarlo?**
- Después de agregar/eliminar archivos `.c` en el Kernel
- Después de cambiar flags de compilación en `Makefile.inc`
- Si tu IDE no reconoce los includes correctamente

```bash
cd Kernel
python3 generate_compile_commands.py
cd ..
```

**IDEs compatibles:** VS Code, CLion, Vim/Neovim (LSP), Emacs (LSP), Sublime Text (LSP)

## Estructura de archivos importantes

- `compile_in_container.sh` - Compilación rápida en contenedor existente
- `clean_in_container.sh` - Limpiar proyecto
- `compile.sh` - Compilación completa (rebuild de imagen)
- `validate_structure.sh` - Validar estructura del proyecto
- `ESTRUCTURA.md` - Documentación completa de la estructura

## Soporte de IDE

Los archivos de configuración para IDEs están en:
- `Kernel/compile_flags.txt`
- `Kernel/compile_commands.json`
- `Userland/UserCodeModule/compile_flags.txt`

Estos archivos hacen que VS Code, clangd y otros IDEs reconozcan correctamente los includes.
