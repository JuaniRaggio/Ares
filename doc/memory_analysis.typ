#set page(margin: 2.2cm, numbering: "1")
#set text(font: "New Computer Modern", size: 11pt, lang: "es")
#set heading(numbering: "1.1")
#show raw.where(block: true): it => block(
  fill: luma(245), inset: 8pt, radius: 4pt, width: 100%, it,
)
#set par(justify: true)

#align(center)[
  #text(17pt, weight: "bold")[Consumo de memoria por proceso en AresOS] \
  #text(11pt)[Modelo analitico del heap y validacion experimental] \
  #v(2pt)
  #text(9pt, style: "italic")[Sistemas Operativos -- ITBA]
]

#v(6pt)

= Objetivo

Cada proceso reserva memoria del heap del kernel en el momento de su creacion.
Queremos construir un *modelo exacto* que prediga el valor `Used` que reporta el
comando `mem`, derivarlo a partir del codigo, y validarlo midiendo el sistema
real. Como caso de estudio reducimos el stack de usuario de $16$ KiB a $8$ KiB y
demostramos que el ahorro observado coincide con el predicho al byte.

= Que asigna el kernel al crear un proceso

La funcion `process_create` realiza exactamente cuatro asignaciones de heap por
proceso: el stack de kernel, el stack de usuario, la copia de `argv` y el area de
estado FPU/SSE.

```c
uint8_t *kstack = (uint8_t *)mem_alloc(KERNEL_STACK_SIZE);   // 16 KiB
uint8_t *ustack = (uint8_t *)mem_alloc(USER_STACK_SIZE);     //  8 KiB
char   **argv_copy = clone_argv(argc, argv);                 // 1 bloque
uint8_t *fpu_area  = alloc_fpu_area();                        // FPU_AREA_SIZE
```

Los tamanos solicitados estan fijados por estas constantes:

```c
#define KERNEL_STACK_SIZE (4096 * 4) /* 16384 */
#define USER_STACK_SIZE   (4096 * 2) /*  8192 */
#define FPU_AREA_SIZE     512
```

La copia de argumentos empaqueta el arreglo de punteros y las cadenas en *un
unico bloque*:

```c
uint64_t total = (argc + 1) * sizeof(char *);
for (uint64_t i = 0; i < argc; i++)
        total += strlen(argv[i]) + 1;
char **copy = (char **)mem_alloc(total);
```

es decir, el tamano pedido por `clone_argv` es
$ s_("argv") = 8(c+1) + sum_(i=1)^(c) (|a_i| + 1), $
donde $c$ es `argc` y $|a_i|$ la longitud de cada argumento.

= El asignador: redondeo y contabilidad

El allocator de primer ajuste (`multi_region_heap.c`) antepone a cada bloque una
cabecera y redondea el pedido al multiplo de alineacion:

```c
size_t needed = align_up(size + header_size);
...
heap_status.available_heap_space_bytes -= block->block_size;
heap_status.successful_allocations++;
```

con las constantes

```c
#define HEAP_ALIGNMENT 16
header_size = align_up(sizeof(block_list_t)); // block_list_t = {ptr, size_t} = 16
```

por lo que $h = "header\_size" = 16$ y la alineacion es $A = 16$. Definimos la
funcion de redondeo
$ "align"(x) = ceil(x / A) dot A = ceil(x / 16) dot 16. $

Cuando el bloque elegido no se puede partir (resto $< h + A$) se entrega entero;
en una region recien inicializada el resto siempre es enorme, de modo que cada
asignacion parte el bloque y su huella real es exactamente `needed`. Definimos
entonces el *costo de un bloque* de tamano solicitado $s$ como
$ B(s) = "align"(s + h) = ceil((s + 16) / 16) dot 16. $

Ademas `Used` (el campo `occupied_heap_space_bytes`) se calcula como
$ "Used" = "total" - "available", $
y como `available` se decrementa en `block_size` por cada asignacion viva, se
cumple que `Used` es la *suma de las huellas de todos los bloques vivos*. Esta es
la identidad central que usaremos.

= Huella de un proceso

Evaluamos $B$ en cada uno de los cuatro bloques. Para un proceso lanzado como
`loop 999` se tiene $c = 1$, $a_1 = $ `"999"`, luego
$ s_("argv") = 8(1+1) + (3 + 1) = 16 + 4 = 20. $

#table(
  columns: (auto, auto, auto),
  align: (left, center, right),
  table.header([*Bloque*], [*$s$ (pedido)*], [*$B(s)$ (huella)*]),
  [Stack de kernel], [$16384$], [$"align"(16400) = 1025 dot 16 = 16400$],
  [Stack de usuario], [$8192$],  [$"align"(8208) = 513 dot 16 = 8208$],
  [Copia de `argv`],  [$20$],    [$"align"(36) = 3 dot 16 = 48$],
  [Area FPU],         [$512$],   [$"align"(528) = 33 dot 16 = 528$],
)

La huella total de un proceso `loop` es
$ P_("loop") = B(16384) + B(8192) + B(20) + B(512) = 16400 + 8208 + 48 + 528 = bold(25184) "bytes." $

= Modelo del estado base

Tres entidades viven de forma permanente desde el arranque:

