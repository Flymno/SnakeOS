/* 
 * Multiboot2 Header
 * This section defines the Multiboot2 header required by the bootloader.
 * It is binary data only (no executable code).
 */

.section .multiboot2
.align 8

multiboot_header_start:
	.long 0xe85250d6                                                           /* Magic Number */
	.long 0                                                                    /* Architecture (0 = i386) */
	.long multiboot_header_end - multiboot_header_start                        /* Length of header (bytes) */
	.long -(0xe85250d6 + 0 + (multiboot_header_end - multiboot_header_start))  /* Checksum (magic + length + arch MUST sum to 0) */
	
	/* Framebuffer Tag */
	.word 5                                                                    /* Tag 5: Framebuffer */
	.word 0
	.long 24                                                                   /* Size of tag (bytes) */
	.long 1920                                                                 /* 1920x1080 */
	.long 1080                                                                 /* 1920x1080 */
	.long 32                                                                   /* 32bpp*/
	.long 0                                                                    /* Type 0: RGB*/
	
	/* Ending Tag */
	.word 0
	.word 0
	.long 8
multiboot_header_end:
