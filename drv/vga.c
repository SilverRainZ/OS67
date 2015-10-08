/* vga.c
 * control the display of char, 
 * you must invoke init_vga before use other func
 */
// std
#include <type.h>
#include <asm.h>
#include <dbg.h>
// libs
#include <string.h>
// drv
#include <vga.h>

/* it is strange that i can't use 
 * char* vga_mem = 0xb8000, 
 * it can only disp blank char */
struct vga_char*vga_mem;    // vga[25][80] at 0xb800
struct vga_char color;      // use vag_char structure to store color

struct point cur;

static void move_cur(){
    uint16_t tmp;

    tmp = cur.y * 80 + cur.x;

    /* cursor high port to vga index register */
    outb(VGA_CRT_IC, 0xe);
    outb(VGA_CRT_DC, tmp >> 8);
    /* cursor low port to vga index register */
    outb(VGA_CRT_IC, 0xf);
    outb(VGA_CRT_DC, tmp);
}

static void scroll(){
    uint16_t blank = color.ch | ((color.bc<<4)|color.fc)<<8;

    if (cur.y >= 25){   // it is strange, so be it temporarily  
        memcpy (vga_mem, vga_mem + 80, 80*24*2);
        memsetw((uint16_t *)vga_mem + 80*24, blank, 80);
        cur.y--;
    }
}

void cls(){
    int x, y;
    struct vga_char blank = {' ', COL_L_GREY, COL_BLACK};

    for (y = 0; y < 25; y++){
        for (x = 0; x < 80; x++){
            vga_mem[y*80 + x] = blank;
        }
    }
    cur.x = cur.y = 0;
    move_cur();
}

void vga_init(){
    cur.x = 0;
    cur.y = 5; // a ugly practice 
    move_cur();
    vga_mem = (struct vga_char *)0xb8000;
    color.ch = ' ';
    color.fc = COL_L_GREY;
    color.bc = COL_BLACK;
}

void putchar(char ch){
    /* writing to the console in bochs */
    bochs_putc(ch);

    switch (ch){
        case '\r': cur.x = 0; break;
        case '\n': cur.y = cur.y + 1; cur.x = 0; break;
        case '\b': cur.y -= (cur.x == 0)?1:0;
                   cur.x = (cur.x + 80 - 1)%80;
                   vga_mem[cur.y*80 + cur.x].ch = ' ';
                   vga_mem[cur.y*80 + cur.x].fc = color.fc;
                   vga_mem[cur.y*80 + cur.x].bc = color.bc;
                   break;
        case '\t': do putchar(' '); while (cur.x % 4 != 0); break;
        default: {
                     vga_mem[cur.y*80 + cur.x].ch = ch;
                     vga_mem[cur.y*80 + cur.x].fc = color.fc;
                     vga_mem[cur.y*80 + cur.x].bc = color.bc;
                     cur.y += (cur.x + 1)/80;
                     cur.x = (cur.x + 1)%80;
                 }
    }
    scroll();   // you should scroll before updata the cursor, or error
    move_cur();
}

void puts(char *str){
    for (; *str != '\0'; str++) putchar(*str);
}

void vga_setcolor(char fc, char bc){
    if (fc < 0 || fc > 16 || bc < 0 || bc > 16){
        return;
    }
    color.fc = fc;
    color.bc = bc;
}

struct point vga_getcur(){
    return cur;
}

void vga_setcur(int x, int y){
    if (x < 0 || x >= 80||y < 0||y >= 25){
        return;
    }
    cur.x = x;
    cur.y = y;
}

void vga_test(){
}
