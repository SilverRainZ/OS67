// #define __LOG_ON 1

/* kb.c 
 * This file is modified form Bram's Kernel Development Tutorial
 * enable keyboard and retive message by IRQ
 * October is bad :(
 * http://bbs.chinaunix.net/thread-1999379-1-1.html
 * borrow code form: https://github.com/Fleurer/fleurix/blob/master/src/chr/keybd.c
 */
// std
#include <type.h>
#include <asm.h>
// x86
#include <isr.h>
// drv
#include <kb.h>
#include <vga.h>
// libs
#include <printk.h>
// dev
#include <tty.h>

static uint32_t kb_mode = 0;

/* maybe scancode set 1 ?*/
static char kb_map[128] = {
    0,  
    0x1b,   /* esc */
    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
    '-', '=', '\b', '\t', 'q', 'w', 'e', 'r',    
    't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0,      /* left ctrl */
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';','\'', '`',
    0,      /* left shift */
    '\\', 'z', 'x', 'c', 'v', 'b', 'n',
    'm', ',', '.', '/',   
    0,    /* right shift */
    '*',
    0,  /* alt */
    ' ',/* space*/
    0,  /* capslock */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   /* f1 ... f10 */
    0,    /* num lock*/
    0,    /* scroll Lock */
    0,    /* home key */
    0,    /* up arrow */
    0,    /* page up */
    '-',
    0,    /* left arrow */
    0,
    0,    /* right arrow */
    '+',
    0,    /* end key*/
    0,    /* down arrow */
    0,    /* page down */
    0,    /* insert key */
    0,    /* delete key */
    0,   0,   0,
    0,    /* f11 key */
    0,    /* f12 key */
    0,    /* all other keys are undefined */
};

static char kb_shift_map[128] =
{
    0,  
    0x1b, /* esc */
    '!', '@', '#', '$', '%', '^', '&', '*', '(', ')',
    '_', '+', '\b', '\t', 'Q', 'W', 'E', 'R',    
    'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0,  /* left control */
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':','\"', '~',
    0,    /* left shift */
    '|', 'Z', 'X', 'C', 'V', 'B', 'N',
    'M', '<', '>', '?',   
    0,    /* right shift */
    '*',
    0,  /* alt */
    ' ',/* space bar */
    0,  /* caps lock */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   /* f1 ... f10 */
    0,    /* num lock*/
    0,    /* scroll lock */
    0,    /* home key */
    0,    /* up arrow */
    0,    /* page up */
    '-',
    0,    /* left arrow */
    0,
    0,    /* right arrow */
    '+',
    0,    /* end key*/
    0,    /* down arrow */
    0,    /* page down */
    0,    /* insert key */
    0,    /* delete key */
    0, 0, 0,
    0,    /* f11 key */
    0,    /* f12 key */
    0,    /* all other keys are undefined */
};

static char shift(char sc){
    char ch = sc & 0x7f;    // clear highest bit

    /* the previous scancode is 0xe0*/
    if (kb_mode & E0ESC) { 
        switch (ch) {
            case 0x1D: return CTRL;
            case 0x38: return ALT;
        }
    }
    else {
        switch(ch) {
            case 0x2A:
            case 0x36: return SHIFT;
            case 0x1D: return CTRL;
            case 0x38: return ALT;
        }
    }
    return 0;
}

void kb_handler(struct int_frame *r){
    uint8_t sc, m;
    char ch;

    if ((inb(KB_STAT) & KB_STAT_OBF) == 0){
        /* no data to read */
        return;
    }

    /* read */
    sc = inb(KB_DATA);
    printl("kb_handler: sc: 0x%x\n", sc);

    /* is a escape char? */
    if (KB_IS_ESCAPE(sc)){
        printl("kb_handler: ESCAPE\n");
        kb_mode |= E0ESC;
        return;
    } 
    
    /* check alt shift and ctrl */
    if ((m = shift(sc))){
        if (KB_IS_RELEASE(sc)){
            /* clear mode when release the key */
            kb_mode &= ~m;
        } else {
            kb_mode |= m;
        }
        return;
    }

    if (kb_mode & SHIFT){
        printl("kb_handler: SHIFT\n");

        ch = kb_shift_map[sc & 0x7f];
    } else {
        ch = kb_map[sc & 0x7f];
    }

    if (kb_mode & CTRL){
        printl("kb_handler: CTRL\n");
        switch(ch){
            // console control char
            case 'd': printl("kb_handler: EOF\n"); ch = TTY_EOF; break;
            case 'c': printl("kb_handler: INT\n"); ch = TTY_INT; break;
        }
    }

    if (kb_mode & ALT){
        printl("kb_handler: ALT\n");
        /* nothing to do ? */
    }

    /* on release */
    if (KB_IS_RELEASE(sc)){
        printl("kb_handler: release, clear E0ESC\n");
        kb_mode &= ~E0ESC;
    } 
    /* on press */
    else if (ch != 0) {
        printl("kb_handler: char: [%c]\n", ch);

        /* write to kernel input buffer */
        if (tty_buf_in(ch) < 0){
            printl("kb: buffer overflow.\n");
        }
    }
}

void kb_init(){
    irq_install(IRQ_KB, kb_handler);
}
