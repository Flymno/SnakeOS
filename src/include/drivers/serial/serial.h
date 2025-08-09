#ifndef SERIAL_H_
#define SERIAL_H_

void init_serial();

void serial_writestring(const char* data);

void serial_writehex(uint32_t val);

void serial_writedec(uint32_t val);

void serial_newline();

#endif