# Checklist para el trabajo practico 2 de sistemas operativos

> Convención: [X] hecho y verificado, [~] parcial / a confirmar.

## Memory managers

- [X] Memory manager custom (en nuestro caso el heap5 de FreeRTOS)
- [X] Buddy System

> [!NOTE]
> Estos administradores no tienen que funcionar en paralelo sino que el
> build system va a tener que tener la capacidad de compilar el SO para
> usar buddy o custom memory manager
> (`make` = custom, `make buddy` = buddy; ambos compilan limpio con -Wall)


### Syscalls involucradas
- [X] Reservar memoria
- [X] Liberar memoria
- [X] Consultar el estado de la memoria:
    - [X] Total
    - [X] Ocupada
    - [X] Libre
    - [X] Otras variables que se consideren importantes / necesarias


### Tests provistos
- [X] test_mm: ciclo infinito que pide y libera bloques de tamaño random,
      chequeando en cada iteracion que los mismos no se solapan.
      Parametro: cantidad maxima de memoria a utilizar en bytes
      (verificado en fg y bg; mem confirma sin fugas en ~500k ciclos)


## Procesos, Context Switching y Scheduling

- [X] Round robin con prioridades

### Syscalls involucradas

- [X] Crear y finalizar un proceso
- [X] Obtener el ID del proceso que llama
- [X] Listar todos los procesos:
        - [X] nombre
        - [X] ID
        - [X] prioridad
        - [X] stack
        - [X] base pointer
        - [X] foreground
        - [X] variables que se consideren necesarias

- [X] Matar un proceso arbitrario
- [X] Modificar una prioridad de un proceso arbitrario
- [X] Bloquear un proceso arbitrario
- [X] Desbloquear un proceso arbitrario
- [X] Renunciar al CPI
- [X] Esperar a que los hijos terminen


### Test provistos

- [X] test_proc: inf loop que crea, bloquea, desbloquea y mata procesos 
      dummy aleatoriamente. Toma como parametro la cantidad maxima de 
      procesos a crear. Solo imprime en caso de que ocurran errores.
      (verificado en bg; 1M+ ciclos sin fugas ni errores)

- [~] test_prio: crea 3 procesos que incrementan, cada uno, una variable 
      inicializada en 0.
      En primera instancia los 3 procesos poseen la misma prioridad. Luego 
      se vuelven a ejecutar con 3 prioridades diferentes a fin de visualizar
      diferencias en sus tiempos de ejecucion. Toma como parametro el valor 
      al que deben llegar las variables para finalizar
      (implementado y registrado como proceso; falta re-verificar la salida)

## Sincronizacion

> Seccion del enunciado que faltaba en esta checklist; agregada para respetar
> la consigna.

- [X] Semaforos compartibles por procesos no relacionados (id a priori)
- [X] Libre de busy waiting, deadlock y race conditions; instruccion atomica
      (xchg en el spinlock; se corrigio un race en process_wait)

### Syscalls involucradas
- [X] Crear, abrir y cerrar semaforos
- [X] Modificar el valor de un semaforo (wait/post)

### Tests provistos
- [X] test_sync: incrementa/decrementa una variable global compartida.
      Con semaforos el resultado final es 0; sin semaforos varia.
      (verificado en fg y bg: con sem `Final value: 0`, sin sem distinto de 0)

## IPC

Se deben implementar:

    - [X] Pipes unidireccionales: Esto incluye las operaciones de lectura y
          escritura sobre los mismos deberan ser bloqueantes. Todo proceso
          debera ser capaz de leer/escribir tanto de un pipe como de la
          pantalla sin necesidad de que su codigo sea modificado.
          Notar que esto permitira que el interprete de comandos conecte 2
          programas utilizando un pipe y a su vez, estos programas prodran
          ser ejecutados de forma aislada. El sistema tambien debera 
          permitir que procesos no relacionados puedan compartirlos 
          acordando un identificador a priori

### Syscalls involucradas

    - [X] Crear pipes
    - [X] Abrir pipes
    - [X] Leer de un pipe
    - [X] Escribir un pipe

> [!NOTE]
> Notar que debe ser transparente para un proceso leer o escribir de un pipe
> o de la terminal


## Aplicaciones de User Space

Se deben implementar las siguientes apliacciones y *los nombres de las 
mismas deberan ser los mismos que en este listado*

- [X] *sh*: shell de usuario que permite ejecutar las apps
        - [X] Mecanismo si va a ceder o no el fg al proceso que se ejecuta (&)
        - [X] Permitir conectar 2 procesos mediante un pipe (|)
        - [X] Proveer soporte para <C-d> (envio de end of file)
        - [X] Proveer soporte para <C-C> (matar al proceso de foreground)
      (el proceso se renombro de "shell" a "sh" para respetar el enunciado)

- [X] *help*: muestra una lista con todos los comandos disponibles. Tiene 
      que tener un apartado con el listado de tests provistos por la catedra


### Physical memory management

- [X] *mem*: Imprime el estado de la memoria


### Procesos, context switching y scheduling

- [X] *ps*: Imprime la lista de todos los procesos con sus prioridades

- [X] *loop*: Imprime su ID con un saludo cada una determinada cantidad de
      tiempo para no inundar la pantalla. La espera debera realizarse de 
      forma activa, es decir, no bloquearse

- [X] *kill*: Mata un proceso dado su ID

- [X] *nice*: Cambia la prioridad de un proceso dado su ID y la nueva 
      prioridad

- [X] *block*: Cambia el estado de un proceso entre bloqueaado y listo dado
      su ID


### IPC

- [X] *cat*: Imprime el stdin tal como lo recibe
- [X] *wc*: Cuenta la *cantidad de lineas* del input
- [~] *mvar*: Implementa el problema de multiples lectores y escritores 
      sobre una variable global.
      (implementado; la verificacion de la tabla de salidas la hace el grupo)

> [!NOTE]
> Checkear si o si que los siguientes comandos con los siguientes valores
> en mvar, devuelvan lo determinado en la tabla del pdf
