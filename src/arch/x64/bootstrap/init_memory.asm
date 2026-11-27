[BITS 32]

global init_early_paging
global init_early_gdt

extern long_mode

PAGE_PRESENT equ 1 << 0
PAGE_WRITABLE equ 1 << 1
PAGE_HUGE equ 1 << 7

section .bootstrap.bss nobits noexec write alloc
align 16

earlyPML4: resq 512
earlyPDPLow: resq 512
earlyPDPHigh: resq 512
earlyPDShared: resq 512

earlyGDT: resq 3

section .bootstrap.data noexec write alloc
align 8

earlyGdtStart:
    dq 0                         ; Null descriptor
    dq 0x00209A0000000000        ; Kernel Code (L-bit set, Present, Ring 0)
    dq 0x0000920000000000        ; Kernel Data (Present, Ring 0, Writable)
earlyGdtEnd:

earlyGdtPtr:
    dw earlyGdtEnd - earlyGdtStart - 1   ; Size (Limit)
    dd earlyGdtStart

section .bootstrap.text exec nowrite alloc

init_early_paging:
    cld

    mov edi, earlyPML4            ;Clear earlyPML4
    mov eax, 0
    mov ecx, 1024
    rep stosd

    mov edi, earlyPDPLow          ;Clear earlyPDPLow
    mov eax, 0
    mov ecx, 1024
    rep stosd

    mov edi, earlyPDPHigh         ;Clear earlyPDPHigh
    mov eax, 0
    mov ecx, 1024
    rep stosd

    mov edi, earlyPDShared        ;Clear earlyPDShared
    mov eax, 0
    mov ecx, 1024
    rep stosd

    mov esi, 0
.populatePD:
    cmp esi, 4
    jge .populatedPD

    mov eax, esi
    shl eax, 21
    or eax, PAGE_HUGE
    or eax, PAGE_PRESENT
    or eax, PAGE_WRITABLE

    mov ebx, 511
    sub ebx, esi

    mov [earlyPDShared + esi * 8], eax
    mov [earlyPDShared + ebx * 8], eax

    inc esi
    jmp .populatePD

.populatedPD:
    mov eax, earlyPDPLow
    or eax, PAGE_PRESENT
    or eax, PAGE_WRITABLE
    mov [earlyPML4], eax

    mov eax, earlyPDPHigh
    or eax, PAGE_PRESENT
    or eax, PAGE_WRITABLE
    mov [earlyPML4 + 511 * 8], eax

    mov eax, earlyPDShared
    or eax, PAGE_PRESENT
    or eax, PAGE_WRITABLE
    mov [earlyPDPLow], eax
    mov [earlyPDPHigh + 510 * 8], eax

    mov eax, earlyPML4
    ret

init_early_gdt:
    lgdt [earlyGdtPtr]

	mov ax, 0x10             ;Reload segment registers
	mov ds, ax               ;Reload DS
    mov es, ax               ;Reload ES
    mov fs, ax               ;Reload FS
    mov gs, ax               ;Reload GS
    mov ss, ax               ;Reload SS

    jmp 0x08:long_mode       ;Jump into long mode