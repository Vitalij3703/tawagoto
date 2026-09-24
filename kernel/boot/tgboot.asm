_MAGIC equ 0x1BADB002
_FLAGS equ 0x00000003
_CHECK equ -(_MAGIC + _FLAGS) ; sum

section .multiboot
align 4
    dd _MAGIC
    dd _FLAGS
    dd _CHECK
    dd 0, 0, 0, 0, 0
    dd 0
    dd 1024
    dd 768
    dd 32

section .text
global _start
global __tgbpm
extern _tgkmain
_start:
    cli
    mov esp, stack_top
    push ebx
    push eax
    call _tgkmain
.hang:
    hlt
    jmp .hang
global enablepaging
enablepaging:
    push ebp
    mov ebp, esp
    mov eax, [ebp+8]
    mov cr3, eax
    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax
    pop ebp
    ret
section .bss
align 16
stack_bottom:
    resb 16384
stack_top:
    