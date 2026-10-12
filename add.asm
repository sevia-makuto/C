section .text
    global _start

_start:
    mov eax, 5
    add eax, 3

    mov eax, 60
    mov edi, 0
    syscall
