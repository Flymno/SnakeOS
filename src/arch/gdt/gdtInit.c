#include <stdint.h>

extern void reload_segment_registers(void);

struct GDT_entry {
	uint32_t base;
	uint32_t limit;
	uint8_t access;
	uint8_t flags;
};

typedef struct GDTPointer {
	uint16_t limit;
	uint32_t base;
}__attribute__((packed)) GDTPointer;

struct GDT_entry GDTTable [3] = {
	{0x0, 0x0, 0x0, 0x0},
	{0x0, 0xfffff, 0x9a, 0xc},
	{0x0, 0xfffff, 0x92, 0xc}
};

void encode_gdt_entry(uint8_t *target, struct GDT_entry source)
{
    
    // Encode the limit
    target[0] = source.limit & 0xFF;
    target[1] = (source.limit >> 8) & 0xFF;
    target[6] = (source.limit >> 16) & 0x0F;
    
    // Encode the base
    target[2] = source.base & 0xFF;
    target[3] = (source.base >> 8) & 0xFF;
    target[4] = (source.base >> 16) & 0xFF;
    target[7] = (source.base >> 24) & 0xFF;
    
    // Encode the access byte
    target[5] = source.access;
    
    // Encode the flags
    target[6] |= (source.flags << 4);
}

uint8_t gdt_data[3 * 8] __attribute__((aligned(8)));

void init_gdt()
{ 

	for (int i = 0; i < 3; i++)
	{encode_gdt_entry(&gdt_data[i * 8], GDTTable[i]);}

	GDTPointer gdtp;

	gdtp.limit = (3 * 8) - 1;
	gdtp.base = (uint32_t)gdt_data;

	asm volatile ("lgdt %0" : : "m"(gdtp));

	reload_segment_registers();
}