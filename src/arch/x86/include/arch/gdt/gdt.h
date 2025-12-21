#ifndef GDT_H_
#define GDT_H_

/* ---------------- Public API Prototypes ---------------- 
* void gdt_init(void)
*	-Initialises the GDT. Calls gdtReload.s
*/
void gdt_init();

#endif