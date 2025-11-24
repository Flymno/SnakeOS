/* Reload all segment registers with kernel segments and refresh CS via far jump */

.section .text
.global reload_segment_registers
.type reload_segment_registers, @function

reload_segment_registers:
	mov $0x10, %ax           /* Reload segment registers */
	mov %ax, %ds             /* Reload DS*/
    mov %ax, %es             /* Reload ES*/
    mov %ax, %fs             /* Reload FS*/
    mov %ax, %gs             /* Reload GS*/
    mov %ax, %ss             /* Reload SS*/

	jmp $0x08, $flush        /* Far jump to reload CS with kernel code segment */

flush:
	ret                      /* Return to caller*/
