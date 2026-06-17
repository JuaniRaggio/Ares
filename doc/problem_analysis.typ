#set page(margin: 2.2cm, numbering: "1")
#set text(font: "New Computer Modern", size: 11pt, lang: "es")
#set heading(numbering: "1.1")
#show raw.where(block: true): it => block(
  fill: luma(245), inset: 8pt, radius: 4pt, width: 100%, it,
)
#set par(justify: true)

#align(center)[
  #text(17pt, weight: "bold")[Analisis de problemas y soluciones en AresOS] \
  #text(11pt)[Bugs reales contra falsos positivos: diagnostico, correccion y validacion] \
  #v(2pt)
  #text(9pt, style: "italic")[Sistemas Operativos -- ITBA]
]

#v(6pt)

= Objetivo y metodo

Una herramienta de analisis sugirio una lista de bugs y mejoras. Este documento
audita cada item contra el codigo real y lo clasifica en una de tres categorias:
*bug real* (se corrige), *falso positivo* (comportamiento intencional o correcto)
y *mejora fuera de alcance o diferida*. Para cada bug real se documenta la causa
raiz, la correccion y la validacion empirica en QEMU.

El criterio para separar un bug real de un falso positivo es conservador: solo se
clasifica como bug aquello que produce un estado incorrecto observable (deadlock,
corrupcion, perdida de recursos) bajo una secuencia de ejecucion alcanzable.

= Bug A: condicion de carrera en `process_wait`

== Diagnostico

La herramienta marco una ventana entre el chequeo del lazo y el bloqueo del
padre. El analisis confirma que el bug es *real*, y la causa raiz esta en la
configuracion de `syscall`: el MSR FMASK se inicializa en cero.

```asm
mov ecx, 0xC0000084   ; IA32_FMASK
xor eax, eax
xor edx, edx
wrmsr                 ; FMASK = 0: syscall no enmascara ningun bit de RFLAGS
```

Con FMASK $= 0$, el flag de interrupciones (IF) permanece habilitado durante todo
el handler de syscall. Por lo tanto un tick del timer puede interrumpir
`process_wait` entre el test del estado del hijo y la transicion del padre a
`BLOCKED`. La version original escribia `BLOCKED` con interrupciones encendidas:

```c
while (target->state != PROCESS_ZOMBIE && target->state != PROCESS_DEAD) {
        current->waiting_for = pid;
        current->state       = PROCESS_BLOCKED;  // <- tick aqui pierde el wakeup
        scheduler_yield();
        _hlt();
}
```

== Secuencia que produce el deadlock

#table(
  columns: (auto, 1fr),
  align: (left, left),
  table.header([*Paso*], [*Evento*]),
  [1], [El padre evalua la condicion: el hijo aun vive, entra al cuerpo.],
  [2], [Un tick del timer interrumpe antes de `state = BLOCKED`.],
  [3], [El scheduler corre al hijo; el hijo hace `process_exit` y llama a `wake_waiters`.],
  [4], [`wake_waiters` solo despierta procesos ya en `BLOCKED`; el padre todavia esta `RUNNING`, no matchea. El hijo queda `ZOMBIE`.],
  [5], [El padre retoma, se pone `BLOCKED` y duerme. Nadie volvera a despertarlo: deadlock.],
)

La evidencia de que la ventana es alcanzable es que `sem_wait` ya se protegia con
`cli` para exactamente este patron, mientras que `process_wait` no lo hacia.

== Correccion

Se hace el test del estado del hijo y la transicion a `BLOCKED` de forma atomica
con interrupciones apagadas, replicando la disciplina de `sem_wait`:

```c
for (;;) {
        uint64_t flags = irq_save();
        if (target->pid != pid || target->state == PROCESS_ZOMBIE ||
            target->state == PROCESS_DEAD) {
                irq_restore(flags);
                break;
        }
        current->waiting_for = pid;
        current->state       = PROCESS_BLOCKED;
        irq_restore(flags);

        scheduler_yield();
        while (current->state == PROCESS_BLOCKED)
                _hlt();
}
```

Si el hijo termina dentro de la seccion critica, el siguiente giro del lazo lee
`ZOMBIE` y sale; si termina despues, `wake_waiters` encuentra al padre ya en
`BLOCKED` y lo despierta. La ventana de perdida desaparece.

== Validacion

En QEMU, ejecutar `div 6 2` en foreground produce `6 / 2 = 3` y devuelve el
prompt: la shell espera con `process_wait`, cosecha el hijo y continua. En una
sesion multi-comando la shell nunca se colgo.

= Bug B: el manejo de excepciones no terminaba el proceso

== Diagnostico

*Bug real* para un kernel multiproceso. El macro `exceptionHandler` reescribia la
direccion de retorno con la entrada de userland y hacia `iretq`, reiniciando la
shell sin importar que proceso fallo ni liberar sus recursos:

```asm
popState
call getStackBase
mov [rsp+24], rax     ; nuevo RSP = stack base
mov rax, userland
mov [rsp], rax        ; nuevo RIP = 0x400000 (reinicia userland)
sti
iretq
```

