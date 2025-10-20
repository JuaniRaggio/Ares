#set document(
  title: "ARES - Roadmap del Proyecto",
  author: ("Juan Ignacio Raggio", "Enzo Canelo", "Matias Sanchez"),
)

#set page(
  paper: "a4",
  margin: (
    top: 2.5cm,
    bottom: 2.5cm,
    left: 2cm,
    right: 2cm,
  ),
  numbering: "1",
  number-align: bottom + right,

  header: [
    #set text(size: 9pt, fill: gray)
    #grid(
      columns: (1fr, 1fr),
      align: (left, right),
      [Raggio, Canelo, Sanchez],
      [#datetime.today().display("[day]/[month]/[year]")]
    )
    #line(length: 100%, stroke: 0.5pt + gray)
  ],

  footer: context [
    #set text(size: 9pt, fill: gray)
    #line(length: 100%, stroke: 0.5pt + gray)
    #v(0.2em)
    #align(center)[
      Pagina #counter(page).display() de #counter(page).final().first()
    ]
  ]
)

#set text(
  font: "New Computer Modern",
  size: 11pt,
  lang: "es",
  hyphenate: true,
)

#set par(
  justify: true,
  leading: 0.65em,
  first-line-indent: 0em,
  spacing: 1.2em,
)

#set heading(numbering: "1.1")
#show heading.where(level: 1): set text(size: 18pt, weight: "bold")
#show heading.where(level: 2): set text(size: 14pt, weight: "bold")
#show heading.where(level: 3): set text(size: 12pt, weight: "bold")

#show heading: it => {
  v(0.5em)
  it
  v(0.3em)
}

#set list(indent: 1em, marker: ("•", "◦", "▪"))
#set enum(indent: 1em, numbering: "1.a.")

#show raw.where(block: false): box.with(
  fill: luma(240),
  inset: (x: 3pt, y: 0pt),
  outset: (y: 3pt),
  radius: 2pt,
)

#show raw.where(block: true): block.with(
  fill: luma(240),
  inset: 10pt,
  radius: 4pt,
  width: 100%,
)

#show link: underline

// Funciones de ayuda
#let nota(contenido) = {
  block(
    fill: rgb("#E3F2FD"),
    stroke: rgb("#1976D2") + 1pt,
    inset: 10pt,
    radius: 4pt,
    width: 100%,
  )[
    #text(weight: "bold", fill: rgb("#1976D2"))[Nota:] #contenido
  ]
}

#let importante(contenido) = {
  block(
    fill: rgb("#FFF3E0"),
    stroke: rgb("#F57C00") + 1pt,
    inset: 10pt,
    radius: 4pt,
    width: 100%,
  )[
    #text(weight: "bold", fill: rgb("#F57C00"))[Importante:] #contenido
  ]
}

#let prioridad(nivel) = {
  let colores = (
    "CRITICA": (fill: rgb("#FFEBEE"), stroke: rgb("#D32F2F"), text_color: rgb("#D32F2F")),
    "ALTA": (fill: rgb("#FFF3E0"), stroke: rgb("#F57C00"), text_color: rgb("#F57C00")),
    "MEDIA": (fill: rgb("#FFF9C4"), stroke: rgb("#F9A825"), text_color: rgb("#F9A825")),
    "BAJA": (fill: rgb("#E8F5E9"), stroke: rgb("#388E3C"), text_color: rgb("#388E3C")),
  )

  let config = colores.at(nivel)

  box(
    fill: config.fill,
    stroke: config.stroke + 1pt,
    inset: (x: 6pt, y: 2pt),
    radius: 3pt,
  )[
    #text(weight: "bold", fill: config.text_color, size: 9pt)[#nivel]
  ]
}

#let estado(tipo) = {
  let configs = (
    "PENDIENTE": (fill: rgb("#E0E0E0"), text_color: rgb("#424242"), icon: "○"),
    "EN_PROGRESO": (fill: rgb("#BBDEFB"), text_color: rgb("#1976D2"), icon: "◐"),
    "COMPLETADO": (fill: rgb("#C8E6C9"), text_color: rgb("#388E3C"), icon: "●"),
    "PARCIAL": (fill: rgb("#FFF9C4"), text_color: rgb("#F57C00"), icon: "◑"),
  )

  let config = configs.at(tipo)

  box(
    fill: config.fill,
    inset: (x: 5pt, y: 1pt),
    radius: 2pt,
  )[
    #text(weight: "bold", fill: config.text_color, size: 9pt)[#config.icon]
  ]
}

#let tarea(titulo, descripcion, prioridad_nivel, estado_actual, dependencias: none) = {
  block(
    fill: luma(250),
    stroke: luma(180) + 0.5pt,
    inset: 12pt,
    radius: 4pt,
    width: 100%,
    breakable: false,
  )[
    #grid(
      columns: (1fr, auto, auto),
      align: (left, right, right),
      column-gutter: 8pt,
      [#text(weight: "bold", size: 11pt)[#titulo]],
      prioridad(prioridad_nivel),
      estado(estado_actual),
    )

    #v(0.3em)
    #text(size: 10pt)[#descripcion]

    #if dependencias != none [
      #v(0.3em)
      #text(size: 9pt, fill: gray, style: "italic")[
        Dependencias: #dependencias
      ]
    ]
  ]
  v(0.5em)
}

// ====================================
// PORTADA
// ====================================

