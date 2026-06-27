# Cambios desde la entrega

Todo lo posterior son correcciones de robustez, sincronizacion y feedback 
al usuario; no hay funcionalidad nueva. Resumen por subsistema.

## Scheduler
- Se elimino `scheduler_yield()`, era una funcion vacia que se llamaba en
  varios lugares y no tenia implementacion, para ceder CPU se usa
  `_yield_now()` directo.
- Seleccion por prioridad con creditos: cada ronda un proceso recibe 
  `priority` creditos; se elige al proceso READY de mayor prioridad que 
  aun tiene credito, y los creditos se recargan solo cuando todos los listos
  agotaron los suyos. La prioridad queda visible en el orden de finalizacion
  sin generar inanicion (todo proceso corre al menos una vez por ronda).

## Semaforos
- re-chequeo: el valor nunca es negativo y se re-testea al despertar. 
  Corrige la race condition de `test_sync` (corrige valor distinto de 0)
- Reference counting: solo el ultimo `close` destruye el semaforo.
- Al morir un proceso se liberan los semaforos que dejo abiertos
  (`sem_release_process_refs`), consistente con el resto de sus recursos.
- `sem_close` rechaza cerrar un semaforo que el proceso no abrio (no toca el
  refcount de algo ajeno; evita que un proceso destruya un semaforo en uso).
- Validacion de `sem_id` vacio/NULL y `#pragma once` en el header.

## Procesos
- `MAX_PROCESSES` a 64.
- Reclamo total de recursos al morir (`process_free_resources`): kernel stack,
  user stack, FPU area, `argv`, memoria pedida por `malloc` (lista
  `user_allocs`) y semaforos abiertos. Un proceso matado no filtra recursos.
- Reparenting de huerfanos a la shell y reap de zombies sin waiter (no quedan
  PCBs ni stacks colgados).
- `kill` / `block` / `nice` rechazan a la shell y a idle (matar idle colgaba
  todo el sistema).
- Fix de race con pipes en exit/kill: se marca ZOMBIE y se desconectan los fds
  del pipe antes de despertar al peer, para que este detecte EOF en vez de
  re-bloquearse.
- Fix de race TOCTOU: se toma `irq_save` antes de `process_get` en
  `kill` / `block` / `unblock` / `nice`, para que un tick no pueda reapear y
  reusar el slot entre encontrar el PCB y usarlo.
- Feedback de error al crear procesos (tabla llena / sin memoria).

## Memory managers
- Buddy: fix de `MAX_POOLS`. Estaba en `HEAP_REGION_COUNT * 2` (=4) y la region
  chica agotaba los pools, dejando sin registrar la region grande de 32 MB; el
  heap del buddy quedaba en ~2.7 MB. Se corrigio la cota a
  `HEAP_REGION_COUNT * NUM_ORDERS`; ahora el heap completo (~36 MB) queda
  disponible.
- `sys_malloc` / `sys_free` atomicos (`irq_save` alrededor de reservar+enlazar y
  de desenlazar+liberar). Cierra una fuga que aparecia al matar un proceso justo
  durante un `malloc`: el bloque quedaba reservado pero sin contabilizar en
  `user_allocs`. Afecta a los dos allocators (esta en la capa de syscalls).

## Pipes
- Limpieza correcta al morir un proceso y manejo de EOF/kill: un flag
  `had_writer` distingue "el writer todavia no se creo" de "el writer termino",
  evitando EOF prematuro y cuelgues del lector.

## Teclado
- Buffer circular: al llenarse descarta el byte mas viejo, asi el input
  pendiente no se confunde con "no hay input".

## Syscalls
- Chequeo de NULL en las syscalls que escriben por puntero.
- Halt cooperativo: idle cede la CPU si hay trabajo listo y hace `hlt` solo si
  no lo hay (baja la latencia de teclado).

## Apps de usuario
- mvar: pacing en tiempo real cooperativo (cede CPU en vez de espera activa, que
  generaba lag y races); cada writer/reader abre sus propios semaforos y corre
  en foreground (Ctrl+C los frena); validacion correcta (hasta 26 writers, una
  letra A-Z cada uno; hasta 8 readers, un color cada uno) con mensajes precisos.
- ps: padding y alineacion de columnas, snapshot de 64 procesos.
- nice: valida la prioridad en el rango [1, 4] con mensaje acorde.

## Tests
- test_sync: parametrizado a `test_sync <pares> <n> <use_sem>`; los workers
  corren en foreground (Ctrl+C los mata, ya no quedan huerfanos contaminando la
  variable compartida en corridas posteriores); mejora grande de tiempo de
  ejecucion.
- help: corregida la firma de `test_sync`.
