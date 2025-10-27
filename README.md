# ARES Recursive Experimental System

## Configuración del entorno de desarrollo

### Configurar Git Hooks

Este proyecto utiliza git hooks para mantener la consistencia del código. Para configurar los hooks en tu repositorio local:

```bash
./setup-hooks.sh
```

Este comando configurará git para usar los hooks en el directorio `.githooks/`, que incluye:

- **pre-commit**: Aplica automáticamente `clang-format` a todos los archivos C/C++ antes de hacer commit

### Requisitos

- `clang-format`: Necesario para el formateo automático del código


