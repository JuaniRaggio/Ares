# Checklist para el trabajo practico 2 de sistemas operativos

## Memory managers

- [ ] Memory manager custom (en nuestro caso el heap5 de FreeRTOS)
- [ ] Buddy System

> [!NOTE]
> Estos administradores no tienen que funcionar en paralelo sino que el
> build system va a tener que tener la capacidad de compilar el SO para
> usar buddy o custom memory manager


### Syscalls involucradas
- [ ] Reservar memoria
- [ ] Liberar memoria
- [ ] Consultar el estado de la memoria:
    - [ ] Total
    - [ ] Ocupada
    - [ ] Libre
    - [ ] Otras variables que se consideren importantes / necesarias


### Tests provistos
- [ ] test_mm: ciclo infinito que pide y libera bloques de tamaño random,
      chequeando en cada iteracion que los mismos no se solapan.
      Parametro: cantidad maxima de memoria a utilizar en bytes


## Procesos, Context Switching y Scheduling

- [ ] Round robin con prioridades

### Syscalls involucradas

- [ ] Crear y finalizar un proceso
- [ ] Obtener el ID del proceso que llama
- [ ] Listar todos los procesos:
        - [ ] nombre
        - [ ] ID
        - [ ] prioridad
        - [ ] stack
        - [ ] base pointer
        - [ ] foreground
        - [ ] variables que se consideren necesarias

- [ ] Matar un proceso arbitrario
- [ ] Modificar una prioridad de un proceso arbitrario
- [ ] Bloquear un proceso arbitrario
- [ ] Desbloquear un proceso arbitrario
- [ ] Renunciar al CPI
- [ ] Esperar a que los hijos terminen


### Test provistos

- [ ] test_proc: inf loop que crea, bloquea, desbloquea y mata procesos 
      dummy aleatoriamente. Toma como parametro la cantidad maxima de 
      procesos a crear. Solo imprime en caso de que ocurran errores.

- [ ] test_prio: crea 3 procesos que incrementan, cada uno, una variable 
      inicializada en 0.
      En primera instancia los 3 procesos poseen la misma prioridad. Luego 
      se vuelven a ejecutar con 3 prioridades diferentes a fin de visualizar
      diferencias en sus tiempos de ejecucion. Toma como parametro el valor 
      al que deben llegar las variables para finalizar

## IPC

Se deben implementar:

    - [ ] Pipes unidireccionales: Esto incluye las operaciones de lectura y
          escritura sobre los mismos deberan ser bloqueantes. Todo proceso
          debera ser capaz de leer/escribir tanto de un pipe como de la
          pantalla sin necesidad de que su codigo sea modificado.
          Notar que esto permitira que el interprete de comandos conecte 2
          programas utilizando un pipe y a su vez, estos programas prodran
          ser ejecutados de forma aislada. El sistema tambien debera 
          permitir que procesos no relacionados puedan compartirlos 
          acordando un identificador a priori

### Syscalls involucradas

    - [ ] Crear pipes
    - [ ] Abrir pipes
    - [ ] Leer de un pipe
    - [ ] Escribir un pipe

> [!NOTE]
> Notar que debe ser transparente para un proceso leer o escribir de un pipe
> o de la terminal


## Aplicaciones de User Space

Se deben implementar las siguientes apliacciones y *los nombres de las 
mismas deberan ser los mismos que en este listado*

- [ ] *sh*: shell de usuario que permite ejecutar las apps
        - Mecanismo si va a ceder o no el fg al proceso que se ejecuta (&)
        - Permitir conectar 2 procesos mediante un pipe (|)
        - Proveer soporte para <C-d> (envio de end of file)
        - Proveer soporte para <C-C> (matar al proceso de foreground)

- [ ] *help*: muestra una lista con todos los comandos disponibles. Tiene 
      que tener un apartado con el listado de tests provistos por la catedra


### Physical memory management

- [ ] *mem*: Imprime el estado de la memoria


### Procesos, context switching y scheduling

- [ ] *ps*: Imprime la lista de todos los procesos con sus prioridades

- [ ] *loop*: Imprime su ID con un saludo cada una determinada cantidad de
      tiempo para no inundar la pantalla. La espera debera realizarse de 
      forma activa, es decir, no bloquearse

- [ ] *kill*: Mata un proceso dado su ID

- [ ] *nice*: Cambia la prioridad de un proceso dado su ID y la nueva 
      prioridad

- [ ] *block*: Cambia el estado de un proceso entre bloqueaado y listo dado
      su ID


### IPC

- [ ] *cat*: Imprime el stdin tal como lo recibe
- [ ] *wc*: Cuenta la *cantidad de lineas* del input
- [ ] *mvar*: Implementa el problema de multiples lectores y escritores 
      sobre una variable global.

> [!NOTE]
> Checkear si o si que los siguientes comandos con los siguientes valores
> en mvar, devuelvan lo determinado en la tabla del pdf