#align(center)[
  #v(2em)
  #image("ITBAgua.png", width: 40%)
  #v(1em)
  #text(size: 24pt, weight: "bold")[Arquitectura de Computadoras]
  #v(0.5em)
  #text(size: 20pt, weight: "bold")[ROADMAP DEL PROYECTO]
  #v(0.5em)
  #text(size: 18pt)[ARES - ARES Recursive Experimental System]
  #v(1em)
  #text(size: 12pt)[
    *Autores:* \
    Juan Ignacio Raggio \
    Enzo Canelo \
    Matias Sanchez
  ]
  #v(0.5em)
  #text(size: 12pt, fill: gray)[
    Segundo Cuatrimestre 2025 \
    #datetime.today().display("[day]/[month]/[year]")
  ]
  #v(2em)
]

#line(length: 100%, stroke: 1pt)
#v(1em)

#pagebreak()

// ====================================
// CONTENIDO
// ====================================

= Resumen Ejecutivo del Roadmap

Este documento presenta el roadmap detallado para el desarrollo del proyecto ARES, organizado por fases y prioridades. El proyecto se divide en 4 fases principales, cada una con objetivos claros y medibles.

#importante[
Este roadmap está diseñado para ser seguido de manera secuencial. Las tareas de prioridad CRITICA deben completarse antes de avanzar a la siguiente fase.
]

== Estado Actual del Proyecto

Según el análisis del código base existente:

#grid(
  columns: (1fr, 1fr),
  column-gutter: 1em,
  row-gutter: 0.5em,

  [*Componentes Existentes:*],
  [*Estado:*],

  [Bootloader (Pure64)], estado("PARCIAL"),
  [Detección VESA], estado("COMPLETADO"),
  [Configuración RTC], estado("COMPLETADO"),
  [Memory Map (E820)], estado("COMPLETADO"),
  [Kernel básico], estado("PENDIENTE"),
  [Drivers], estado("PENDIENTE"),
  [Shell], estado("PENDIENTE"),
  [TRON 2D], estado("PENDIENTE"),
)

#v(1em)

== Objetivos por Fase

=== Fase 1: Infraestructura Base del Kernel
*Objetivo:* Tener un kernel funcional con drivers básicos y sistema de interrupciones.

*Duración estimada:* 3-4 semanas

*Entregable:* Kernel que arranca, inicializa hardware y responde a interrupciones.

=== Fase 2: User Space y API del Kernel
*Objetivo:* Implementar syscalls y shell interactiva funcional.

*Duración estimada:* 2-3 semanas

*Entregable:* Shell que acepta comandos y muestra información del sistema.

=== Fase 3: Implementación de TRON 2D
*Objetivo:* Juego TRON completamente funcional en modo 2D.

*Duración estimada:* 3-4 semanas

*Entregable:* TRON jugable con modos 1 y 2 jugadores.

=== Fase 4: Extensión a 3D (Opcional)
*Objetivo:* Motor 3D básico y TRON en 3D.

*Duración estimada:* 4-6 semanas

*Entregable:* TRON 3D con gráficos wireframe o rasterizados.

#pagebreak()

= Fase 1: Infraestructura Base del Kernel

== 1.1 Inicialización del Kernel

#tarea(
  "Estructura básica del kernel en C",
  "Crear el punto de entrada del kernel (kernel_main), inicialización de segmentos de datos (.bss, .data), configuración del stack del kernel.",
  "CRITICA",
  "PENDIENTE",
)

#tarea(
  "Configuración de GDT (Global Descriptor Table)",
  "Definir descriptores de segmento para code/data en ring 0 y ring 3. Necesario para separación kernel/user space.",
  "CRITICA",
  "PENDIENTE",
  dependencias: "Estructura básica del kernel",
)

#tarea(
  "Mapeo del framebuffer VESA",
  "Leer la información VESA dejada por Pure64 y mapear el framebuffer a memoria virtual del kernel.",
  "ALTA",
  "PENDIENTE",
  dependencias: "Estructura básica del kernel",
)

== 1.2 Sistema de Interrupciones

#tarea(
  "Configuración de IDT (Interrupt Descriptor Table)",
  "Crear tabla de 256 entradas para interrupciones y excepciones. Implementar función para registrar handlers.",
  "CRITICA",
  "PENDIENTE",
  dependencias: "GDT configurado",
)

#tarea(
  "Handlers de excepciones (0-31)",
  "Implementar handlers para Division Error, Page Fault, General Protection Fault, Invalid Opcode, etc. Mostrar información de debug.",
  "CRITICA",
  "PENDIENTE",
  dependencias: "IDT configurado",
)

#tarea(
  "Remapeo del PIC (8259)",
  "Remapear IRQs del PIC para que no colisionen con excepciones del CPU (IRQ0 -> INT 32).",
  "CRITICA",
  "PENDIENTE",
  dependencias: "IDT configurado",
)

#tarea(
  "Handler genérico de IRQs",
  "Implementar dispatcher de IRQs que envíe EOI al PIC correctamente.",
  "CRITICA",
  "PENDIENTE",
  dependencias: "PIC remapeado",
)

== 1.3 Gestión de Memoria

#tarea(
  "Lectura del mapa E820",
  "Parsear el memory map E820 dejado por el bootloader para identificar regiones de memoria disponibles.",
  "ALTA",
  "PENDIENTE",
  dependencias: "Estructura básica del kernel",
)

