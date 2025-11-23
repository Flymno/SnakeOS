.section .text
.global reload_segment_registers
.type reload_segment_registers, @function

reload_segment_registers:
	mov $0x10, %ax /* reload segment registers */
	mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss

	jmp $0x08, $flush /* far jump */

flush:
	ret
