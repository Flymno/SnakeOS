/* ---------------- Includes ---------------- */
#include <stdint.h>

/* ---------------- Extern Symbols ---------------- */
extern void reload_segment_registers(void);

/* ---------------- Internal Types ---------------- */
typedef struct {
	uint32_t base;
	uint32_t limit;
	uint8_t access;
	uint8_t flags;
} GDTEntry_t;

typedef struct {
	uint16_t limit;
	uint32_t base;
}__attribute__((packed)) GDTPointer_t;

/* ---------------- Internal State ---------------- */
GDTEntry_t GDTTable [3] = {
	{0x0, 0x0, 0x0, 0x0},
	{0x0, 0xfffff, 0x9a, 0xc},
	{0x0, 0xfffff, 0x92, 0xc}
};
uint8_t gdt_data[3 * 8] __attribute__((aligned(8)));

/* ---------------- Internal Helper Prototypes ---------------- */
void gdt_encode_entry(uint8_t *target, GDTEntry_t source);

/* ---------------- Internal Helper Implementation ---------------- 
* void gdt_encode_entry(uint8_t *target, GDTEntry_t source)
*	-Encodes a GDTEntry into the correct format at target
*/
void gdt_encode_entry(uint8_t *target, GDTEntry_t source) {
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

/* ---------------- Public API Implementation ---------------- 
* void gdt_init(void)
*	-Initialises the GDT. Calls gdtReload.s
*/
void gdt_init(void) { 
	for (int i = 0; i < 3; i++) {
		gdt_encode_entry(&gdt_data[i * 8], GDTTable[i]);
	}

	GDTPointer_t gdtp;

	gdtp.limit = (3 * 8) - 1;
	gdtp.base = (uint32_t)gdt_data;

	asm volatile ("lgdt %0" : : "m"(gdtp));

	reload_segment_registers();
}