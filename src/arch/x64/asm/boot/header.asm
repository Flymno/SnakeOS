; Multiboot2 Header
; This section defines the Multiboot2 header required by the bootloader.
; It is binary data only (no executable code).


MAGIC       equ 0xe85250d6
ARCHITECTURE equ 0                                                                             ; 32-bit (protected) mode of i386

section .multiboot2
align 8
multiboot_header_start:
	dd MAGIC                                                                                   ; Magic Number
	dd ARCHITECTURE                                                                            ; Architecture (0 = i386)
	dd multiboot_header_end - multiboot_header_start                                           ; Length of header (bytes)
	dd 0x100000000 - (MAGIC + ARCHITECTURE + (multiboot_header_end - multiboot_header_start))  ; Checksum (magic + length + arch MUST sum to 0)
	
	;Framebuffer Tag
	dw 5                                                                                       ; Tag 5: Framebuffer
	dw 0
	dd 24                                                                                      ; Size of tag (bytes)
	dd 1920                                                                                    ; 1920x1080
	dd 1080                                                                                    ; 1920x1080
	dd 32                                                                                      ; 32bpp
	dd 0                                                                                       ; Type 0: RGB
	
	;Ending Tag
	dw 0
	dw 0
	dd 8
multiboot_header_end:
