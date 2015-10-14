#ifndef __TIMER_H
#define __TIMER_H

#include <type.h>

#define PIT_CH0 0x40
#define PIT_CH1 0x41
#define PIT_CH2 0x42
#define PIT_MOD_CMD 0x43

extern uint32_t timer_ticks;

void timer_init();
#endif