#tarea(
  "Allocator básico (bump allocator)",
  "Implementar kmalloc/kfree simple para gestión de memoria dinámica del kernel. Puede ser mejorado después.",
  "ALTA",
  "PENDIENTE",
  dependencias: "Mapa E820 leído",
)

#tarea(
  "Configuración de page tables (MMU)",
  "Configurar paginación de 4 niveles (PML4) para separar kernel space de user space. Identity mapping para el kernel.",
  "MEDIA",
  "PENDIENTE",
  dependencias: "Allocator básico",
)

== 1.4 Driver de Video

#tarea(
  "Inicialización del driver de video",
  "Leer información VESA, configurar estructura global con framebuffer, width, height, pitch, bpp.",
  "CRITICA",
  "PENDIENTE",
  dependencias: "Framebuffer mapeado",
)

#tarea(
  "Primitiva: video_put_pixel",
  "Implementar función para escribir pixel individual en el framebuffer.",
  "CRITICA",
  "PENDIENTE",
  dependencias: "Driver inicializado",
)

#tarea(
  "Primitiva: video_clear_screen",
  "Implementar limpieza de pantalla (rellenar con color sólido).",
  "ALTA",
  "PENDIENTE",
  dependencias: "video_put_pixel",
)

#tarea(
  "Primitiva: video_draw_line (Bresenham)",
  "Implementar algoritmo de Bresenham para dibujar líneas.",
  "ALTA",
  "PENDIENTE",
  dependencias: "video_put_pixel",
)

#tarea(
  "Primitiva: video_draw_rect",
  "Implementar dibujo de rectángulos (rellenos y sin rellenar).",
  "MEDIA",
  "PENDIENTE",
  dependencias: "video_draw_line",
)

#tarea(
  "Implementar double buffering",
  "Alocar back buffer en memoria, implementar video_swap_buffers para copiar back->front sin flickering.",
  "ALTA",
  "PENDIENTE",
  dependencias: "video_clear_screen",
)

== 1.5 Driver de Teclado

#tarea(
  "Handler de IRQ1 (teclado)",
  "Implementar handler de interrupción que lea scancode del puerto 0x60 y envíe EOI.",
  "CRITICA",
  "PENDIENTE",
  dependencias: "Handler genérico de IRQs",
)

#tarea(
  "Tabla de traducción scancode->ASCII",
  "Crear tabla de conversión de scancodes a caracteres ASCII.",
  "ALTA",
  "PENDIENTE",
  dependencias: "Handler IRQ1",
)

#tarea(
  "Buffer circular de teclado",
  "Implementar buffer para almacenar teclas presionadas (256 bytes circular).",
  "ALTA",
  "PENDIENTE",
  dependencias: "Tabla de traducción",
)

#tarea(
  "Funciones keyboard_get_char y keyboard_check_key",
  "API para leer caracteres del buffer y verificar si una tecla específica está presionada.",
  "ALTA",
  "PENDIENTE",
  dependencias: "Buffer circular",
)

== 1.6 Driver de Timer

#tarea(
  "Configurar PIT (8254) o usar RTC",
  "Configurar timer para generar interrupciones a frecuencia conocida (ej. 1000Hz). El código actual usa RTC a 1024Hz.",
  "ALTA",
  "PENDIENTE",
  dependencias: "Handler genérico de IRQs",
)

#tarea(
  "Contador de ticks",
  "Variable global que se incrementa en cada interrupción del timer.",
  "ALTA",
  "PENDIENTE",
  dependencias: "Timer configurado",
)

#tarea(
  "Función sys_get_ticks",
  "Retornar el contador de ticks actual (en milisegundos si es posible).",
  "MEDIA",
  "PENDIENTE",
  dependencias: "Contador de ticks",
)

#tarea(
  "Función sys_sleep",
  "Implementar delay bloqueante basado en ticks.",
  "BAJA",
  "PENDIENTE",
  dependencias: "sys_get_ticks",
)

== 1.7 Driver de Sonido (PC Speaker)

#tarea(
  "Función speaker_play",
  "Programar PIT canal 2 para generar frecuencia específica y habilitar speaker.",
  "BAJA",
  "PENDIENTE",
  dependencias: "Ninguna (puede hacerse en paralelo)",
)

#tarea(
  "Función speaker_stop",
  "Deshabilitar el PC speaker.",
  "BAJA",
  "PENDIENTE",
  dependencias: "speaker_play",
)

#pagebreak()

= Fase 2: User Space y API del Kernel

== 2.1 Sistema de Syscalls

#tarea(
  "Configurar syscall handler (INT 0x80 o SYSCALL)",
  "Implementar dispatcher de syscalls que lea número en RAX y argumentos en registros.",
  "CRITICA",
  "PENDIENTE",
  dependencias: "IDT configurado",
)

#tarea(
  "Tabla de syscalls",
  "Array de punteros a funciones para cada syscall implementado.",
  "CRITICA",
  "PENDIENTE",
  dependencias: "Syscall handler",
)

#tarea(
  "Validación de parámetros",
  "Verificar que punteros pasados por user space estén en rangos válidos.",
  "ALTA",
  "PENDIENTE",
  dependencias: "Tabla de syscalls",
)

== 2.2 Syscalls de I/O Básico

