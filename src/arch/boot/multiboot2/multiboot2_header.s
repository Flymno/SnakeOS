.section .multiboot2
.align 8

multiboot_header_start:
	.long 0xe85250d6 /* magic */
	.long 0
	.long multiboot_header_end - multiboot_header_start /* header length */
	.long -(0xe85250d6 + 0 + (multiboot_header_end - multiboot_header_start)) /* checksum */
	
	/* framebuffer */ 
	.word 5
	.word 0
	.long 24
	.long 1920
	.long 1080
	.long 32
	.long 0
	
	/* ending tag */
	.word 0
	.word 0
	.long 8
multiboot_header_end:
