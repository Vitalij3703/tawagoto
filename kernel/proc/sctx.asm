section .text
global sctx
; sctx(old_slot new_slot)
sctx:
    push ebp
    push ebx
    push esi
    push edi
    ; first arg at [esp+20], second at [esp+24]
    mov eax, [esp+20]
    mov ecx, [esp+24]
    mov [eax], esp
    mov esp, ecx
    pop edi
    pop esi
    pop ebx
    pop ebp
    ret