Si fallaba un proceso distinto de la shell, los demas PCB quedaban en un estado
inconsistente y los recursos del proceso culpable se filtraban.

== Correccion

El dispatcher en C termina el proceso culpable cuando no es la shell. Como
`process_exit` no retorna (el scheduler cosecha al zombie en el proximo tick), la
cola del macro -el `iretq` a `0x400000`- solo se alcanza para la shell, que se
sigue recuperando en el lugar:

```c
if (pid != SHELL_PID)
        process_exit(KILLED_EXIT_CODE);
```

Este camino reutiliza la salida normal de proceso (cleanup de pipes, semaforos,
stacks), por lo que no introduce hazards nuevos.

== Feedback en pantalla

Ademas del volcado de registros, tras una excepcion se informa al usuario que
proceso fallo y que accion tomo el kernel, por feedback y para debugging. El
mensaje se imprime despues del dump (no antes) para que sobreviva al limpiado de
pantalla por desborde:

```c
ncPrint("\nFaulting process: ", VGA_CYAN);
ncPrint(pcb != NULL ? pcb->name : "unknown", VGA_WHITE);
ncPrint(" (pid ", VGA_CYAN); ncPrintDec((uint64_t)pid); ncPrint(")\n", VGA_CYAN);
// Action: process terminated by the kernel; the rest of the system keeps running.
```

== Validacion

`div 1 0` y `opcode`, ahora ejecutados como procesos (ver seccion 4), disparan
las excepciones 0 y 6 con selectores de usuario (`CS = 0x23`, `SS = 0x1B`),
confirmando que el fallo ocurre en ring 3. La pantalla muestra el dump, la linea
`Faulting process: div (pid 2)` y `Action: process terminated by the kernel`. Al
presionar una tecla, la shell vuelve al prompt sin reiniciarse.

= Bug C: division por cero en `div` (falso positivo)

La herramienta marco que `div` no valida el divisor. Esto es un *falso positivo*:
el comando `div` existe precisamente para demostrar el handler de la excepcion 0,
y validar el divisor eliminaria el unico vector de prueba.

La preocupacion legitima detras del reporte -que la excepcion "rompe el flujo del
usuario"- se resuelve de raiz con el Bug B mas la migracion de `div` a proceso
(seccion 4): hoy `div 1 0` falla en un proceso aislado que el kernel mata
limpiamente, sin afectar a la shell ni al resto del sistema. No se removio el
demo.

= Mejora estructural: comandos como procesos

Aprovechando que el sistema ya soporta procesos, se migraron de built-ins a
aplicaciones aquellos comandos que son computo autocontenido o de larga duracion:
`div`, `opcode`, `tron`, `printmem` y `benchmark`. Un comando se mantiene como
built-in solo si lee o escribe el estado privado de la shell, el mismo criterio
que usa Unix (`cd`, `exit`, `history`).

#table(
  columns: (auto, auto, 1fr),
  align: (left, center, left),
  table.header([*Comando*], [*Tipo*], [*Motivo*]),
  [`div`, `opcode`],        [app],      [computo puro / disparo de excepcion: ahora aislados],
  [`tron`, `benchmark`],    [app],      [interactivos y de larga duracion],
  [`printmem`],             [app],      [computo puro sobre memoria via syscall],
  [`help`, `man`, `history`], [built-in], [leen tablas o el historial de la shell],
  [`time`, `exit`],         [built-in], [estado de la shell / control del loop],
  [`clear`, `cursor`, `inforeg`, colores], [built-in], [estado global trivial, no justifica un proceso],
)

Beneficio directo: un fallo en cualquiera de las apps migradas queda contenido al
proceso (Bug B), en lugar de tumbar la shell.

= Items diferidos o fuera de alcance

#table(
  columns: (auto, auto, 1fr),
  align: (left, center, left),
  table.header([*Sugerencia*], [*Estado*], [*Justificacion*]),
  [Scroll real en video],     [diferido],        [Hoy se limpia la pantalla al desbordar. Mejora de UX real; surgio al mostrar el dump de excepciones. Candidato a futuro.],
  [Sleep en lugar de busy-wait en sonido], [diferido], [Requiere una syscall de sleep que bloquee el proceso. Baja prioridad.],
  [Paging / proteccion de memoria], [fuera de alcance], [El modelo es flat con identity mapping por diseno. No es un bug; esfuerzo muy alto.],
  [Pipes multiples],          [diferido],        [Un solo pipe cumple el requisito. Generalizar el parsing es una extension opcional.],
)

= Conclusion

De los tres bugs reportados, dos eran reales (`A` y `B`) y se corrigieron con
validacion empirica; el tercero (`C`) era un falso positivo cuya preocupacion de
fondo se neutralizo con la migracion de comandos a procesos. Las cuatro mejoras
sugeridas son legitimas pero quedan diferidas o fuera de alcance: ninguna es un
defecto de correctitud. El sistema queda sin deadlocks conocidos en `wait`, con
aislamiento de fallos por proceso y con feedback de excepciones util tanto para
el usuario como para el debugging.
