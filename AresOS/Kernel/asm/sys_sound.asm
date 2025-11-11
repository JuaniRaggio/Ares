section .text
global sys_play_sound_wrapper
global sys_beep_wrapper
extern sys_play_sound
extern sys_beep

sys_play_sound_wrapper:
    push rbp
    mov rbp, rsp

    call sys_play_sound

    mov rsp, rbp
    pop rbp
    ret

sys_beep_wrapper:
    push rbp
    mov rbp, rsp

    call sys_beep

    mov rsp, rbp
    pop rbp
    ret
