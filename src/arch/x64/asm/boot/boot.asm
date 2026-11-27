[BITS 32]

global long_mode

section .bootstrap.bss nobits noexec write alloc
align 16

mb2_magic: resd 1
mb2_ptr: resd 1

boot_stack_bottom:
resb 16384
boot_stack_top:

section .bootstrap.text exec nowrite alloc
extern init_early_paging
extern init_early_gdt
global _start
_start:
	mov esp, boot_stack_top

	mov [mb2_magic], eax     ;Save Multiboot2 info for later
	mov [mb2_ptr], ebx

	call init_early_paging

	mov cr3, eax	         ;Load PML4

	mov eax, cr4
	or eax, (1 << 5)	     ;Set PAE
	mov cr4, eax

	mov ecx, 0xC0000080
	rdmsr
	or eax, (1 << 8)	     ;Set Long Mode
	wrmsr

	mov eax, cr0
	or eax, (1 << 31)	     ;Set Page Enable
  	mov cr0, eax

	call init_early_gdt

	[BITS 64]
long_mode:                   ;Long Mode Entry Point
	mov rax, cr0             ;Enable SSE Instructions
	and ax, 0xFFFB
	or ax, 0x2
	mov cr0, rax
	
	mov rax, cr4
	or rax, (3 << 9)
	mov cr4, rax

	lea rcx, _kStart         ;Jump to kernel entry point
	jmp rcx


; Kernel entry point and stack setup.
;	- Allocate 16KB stack in .bss
;	- Setup rsp
;	- Call kernel_init()
;	- Halt CPU if kernel returns

section .bss
align 16

stack_bottom:
resb 16384
stack_top:

section .text
extern arch_kinit
_kStart:
	mov rsp, stack_top       ;Set stack pointer to top of stack
	mov rbp, rsp

	mov eax, [mb2_magic]     ;Reload Multiboot2 info to be passed to kernel entry
	mov rdi, rax

	mov eax, [mb2_ptr]
	mov rsi, rax

	call arch_kinit          ;Call kernel entry function	
	
	cli                      ;Disable interrupts
.hang: 
	hlt                      ;Halt CPU
	jmp .hang
