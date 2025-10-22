section .data
global syscalls_table

syscalls_table:
    dq sys_write    ; 0x00
    dq sys_exit     ; 0x01
