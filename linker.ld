/* The bootloader will look at this image and start execution at the symbol
   designated as the entry point. */
ENTRY(_start)
 
/* Tell where the various sections of the object files will be put in the final
   kernel image. */

PHDRS
{
    boot PT_LOAD FLAGS(5);  /* RX */
	bootdata PT_LOAD FLAGS(6);  /* RW */
    text PT_LOAD FLAGS(5);  /* RX */
    ro PT_LOAD FLAGS(4);    /* R  */
    data PT_LOAD FLAGS(6);  /* RW */
}

SECTIONS
{
	. = 0x00100000;

	.bootstrap.text BLOCK(4K) : ALIGN(4K)
	{
		*(.multiboot2)
		*(.bootstrap.text)
	} :boot
 
	.bootstrap.bss BLOCK(4K) : ALIGN(4K)
	{
		*(.bootstrap.bss)
	} :bootdata
	
	. += 0xC0000000;
	_physStart = . - 0xC0000000;
	_vStart = .;

	_textStart = .;
	_physTextStart = . - 0xC0000000;
	.text ALIGN(4K) : AT (ADDR (.text) - 0xC0000000)
	{
		*(.text)
	} :text
	_textEnd = .;

	.rodata ALIGN(4K) : AT (ADDR (.rodata) - 0xC0000000)
	{
		_roStart = .;
		_physRoStart = . - 0xC0000000;
		*(.rodata .rodata.*)
		_roEnd = .;
	} :ro
 
	
	.data ALIGN(4K) : AT (ADDR (.data) - 0xC0000000)
	{
		_dataStart = .;
		_physDataStart = . - 0xC0000000;
		*(.data)
	} :data

	.bss ALIGN(4K) : AT (ADDR (.bss) - 0xC0000000)
	{
		*(COMMON)
		*(.bss)
	} :data
	_dataEnd = .;

	_physEnd = . - 0xC0000000;
	_vEnd = .;
 
	/* The compiler may produce other sections, by default it will put them in
	   a segment with the same name. Simply add stuff here as needed. */
}