#tarea(
  "sys_getchar",
  "Syscall para leer un carácter del teclado (bloqueante).",
  "CRITICA",
  "PENDIENTE",
  dependencias: "Driver de teclado completo",
)

#tarea(
  "sys_putchar",
  "Syscall para escribir un carácter en pantalla (modo texto o gráfico).",
  "CRITICA",
  "PENDIENTE",
  dependencias: "Driver de video completo",
)

#tarea(
  "sys_read / sys_write",
  "Syscalls genéricos para lectura/escritura con file descriptors (stdin=0, stdout=1).",
  "ALTA",
  "PENDIENTE",
  dependencias: "sys_getchar, sys_putchar",
)

== 2.3 Syscalls de Video

#tarea(
  "sys_draw_pixel",
  "Wrapper de syscall para video_put_pixel.",
  "ALTA",
  "PENDIENTE",
  dependencias: "video_put_pixel",
)

#tarea(
  "sys_draw_line, sys_draw_rect",
  "Wrappers para primitivas gráficas.",
  "ALTA",
  "PENDIENTE",
  dependencias: "video_draw_line, video_draw_rect",
)

#tarea(
  "sys_clear_screen",
  "Wrapper para video_clear_screen.",
  "ALTA",
  "PENDIENTE",
  dependencias: "video_clear_screen",
)

#tarea(
  "sys_swap_buffers",
  "Wrapper para double buffering.",
  "ALTA",
  "PENDIENTE",
  dependencias: "Double buffering implementado",
)

#tarea(
  "sys_get_screen_info",
  "Retornar width y height de la pantalla.",
  "MEDIA",
  "PENDIENTE",
  dependencias: "Driver de video inicializado",
)

== 2.4 Syscalls de Sistema

#tarea(
  "sys_get_ticks",
  "Wrapper para obtener ticks del timer.",
  "MEDIA",
  "PENDIENTE",
  dependencias: "Contador de ticks",
)

#tarea(
  "sys_sleep",
  "Wrapper para delay.",
  "BAJA",
  "PENDIENTE",
  dependencias: "sys_sleep kernel",
)

#tarea(
  "sys_get_cpu_info",
  "Retornar información del CPU (usando CPUID).",
  "BAJA",
  "PENDIENTE",
  dependencias: "Ninguna",
)

#tarea(
  "sys_get_mem_info",
  "Retornar información de memoria disponible/usada.",
  "BAJA",
  "PENDIENTE",
  dependencias: "Gestión de memoria",
)

== 2.5 Librería de User Space

#tarea(
  "Wrappers de syscalls en C",
  "Crear funciones en C que invoquen syscalls con la convención correcta (libares.c).",
  "ALTA",
  "PENDIENTE",
  dependencias: "Sistema de syscalls",
)

#tarea(
  "Implementar printf/sprintf",
  "Parser de format strings y conversión de tipos (sin usar libc). Fundamental para debugging.",
  "CRITICA",
  "PENDIENTE",
  dependencias: "sys_putchar",
)

#tarea(
  "Implementar scanf/sscanf",
  "Parser de input formateado.",
  "MEDIA",
  "PENDIENTE",
  dependencias: "sys_getchar",
)

#tarea(
  "Funciones de string (strlen, strcmp, strcpy, etc.)",
  "Reimplementar funciones básicas de string.h",
  "ALTA",
  "PENDIENTE",
  dependencias: "Ninguna",
)

== 2.6 Shell Interactiva

#tarea(
  "Loop principal de la shell",
  "Mostrar prompt, leer línea de comando, ejecutar.",
  "CRITICA",
  "PENDIENTE",
  dependencias: "printf, scanf",
)

#tarea(
  "Parser de comandos",
  "Separar comando y argumentos (tokenización por espacios).",
  "ALTA",
  "PENDIENTE",
  dependencias: "Loop principal",
)

#tarea(
  "Tabla de comandos",
  "Array de estructuras con nombre, función, descripción.",
  "ALTA",
  "PENDIENTE",
  dependencias: "Parser de comandos",
)

#tarea(
  "Comando: help",
  "Mostrar lista de comandos disponibles.",
  "ALTA",
  "PENDIENTE",
  dependencias: "Tabla de comandos",
)

#tarea(
  "Comando: clear",
  "Limpiar la pantalla.",
  "MEDIA",
  "PENDIENTE",
  dependencias: "sys_clear_screen",
)

#tarea(
  "Comando: time",
  "Mostrar fecha/hora del RTC.",
  "MEDIA",
  "PENDIENTE",
  dependencias: "Driver RTC (leer CMOS)",
)

#tarea(
  "Comando: meminfo",
  "Mostrar información de memoria y registros.",
  "MEDIA",
  "PENDIENTE",
  dependencias: "sys_get_mem_info",
)

#tarea(
  "Comando: cpuinfo",
  "Mostrar información del CPU.",
  "BAJA",
  "PENDIENTE",
  dependencias: "sys_get_cpu_info",
)

#tarea(
  "Comando: benchmark",
  "Ejecutar benchmarks del sistema.",
  "BAJA",
  "PENDIENTE",
  dependencias: "Timer, video",
)

#pagebreak()

= Fase 3: Implementación de TRON 2D

== 3.1 Motor Gráfico 2D

#tarea(
  "Sistema de grid lógico",
  "Definir grid de juego (ej. 128x96) y funciones de conversión grid->screen.",
  "CRITICA",
  "PENDIENTE",
  dependencias: "Driver de video completo",
)

