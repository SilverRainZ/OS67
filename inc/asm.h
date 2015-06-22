#ifndef __ASM_H
#define __ASM_H
#include <type.h>
uint8_t inb(uint16_t port);
void outb(uint16_t port, uint8_t data);

uint16_t inw(uint16_t port);
void outw(uint16_t port, uint16_t data);

void insl(uint32_t port, void *addr, uint32_t cnt);
void outsl(uint32_t port, const void *addr, uint32_t cnt);

void cli();
void sti();
void hlt();
#endif
