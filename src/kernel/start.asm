[bits 32]

section .text

extern kernel_init

global _start
_start:
    call kernel_init
    jmp $ ; 阻塞