#ifndef __ASM_H
#define __ASM_H
#include <type.h>
uint8_t inportb(uint16_t _port);
void outportb(uint16_t _port, uint8_t _data);
void cli();
void sti();
void hlt();
#endif
