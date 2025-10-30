section .data
global syscalls_table
extern sys_exit
extern sys_write

extern sys_write
extern sys_exit

syscalls_table:
    dq sys_write
    dq sys_exit

