;Reload all segment registers with kernel segments and refresh CS via far jump

section .text
global reload_segment_registers
reload_segment_registers:
    mov ax, 0x10             ;Reload segment registers
	mov ds, ax               ;Reload DS
    mov es, ax               ;Reload ES
    mov fs, ax               ;Reload FS
    mov gs, ax               ;Reload GS
    mov ss, ax               ;Reload SS

	jmp 0x08:flush           ;Far jump to reload CS with kernel code segment

flush:
	ret                      ;Return to caller
