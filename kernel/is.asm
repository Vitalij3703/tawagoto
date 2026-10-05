; interrupt stubs
section .text
extern _tgkinterrupt
global isr0
global isr1
global isr2
global isr3
global isr4
global isr5
global isr6
global isr7
global isr8
global isr9
global isr10
global isr11
global isr12
global isr13
global isr14
global isr15
global isr16
global isr17
global isr18
global isr19
global isr20
global isr21
global isr22
global isr23
global isr24
global isr25
global isr26
global isr27
global isr28
global isr29
global isr30
global isr31
global irq0
global irq1
global irq2
global irq3
global irq4
global irq5
global irq6
global irq7
global irq8
global irq9
global irq10
global irq11
global irq12
global irq13
global irq14
global irq15
hang: ;for debugging
    hlt
    jmp hang
commoni:
    pusha
    push esp
    call _tgkinterrupt
    ;jmp hang
    add esp, 4
    popa
    add esp, 8
    iretd
    ;hlt
isr0: ;1
    push 0
    push 0
    jmp commoni
isr1: ;2
    push 0
    push 1
    jmp commoni
isr2: ;3
    push 0
    push 2
    jmp commoni
isr3: ;4
    push  0
    push 3
    jmp commoni
isr4: ;5
    push 0
    push 4
    jmp commoni
isr5: ;6
    push 0
    push 5
    jmp commoni
isr6: ;7
    push 0
    push 6
    jmp commoni
isr7: ;8
    push 0
    push 7
    jmp commoni
isr8: ;9
    ; theres no mistake, the cpu already pushes the error code
    push 8
    jmp commoni
isr9: ;10
    push 0
    push 9
    jmp commoni
isr10: ;11
    push 10
    jmp commoni
isr11: ;12
    push 11
    jmp commoni
isr12: ;13
    push 12
    jmp commoni
isr13: ;14
    push 13
    jmp commoni
isr14: ;15
    push 14
    jmp commoni
isr15: ;16
    push 0
    push 15
    jmp commoni
isr16: ;17
    push 0
    push 16
    jmp commoni
isr17: ;18
    push 17
    jmp commoni
isr18: ;19
    push 0
    push 18
    jmp commoni
isr19: ;20
    push 0
    push 19
    jmp commoni
isr20: ;21
    push 0
    push 20
    jmp commoni
isr21: ;you see the trend now
    push 21
    jmp commoni
isr22:
    push 0
    push 22
    jmp commoni
isr23:
    push 0
    push 23
    jmp commoni
isr24:
    push 0
    push 24
    jmp commoni
isr25:
    push 0
    push 25
    jmp commoni
isr26:
    push 0
    push 26
    jmp commoni
isr27:
    push 0
    push 27
    jmp commoni
isr28:
    push 0
    push 28
    jmp commoni
isr29:
    push 29
    jmp commoni
isr30:
    push 30
    jmp commoni
isr31:
    push 0
    push 31
    jmp commoni
irq0:
    push 0
    push 32
    jmp commoni
irq1:
    push 0
    push 33
    jmp commoni
irq2:
    push 0
    push 34
    jmp commoni
irq3:
    push 0
    push 35
    jmp commoni
irq4:
    push 0
    push 36
    jmp commoni
irq5:
    push 0
    push 37
    jmp commoni
irq6:
    push 0
    push 38
    jmp commoni
irq7:
    push 0
    push 39
    jmp commoni
irq8:
    push 0
    push 40
    jmp commoni
irq9:
    push 0
    push 41
    jmp commoni
irq10:
    push 0
    push 42
    jmp commoni
irq11:
    push 0
    push 43
    jmp commoni
irq12:
    push 0
    push 44
    jmp commoni
irq13:
    push 0
    push 45
    jmp commoni
irq14:
    push 0
    push 46
    jmp commoni
irq15:
    push 0
    push 47
    jmp commoni