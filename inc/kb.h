#ifndef __KB_H
#define __KB_H

#define KB_DATA     0x60
#define KB_STAT     0x64
#define KB_CMD      0x64

/* output buffer full */
#define KB_STAT_OBF     0x01

#define SHIFT       (1<<0)
#define CTRL        (1<<1)
#define ALT         (1<<2)

#define E0ESC       (1<<6)

#define KB_IS_RELEASE(sc)   (sc & 0x80) 
#define KB_IS_ESCAPE(sc)    (sc == 0xe0)

void kb_init();
#endif
