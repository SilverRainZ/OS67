#ifndef __ASM_C
#define __ASM_C
#include <type.h>
uint8_t inportb(uint16_t _port);
void outportb(uint16_t _port, uint8_t _data);
void cli();
void sti();
void hlt();
#endif