#tarea(
  "Estructura game_grid_t",
  "Array 2D para representar estado de cada celda (vacío, jugador1, jugador2, obstáculo).",
  "CRITICA",
  "PENDIENTE",
  dependencias: "Sistema de grid",
)

#tarea(
  "Función grid_to_screen",
  "Convertir coordenadas del grid a coordenadas de pantalla.",
  "ALTA",
  "PENDIENTE",
  dependencias: "Sistema de grid",
)

#tarea(
  "Paleta de colores del juego",
  "Definir constantes para colores de jugadores, background, UI, etc.",
  "MEDIA",
  "PENDIENTE",
  dependencias: "Ninguna",
)

== 3.2 Estructuras de Datos del Juego

#tarea(
  "Estructura player_t",
  "Definir posición, dirección, color, estado (vivo/muerto), score.",
  "CRITICA",
  "PENDIENTE",
  dependencias: "Ninguna",
)

#tarea(
  "Estructura game_state_t",
  "Estado global del juego: array de jugadores, grid, estado del juego (menu/playing/gameover), velocidad, ticks.",
  "CRITICA",
  "PENDIENTE",
  dependencias: "player_t, game_grid_t",
)

#tarea(
  "Función game_init",
  "Inicializar estado del juego: limpiar grid, posicionar jugadores, resetear scores.",
  "ALTA",
  "PENDIENTE",
  dependencias: "game_state_t",
)

== 3.3 Game Loop

#tarea(
  "Loop principal del juego",
  "Implementar game loop con fixed timestep: input -> update -> render.",
  "CRITICA",
  "PENDIENTE",
  dependencias: "game_init, sys_get_ticks",
)

#tarea(
  "Control de velocidad (FPS cap)",
  "Limitar actualizaciones del juego a N movimientos por segundo (ej. 20).",
  "ALTA",
  "PENDIENTE",
  dependencias: "Loop principal",
)

#tarea(
  "FPS counter",
  "Calcular y mostrar FPS en pantalla.",
  "BAJA",
  "PENDIENTE",
  dependencias: "Loop principal",
)

== 3.4 Input y Controles

#tarea(
  "Función handle_input",
  "Leer estado del teclado y actualizar dirección de jugadores.",
  "CRITICA",
  "PENDIENTE",
  dependencias: "keyboard_check_key",
)

#tarea(
  "Mapeo de teclas: Jugador 1 (WASD)",
  "W=arriba, S=abajo, A=izquierda, D=derecha.",
  "ALTA",
  "PENDIENTE",
  dependencias: "handle_input",
)

#tarea(
  "Mapeo de teclas: Jugador 2 (Flechas)",
  "UP/DOWN/LEFT/RIGHT.",
  "ALTA",
  "PENDIENTE",
  dependencias: "handle_input",
)

#tarea(
  "Validación de dirección (no 180 grados)",
  "Prevenir que jugador se mueva en dirección opuesta directamente.",
  "MEDIA",
  "PENDIENTE",
  dependencias: "handle_input",
)

#tarea(
  "Teclas generales: ESC, R",
  "ESC para pausar/volver a menú, R para reiniciar.",
  "MEDIA",
  "PENDIENTE",
  dependencias: "handle_input",
)

== 3.5 Física y Movimiento

#tarea(
  "Función update_player",
  "Actualizar posición del jugador según su dirección.",
  "CRITICA",
  "PENDIENTE",
  dependencias: "player_t",
)

#tarea(
  "Marcar trail en el grid",
  "Al moverse, marcar la posición anterior como ocupada por ese jugador.",
  "CRITICA",
  "PENDIENTE",
  dependencias: "update_player",
)

#tarea(
  "Wrap-around (opcional)",
  "Hacer que jugador reaparezca del otro lado al salir de los bordes.",
  "BAJA",
  "PENDIENTE",
  dependencias: "update_player",
)

== 3.6 Detección de Colisiones

#tarea(
  "Función check_collision",
  "Verificar si la nueva posición del jugador está ocupada (trail, pared, obstáculo).",
  "CRITICA",
  "PENDIENTE",
  dependencias: "game_grid_t",
)

#tarea(
  "Colisión con bordes",
  "Detectar cuando jugador sale de los límites del grid.",
  "ALTA",
  "PENDIENTE",
  dependencias: "check_collision",
)

#tarea(
  "Colisión con trails",
  "Detectar choque con rastro propio o del oponente.",
  "CRITICA",
  "PENDIENTE",
  dependencias: "check_collision",
)

#tarea(
  "Colisión con obstáculos",
  "Detectar choque con obstáculos generados.",
  "MEDIA",
  "PENDIENTE",
  dependencias: "check_collision, generación de obstáculos",
)

== 3.7 Renderizado

#tarea(
  "Función render_game",
  "Dibujar todo el estado del juego en el back buffer.",
  "CRITICA",
  "PENDIENTE",
  dependencias: "Double buffering",
)

#tarea(
  "Renderizar grid (opcional)",
  "Dibujar líneas de la grilla para visualización (puede deshabilitarse).",
  "BAJA",
  "PENDIENTE",
  dependencias: "render_game",
)

#tarea(
  "Renderizar trails",
  "Dibujar rastros de cada jugador como rectángulos de color.",
  "CRITICA",
  "PENDIENTE",
  dependencias: "render_game, video_draw_rect",
)

