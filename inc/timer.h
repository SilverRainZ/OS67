#ifndef __TIMER_H
#define __TIMER_H

#define PIT_CH0 0x40
#define PIT_CH1 0x41
#define PIT_CH2 0x42
#define PIT_MOD_CMD 0x43

void timer_init();
void timer_wait();
#endif
