.section .bootstrap.bss, "aw"
.align 16

boot_stack_bottom:
.skip 16384
boot_stack_top:

.section .bootstrap.text, "ax"
.global _start
.type _start, @function
_start:
	mov $boot_stack_top, %esp

	push %eax
	push %ebx

	call init_early_paging

	mov %eax, %cr3
	mov %cr4, %eax
	or $0x00000010, %eax
	mov %eax, %cr4
	mov %cr0, %eax
	or $0x80000000, %eax
	mov %eax, %cr0
	jmp 1f
1:
	pop %ebx
	pop %eax

	lea _kStart, %ecx
	jmp *%ecx

.size _start, . - _start

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
.global _kStart
.type _kStart, @function
_kStart:
	mov $stack_top, %esp               /* Set stack pointer to top of stack*/

    push %ebx                          /* Push Multiboot2 info address */
    push %eax                          /* Push Multiboot2 magic*/
	call kernel_init                   /* Call kernel entry function */	
	
	cli                                /* Disable interrupts*/
1: 	hlt                                /* Halt CPU */
	jmp 1b