#tarea(
  "Renderizar jugadores",
  "Dibujar posición actual de cada jugador (rectángulo o círculo brillante).",
  "ALTA",
  "PENDIENTE",
  dependencias: "render_game",
)

#tarea(
  "Renderizar HUD",
  "Mostrar scores, FPS, barras de turbo en la parte superior.",
  "MEDIA",
  "PENDIENTE",
  dependencias: "render_game, printf gráfico (opcional)",
)

== 3.8 Sistema de Menús

#tarea(
  "Menú principal",
  "Mostrar opciones: 1P, 2P, Configuración, Salir.",
  "ALTA",
  "PENDIENTE",
  dependencias: "Primitivas gráficas",
)

#tarea(
  "Menú de configuración",
  "Ajustar velocidad, obstáculos, sonido, etc.",
  "MEDIA",
  "PENDIENTE",
  dependencias: "Menú principal",
)

#tarea(
  "Pantalla de Game Over",
  "Mostrar ganador y scores finales. Opciones: jugar de nuevo, volver a menú.",
  "ALTA",
  "PENDIENTE",
  dependencias: "Detección de fin de juego",
)

#tarea(
  "Menú de pausa",
  "Congelar juego, mostrar opciones: continuar, reiniciar, salir.",
  "MEDIA",
  "PENDIENTE",
  dependencias: "Menú principal",
)

== 3.9 Features Adicionales

#tarea(
  "Generación procedural de obstáculos",
  "Crear obstáculos aleatorios al inicio de la partida.",
  "MEDIA",
  "PENDIENTE",
  dependencias: "game_grid_t, PRNG",
)

#tarea(
  "Sistema de velocidad variable",
  "Permitir cambiar velocidad del juego (lento, normal, rápido, turbo).",
  "BAJA",
  "PENDIENTE",
  dependencias: "Game loop",
)

#tarea(
  "Boost/Turbo temporal",
  "Permitir acelerar temporalmente con tecla especial (barra de turbo).",
  "BAJA",
  "PENDIENTE",
  dependencias: "Sistema de velocidad",
)

#tarea(
  "Efectos de sonido",
  "Beeps en colisiones, movimientos, victoria.",
  "BAJA",
  "PENDIENTE",
  dependencias: "Driver de sonido",
)

#tarea(
  "Sistema de rounds (mejor de 3/5)",
  "Jugar múltiples rondas, primer jugador en ganar X rondas gana la partida.",
  "MEDIA",
  "PENDIENTE",
  dependencias: "Detección de fin de juego",
)

#tarea(
  "IA básica para modo 1 jugador",
  "Algoritmo simple para que CPU controle segundo jugador (seguir paredes, evitar colisiones).",
  "BAJA",
  "PENDIENTE",
  dependencias: "Lógica del juego completa",
)

#pagebreak()

= Fase 4: Extensión a 3D (OPCIONAL)

#nota[
Esta fase es opcional y representa una expansión significativa del proyecto. Solo debe abordarse si las fases anteriores están completamente funcionales y estables.
]

== 4.1 Motor de Matemáticas 3D

#tarea(
  "Estructura vec3_t y operaciones",
  "Vectores 3D con suma, resta, producto escalar, producto cruz, normalización.",
  "CRITICA",
  "PENDIENTE",
  dependencias: "Ninguna",
)

#tarea(
  "Estructura mat4_t y operaciones",
  "Matrices 4x4 con multiplicación, identidad, transformaciones.",
  "CRITICA",
  "PENDIENTE",
  dependencias: "vec3_t",
)

#tarea(
  "Funciones de transformación",
  "mat4_translate, mat4_rotate_x/y/z, mat4_scale.",
  "ALTA",
  "PENDIENTE",
  dependencias: "mat4_t",
)

#tarea(
  "Función mat4_perspective",
  "Matriz de proyección perspectiva con FOV, aspect ratio, near/far planes.",
  "CRITICA",
  "PENDIENTE",
  dependencias: "mat4_t",
)

#tarea(
  "Función mat4_look_at",
  "Matriz de vista para cámara.",
  "ALTA",
  "PENDIENTE",
  dependencias: "mat4_t, vec3_t",
)

#tarea(
  "Función project_3d_to_2d",
  "Proyectar punto 3D a coordenadas de pantalla 2D.",
  "CRITICA",
  "PENDIENTE",
  dependencias: "mat4_perspective, mat4_look_at",
)

== 4.2 Pipeline de Renderizado 3D

#tarea(
  "Estructura de cámara (camera_t)",
  "Posición, look_at, up, FOV, aspect, near, far.",
  "ALTA",
  "PENDIENTE",
  dependencias: "vec3_t",
)

#tarea(
  "Estructura de triángulo/polígono",
  "3 vértices, normal, color/textura.",
  "ALTA",
  "PENDIENTE",
  dependencias: "vec3_t",
)

#tarea(
  "Z-buffer: zbuffer_init, zbuffer_clear, zbuffer_test",
  "Depth buffer para ocultar superficies no visibles.",
  "CRITICA",
  "PENDIENTE",
  dependencias: "Allocator",
)

#tarea(
  "Backface culling",
  "Eliminar polígonos que miran hacia atrás (no visibles).",
  "ALTA",
  "PENDIENTE",
  dependencias: "vec3_t (producto cruz)",
)