- *shell* (pid 0): usa el stack estatico del boot (en `ps` su `STACK BASE` es
  $0$), por lo que solo asigna su area FPU: $B(512) = 528$.
- *idle* (pid 1): creado con `process_create` y `argc = 0` (sin `argv`), aporta
  $ I = B(16384) + B(8192) + B(512) = 16400 + 8208 + 528 = 25136. $
- *cache de slab* de semaforos: `create_cache` hace `mem_alloc(sizeof(slab_cache_t))`
  con `slab_cache_t = {size_t, void*, size_t}` $= 24$ bytes, luego
  $ S = B(24) = "align"(40) = 48. $

El estado residente es
$ R = B(512) + I + S = 528 + 25136 + 48 = 25712. $

Cuando ejecutamos `mem` para tomar la medicion, `mem` es a su vez un proceso
(`argc = 0`), con huella
$ M = B(16384) + B(8192) + B(512) = 25136. $

Por lo tanto, con $n$ procesos `loop` vivos en segundo plano, el valor reportado
por una invocacion de `mem` es
$ "Used"(n) = R + M + n dot P_("loop") = 25712 + 25136 + 25184 n = bold(50848 + 25184 n). $

Analogamente, la cantidad de bloques vivos es $L(n) = 8 + 4n$ (los $8$ del estado
base mas `mem`: shell $1$, idle $3$, slab $1$, mem $3$; mas $4$ por cada `loop`).

= Validacion experimental

Se midio sobre QEMU en un arranque limpio: `mem`, luego `loop 999 &` repetido, y
una invocacion de `mem` por paso. El allocator no registra ningun byte en vuelo
porque `loop` no asigna memoria tras crearse (su espera es activa y `printf` usa
buffer estatico).

#table(
  columns: (auto, auto, auto, auto, auto),
  align: (center, center, center, center, center),
  table.header(
    [*$n$*], [*`Used` predicho*], [*`Used` medido*], [*$Delta$ medido*], [*vivos $L$*],
  ),
  [$0$], [$50848$],  [$50848$],  [---],     [$8$],
  [$1$], [$76032$],  [$76032$],  [$25184$], [$12$],
  [$2$], [$101216$], [$101216$], [$25184$], [$16$],
)

La prediccion coincide *exactamente* en los tres puntos, el incremento es
constante e igual a $P_("loop") = 25184$, y los bloques vivos siguen $L(n) = 8 + 4n$
(verificado con $"allocs" - "frees"$). Como corroboracion independiente, en `ps` la
diferencia entre los `STACK BASE` de dos `loop` consecutivos es
$ "0x1576f0" - "0x151490" = "0x6260" = 25184, $
es decir, los procesos quedan contiguos a exactamente una huella de distancia.

= Liberacion al terminar: no hay fuga

`process_free_resources` libera los cuatro bloques cuando el proceso es
cosechado:

```c
mem_free(pcb->kernel_stack_base);
mem_free(pcb->user_stack_base);
mem_free(pcb->argv_copy);
mem_free(pcb->fpu_area);
```

Partiendo de un `loop` vivo ($"Used" = 76032$) y ejecutando `kill <pid>`:

#table(
  columns: (auto, auto, auto),
  align: (left, center, center),
  table.header([*Estado*], [*`Used`*], [*vivos $L$*]),
  [`loop` vivo],         [$76032$], [$12$],
  [tras `kill`],         [$50848$], [$8$],
)

Se recuperan $76032 - 50848 = 25184$ bytes, es decir la huella completa del
proceso, y los bloques vivos vuelven a $8$. La prueba de estres lo confirma a
otra escala: `test_mm` ejecuta su lazo `malloc`/`free` y tras $approx 8.3$ millones
de operaciones los bloques vivos permanecen en $8$ (los del estado base). Si el
lazo perdiera aun un solo bloque por iteracion, los vivos estarian en millones.

= Efecto de reducir el stack de usuario

Antes del cambio ambos stacks median $16$ KiB. La huella por proceso era
$ P_("old") = B(16384) + B(16384) + B(20) + B(512) = 16400 + 16400 + 48 + 528 = 33376. $

Con el stack de usuario en $8$ KiB,
$ P_("new") = 25184, quad "ahorro" = P_("old") - P_("new") = 33376 - 25184 = bold(8192) "bytes" = 8 "KiB," $

exactamente la reduccion aplicada: como $16384$ y $8192$ son ambos multiplos de
$16$, el termino de cabecera $h$ se cancela en la diferencia
$B(16384) - B(8192) = 16400 - 8208 = 8192$. Para `MAX_PROCESSES` $= 32$ el ahorro
maximo agregado es $32 dot 8192 = 262144$ bytes $= 256$ KiB.

= Conclusion

El modelo $"Used"(n) = 50848 + 25184 n$ se deriva integramente del codigo (las
cuatro asignaciones de `process_create`, el redondeo $B(s) = "align"(s + 16)$ y la
identidad $"Used" = "total" - "available"$) y reproduce las mediciones al byte. El
incremento por proceso es exactamente $P_("loop") = 25184$ bytes, se libera por
completo al terminar el proceso (no hay fuga) y la reduccion del stack de usuario
produce el ahorro exacto de $8192$ bytes por proceso predicho por el modelo.
