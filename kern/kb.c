/* kb.c */
#include <type.h>
#include <vga.h>
#include <asm.h>
#include <isr.h>
#include <kb.h>

static unsigned char kb_map[128] =
{
    0,  
    27, /* esc */
    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
    '-', '=', '\b', '\t',		'q', 'w', 'e', 'r',	
    't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0,  /* Left Control */
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';','\'', '`',
    0,	/* Left shift */
    '\\', 'z', 'x', 'c', 'v', 'b', 'n',
    'm', ',', '.', '/',   
    0,	/* Right shift */
    '*',
    0,  /* Alt */
    ' ',/* Space bar */
    0,  /* Caps lock */
    0,	/* 59 - F1 key ... > */
    0, 0, 0, 0, 0, 0, 0, 0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
    '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
    '+',
    0,	/* End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};

static unsigned char kb_map_shift[128] =
{
    0,  
    27, /* esc */
    '!', '@', '#', '$', '%', '^', '&', '*', '(', ')',
    '_', '+', '\b', '\t', 'Q', 'W', 'E', 'R',	
    'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0,  /* Left Control */
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':','\"', '~',
    0,	/* Left shift */
    '|', 'Z', 'X', 'C', 'V', 'B', 'N',
    'M', '<', '>', '?',   
    0,	/* Right shift */
    '*',
    0,  /* Alt */
    ' ',/* Space bar */
    0,  /* Caps lock */
    0,	/* 59 - F1 key ... > */
    0, 0, 0, 0, 0, 0, 0, 0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
    '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
    '+',
    0,	/* End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0, 0, 0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};

void kb_handler(struct regs_s *r){
    unsigned char scancode;
    kb_map_shift[0] = 0;
    /* Read from the keyboard's data buffer */
    scancode = inportb(0x60);
    /* If the top bit of the byte we read from the keyboard is
     *  set, that means that a key has just been released */
    if (scancode & 0x80){
        /* You can use this one to see if the user released the
         *  shift, alt, or control keys... */
    }
    else
    {
        /* Here, a key was just pressed. Please note that if you
         *  hold a key down, you will get repeated key press
         *  interrupts. */

        /* Just to show you how this works, we simply translate
         *  the keyboard scancode into an ASCII value, and then
         *  display it to the screen. You can get creative and
         *  use some flags to see if a shift is pressed and use a
         *  different layout, or you can add another 128 entries
         *  to the above layout to correspond to 'shift' being
         *  held. If shift is held using the larger lookup table,
         *  you would add 128 to the scancode when you look for it */
        putchar(kb_map[scancode]);
    }
}

void kb_init(){
    irq_install_handler(1, kb_handler);
}
