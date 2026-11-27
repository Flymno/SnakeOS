/* ---------------- Includes ---------------- */
#include <common/types.h>
#include <hal/hal.h>
#include <memory/heap.h>
#include <arch/hwio/ports.h>
#include <lib/string.h>

/* ---------------- Internal Helper Prototypes ---------------- */
static void x64_serial_init(void);
static void x64_serial_putchar(char c);
static size_t serial_can_write(void);

/* ---------------- Internal Helper Implementation ---------------- 
* static void serial_init()
*   -Initialises the serial port hardware (COM1)
* static void serial_putchar(char c)
*   -Writes a byte to the serial port
* static uint8_t serial_can_write(void)
*   -Returns nonzero if a new byte can be written to the serial port
*/
static void x64_serial_init(void) {
	outb(0x3F8 + 1, 0x00);    // Disable all interrupts
    outb(0x3F8 + 3, 0x80);    // Enable DLAB (set baud rate divisor)
    outb(0x3F8 + 0, 0x03);    // Set divisor to 3 (low byte) 38400 baud
    outb(0x3F8 + 1, 0x00);    //                 (high byte)
    outb(0x3F8 + 3, 0x03);    // 8 bits, no parity, one stop bit
    outb(0x3F8 + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
    outb(0x3F8 + 4, 0x0B);    // IRQs enabled, RTS/DSR set
}

static void x64_serial_putchar(char c) {
	while (serial_can_write() == 0);
	outb(0x3f8, c);
}

static size_t serial_can_write(void) {
	return inb(0x3f8 + 5) & 0x20;
 }

static status_t control(uint32_t CMD, hal_arg_t in, hal_arg_t out) {
    switch (CMD)
    {
        case CMD_PUT: {
            char c = (char)in;
            x64_serial_putchar(c);
            return SUCCESS;
        }
                   
        case CMD_WRITE: {
            const char* s = (const char *)in;
            if (!s) {
                return FAILURE;
            }
            for (size_t i = 0; s[i] != '\0'; i++) {
                x64_serial_putchar(s[i]);
            }
            return SUCCESS;
        }

        case CMD_INIT: {
            x64_serial_init();
            return SUCCESS;
        }
        
        case CMD_IDENT: {
            if (!out) {
                return FAILURE;
            }

            CmdIdentResponse_t *ident = (CmdIdentResponse_t *)out;
            ident->dev_type = DEV_CONSOLE;
            strcpy(ident->name, "x64 Serial Console");
            return SUCCESS;
        }

        default: {
            return FAILURE;
        }
    }
}

HALDevice_t* serial_hal_registration() {
    HALDevice_t *devInfo = kmalloc(sizeof(HALDevice_t));
    devInfo->control = control;

    return devInfo;
}