/* Kernel entry point and stack setup.
	- Allocate 16KB stack in .bss
	- Setup %esp
	- Push Multiboot2 info onto stack
	- Call kernel_init()
	- Halt CPU if kernel returns
*/
.section .bss
.align 16

stack_bottom:
.skip 16384
stack_top:


.section .text
.global _start
.type _start, @function
_start:
	mov $stack_top, %esp               /* Set stack pointer to top of stack*/

	
    push %ebx                          /* Push Multiboot2 info address */
    push %eax                          /* Push Multiboot2 magic*/
	call kernel_init                   /* Call kernel entry function */
	
	
	cli                                /* Disable interrupts*/
1: 	hlt                                /* Halt CPU */
	jmp 1b
	
.size _start, . - _start