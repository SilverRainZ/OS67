/* 8259A PIC
 * ref: http://wiki.osdev.org/8259_PIC
 */

#ifndef __PIC_H
#define __PIC_H

/* Chip IO port */
// Master PIC
#define PIC1_CMD    0x20
#define PIC1_DATA   0x21
// Slave PIC
#define PIC2_CMD    0xa0
#define PIC2_DATA   0xa1

// end of intrrupte
#define PIC_EOI     0x20

#define ICW1_ICW4       0x01    /* ICW4 (not) needed */
#define ICW1_SINGLE     0x02    /* Single (cascade) mode */
#define ICW1_INTERVAL4  0x04    /* Call address interval 4 (8) */
#define ICW1_LEVEL      0x08    /* Level triggered (edge) mode */
#define ICW1_INIT       0x10    /* Initialization - required! */
 
#define ICW4_8086       0x01    /* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO       0x02    /* Auto (normal) EOI */
#define ICW4_BUF_SLAVE  0x08    /* Buffered mode/slave */
#define ICW4_BUF_MASTER 0x0C    /* Buffered mode/master */
#define ICW4_SFNM       0x10    /* Special fully nested (not) */

#endif




