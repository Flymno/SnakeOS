section .bootstrap.bss noexec write alloc
align 16

boot_stack_bottom:
resb 16384
boot_stack_top:

section .bootstrap.text exec nowrite alloc
extern init_early_paging
global _start
_start:
	mov esp, boot_stack_top

	push eax
	push ebx

	call init_early_paging

	mov cr3, eax
	mov eax, cr4
	or eax, 0x00000010
	mov cr4, eax
	mov eax, cr0
	or eax, 0x80000000
	mov cr0, eax

	pop ebx
	pop eax

	lea ecx, _kStart
	jmp ecx


; Kernel entry point and stack setup.
;	- Allocate 16KB stack in .bss
;	- Setup %esp
;	- Push Multiboot2 info onto stack
;	- Call kernel_init()
;	- Halt CPU if kernel returns

section .bss
align 16

stack_bottom:
resb 16384
stack_top:

section .text
global _kStart
extern arch_kinit
_kStart:
	mov esp, stack_top                 ;Set stack pointer to top of stack

    push ebx                           ;Push Multiboot2 info address
    push eax                           ;Push Multiboot2 magic
	call arch_kinit                    ;Call kernel entry function	
	
	cli                                ;Disable interrupts
.hang: 
	hlt                                ;Halt CPU
	jmp .hang