#tarea(
  "Frustum culling básico",
  "Eliminar objetos fuera del campo de visión de la cámara.",
  "MEDIA",
  "PENDIENTE",
  dependencias: "camera_t",
)

== 4.3 Rasterización

#tarea(
  "Renderizado wireframe",
  "Dibujar solo las aristas de los polígonos.",
  "ALTA",
  "PENDIENTE",
  dependencias: "project_3d_to_2d, video_draw_line",
)

#tarea(
  "Rasterización de triángulos (scanline)",
  "Rellenar triángulos con color sólido.",
  "CRITICA",
  "PENDIENTE",
  dependencias: "Wireframe, z-buffer",
)

#tarea(
  "Flat shading",
  "Iluminación básica por polígono (producto punto con dirección de luz).",
  "MEDIA",
  "PENDIENTE",
  dependencias: "Rasterización de triángulos",
)

#tarea(
  "Gouraud shading (opcional)",
  "Interpolación de iluminación por vértice.",
  "BAJA",
  "PENDIENTE",
  dependencias: "Flat shading",
)

== 4.4 TRON 3D - Estructuras

#tarea(
  "Estructura player3d_t",
  "Posición (vec3), dirección (vec3), velocidad, física.",
  "ALTA",
  "PENDIENTE",
  dependencias: "vec3_t",
)

#tarea(
  "Estructura wall_segment_t",
  "Segmento de pared 3D: start (vec3), end (vec3), altura, color.",
  "ALTA",
  "PENDIENTE",
  dependencias: "vec3_t",
)

#tarea(
  "Estructura game3d_state_t",
  "Estado del juego 3D: jugadores, paredes, cámara, arena.",
  "ALTA",
  "PENDIENTE",
  dependencias: "player3d_t, wall_segment_t, camera_t",
)

== 4.5 TRON 3D - Lógica

#tarea(
  "Sistema de física 3D básico",
  "Velocidad, aceleración, fricción, inercia en giros.",
  "ALTA",
  "PENDIENTE",
  dependencias: "vec3_t",
)

#tarea(
  "Generación de wall segments",
  "Crear paredes verticales a partir del trail del jugador.",
  "CRITICA",
  "PENDIENTE",
  dependencias: "wall_segment_t",
)

#tarea(
  "Detección de colisiones 3D",
  "Verificar intersección con paredes 3D (AABB o segmentos).",
  "CRITICA",
  "PENDIENTE",
  dependencias: "wall_segment_t, vec3_t",
)

#tarea(
  "Sistema de cámara (selección de modos)",
  "Permitir cambiar entre primera persona, tercera persona, aérea, espectador.",
  "MEDIA",
  "PENDIENTE",
  dependencias: "camera_t",
)

#tarea(
  "Update de cámara según jugador",
  "Seguir al jugador con la cámara seleccionada.",
  "ALTA",
  "PENDIENTE",
  dependencias: "Sistema de cámara",
)

== 4.6 TRON 3D - Renderizado

#tarea(
  "Renderizar arena 3D",
  "Dibujar límites de la arena (plano, paredes perimetrales).",
  "ALTA",
  "PENDIENTE",
  dependencias: "Wireframe o rasterización",
)

#tarea(
  "Renderizar wall segments como geometría",
  "Cada segment = 2 triángulos (quad vertical).",
  "CRITICA",
  "PENDIENTE",
  dependencias: "Rasterización de triángulos",
)

#tarea(
  "Renderizar jugadores (modelos 3D simples)",
  "Cubos o esferas de baja complejidad.",
  "MEDIA",
  "PENDIENTE",
  dependencias: "Rasterización de triángulos",
)

#tarea(
  "Sistema de iluminación",
  "Luz direccional o point light para sombreado.",
  "BAJA",
  "PENDIENTE",
  dependencias: "Flat shading",
)

#tarea(
  "Efectos visuales: glow, trails con alpha",
  "Efectos estéticos para el estilo TRON.",
  "BAJA",
  "PENDIENTE",
  dependencias: "Renderizado completo",
)

== 4.7 Optimizaciones

#tarea(
  "Perfilado de performance",
  "Medir FPS, identificar cuellos de botella (rasterización, transform, etc.).",
  "ALTA",
  "PENDIENTE",
  dependencias: "TRON 3D funcional",
)

#tarea(
  "Optimización en Assembly (rutinas críticas)",
  "Rasterización, multiplicación de matrices, transformaciones con SSE/AVX.",
  "MEDIA",
  "PENDIENTE",
  dependencias: "Perfilado",
)

#tarea(
  "Reducción de resolución para performance",
  "Escalar a 640x480 o 800x600 si es necesario.",
  "BAJA",
  "PENDIENTE",
  dependencias: "Perfilado",
)

#tarea(
  "Sistema de LOD (Level of Detail)",
  "Reducir polígonos de objetos lejanos.",
  "BAJA",
  "PENDIENTE",
  dependencias: "Renderizado 3D completo",
)

#pagebreak()

= Matriz de Dependencias

Esta sección muestra las dependencias críticas entre fases:

#table(
  columns: (auto, 1fr, 1fr),
  align: (left, left, left),

  [*Fase*], [*Depende de*], [*Bloquea a*],

  [Fase 1.1 - Init Kernel], [Bootloader], [Toda Fase 1],
  [Fase 1.2 - Interrupts], [Fase 1.1], [Fase 1.4, 1.5, 1.6],
  [Fase 1.3 - Memory], [Fase 1.1], [Fase 1.4, 2.5],
  [Fase 1.4 - Video], [Fase 1.1, 1.2], [Fase 2.3, 3.x],
  [Fase 1.5 - Keyboard], [Fase 1.2], [Fase 2.2, 3.4],
  [Fase 1.6 - Timer], [Fase 1.2], [Fase 2.4, 3.3],
  [Fase 2.1 - Syscalls], [Fase 1.x completa], [Toda Fase 2],
  [Fase 2.5 - Userspace Lib], [Fase 2.1-2.4], [Fase 2.6, 3.x],
  [Fase 2.6 - Shell], [Fase 2.5], [Comando tron],
  [Fase 3.1 - Motor 2D], [Fase 1.4, 2.3], [Fase 3.x],
  [Fase 3.3 - Game Loop], [Fase 3.1, 3.2], [Resto Fase 3],
  [Fase 4.1 - Math 3D], [Ninguna (standalone)], [Toda Fase 4],
)

#pagebreak()

= Cronograma Estimado

#importante[
Estas estimaciones asumen trabajo constante y pueden variar según el equipo. Incluyen tiempo para debugging y testing.
]

#table(
  columns: (auto, auto, 1fr),
  align: (left, center, left),

  [*Fase*], [*Duración*], [*Hitos Clave*],

  [*Fase 1*], [3-4 sem], [
    - Kernel arranca y muestra info en pantalla
    - Interrupciones funcionan
    - Drivers básicos operativos
  ],

  [*Fase 2*], [2-3 sem], [
    - Syscalls implementados
    - Shell funcional con comandos básicos
    - printf/scanf operativo
  ],

  [*Fase 3*], [3-4 sem], [
    - TRON 2D jugable
    - Modos 1P y 2P
    - Menús y configuración
  ],

  [*Fase 4*], [4-6 sem], [
    - Motor 3D wireframe
    - TRON 3D básico
    - Optimizaciones
  ],
)

*Total (sin Fase 4):* 8-11 semanas

*Total (con Fase 4):* 12-17 semanas

#pagebreak()

= Criterios de Completitud

Para considerar cada fase como completa, se deben cumplir los siguientes criterios:

== Fase 1 - Kernel Base

- [ ] El sistema arranca en QEMU sin errores
- [ ] Se puede escribir texto en pantalla (modo gráfico)
- [ ] Las excepciones se capturan y muestran información
- [ ] El teclado responde y se pueden leer teclas
- [ ] El timer genera interrupciones periódicas
- [ ] Se puede alocar y liberar memoria

== Fase 2 - User Space

- [ ] Las syscalls funcionan desde user space
- [ ] printf puede imprimir strings, enteros, hex
- [ ] La shell muestra el prompt y acepta comandos
- [ ] Al menos 5 comandos funcionan (help, time, clear, meminfo, cpuinfo)
- [ ] Se puede salir de la shell sin colgar el sistema

== Fase 3 - TRON 2D

- [ ] El juego se inicia desde la shell con el comando "tron"
- [ ] Menú principal funcional con navegación
- [ ] Modo 2 jugadores es completamente jugable
- [ ] Detección de colisiones funciona correctamente
- [ ] Se muestra correctamente el ganador al final
- [ ] El juego corre a mínimo 30 FPS estables
- [ ] Se puede volver a la shell al salir del juego

== Fase 4 - TRON 3D (Opcional)

- [ ] Se pueden renderizar modelos 3D simples
- [ ] La cámara se puede mover y rotar
- [ ] El Z-buffer oculta correctamente superficies
- [ ] TRON 3D es jugable a mínimo 20 FPS
- [ ] Las paredes 3D se generan correctamente

#pagebreak()

= Notas Finales

== Priorización Recomendada

Para maximizar las posibilidades de éxito, seguir este orden:

1. *Primero:* Tareas marcadas como CRITICA
2. *Segundo:* Tareas marcadas como ALTA
3. *Tercero:* Tareas marcadas como MEDIA
4. *Último:* Tareas marcadas como BAJA

== Testing Continuo

Después de completar cada subsección, realizar pruebas:

- Test en QEMU (rápido)
- Test en VirtualBox (más realista)
- Test en hardware real (si es posible)

== Debugging

Implementar desde el principio:

- Sistema de logging (printf a puerto serial o pantalla)
- Volcado de registros en excepciones
- Breakpoints con QEMU + GDB
- Asserts para validar invariantes

== Documentación

Mantener actualizado:

- Comentarios en código (especialmente Assembly)
- README con instrucciones de compilación
- Este roadmap con el estado actual de cada tarea

== Recursos Útiles

- OSDev Wiki: https://wiki.osdev.org
- Intel Manual: https://www.intel.com/sdm
- Repositorio de Pure64: https://github.com/ReturnInfinity/Pure64
- QEMU Documentation: https://www.qemu.org/docs/

#importante[
Este roadmap es un documento vivo y debe actualizarse conforme el proyecto evoluciona. Marcar tareas como completadas, agregar nuevas tareas descubiertas durante la implementación, y ajustar estimaciones según sea necesario.
]

#align(center)[
  #v(2em)
  #text(size: 14pt, weight: "bold")[¡Buena suerte con el desarrollo de ARES!]
  #v(1em)
]
