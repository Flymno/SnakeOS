.section .bss
.align 16

stack_bottom:
.skip 16384
stack_top:


.section .text
.global _start
.type _start, @function
_start:
	mov $stack_top, %esp

	/* kernel entry point */
    push %ebx
    push %eax
	call kernel_init
	
	/* halt */
	cli
1: 	hlt
	jmp 1b
	
.size _start, . - _start