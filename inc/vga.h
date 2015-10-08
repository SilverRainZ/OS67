#ifndef __VGA_H
#define __VAG_H

#define COL_BLACK   0
#define COL_BLUE    1
#define COL_GREEN   2
#define COL_CYAN    3
#define COL_RED     4
#define COL_MAGENTA 5
#define COL_BROWN   6
#define COL_L_GREY  7
#define COL_D_GREY  8
#define COL_L_BLUE  9
#define COL_L_GREEN 10
#define COL_L_CYAN  11
#define COL_L_RED   12
#define COL_L_MAG   13
#define COL_L_BROWN 14
#define COL_WHITE   15
/* L_ == Light D_ == Dark */

#define VGA_CRT_IC  0x3d4   // vga index register port
#define VGA_CRT_DC  0x3d5   // vga data register port

struct point{
    int x;
    int y;
};

struct vga_char{
    char ch: 8;
    char fc: 4; // foreground color
    char bc: 4; // background color
};

void vga_init();
void cls();
void putchar_at(int x, int y, char ch);
void putchar(char ch);
void puts(char *str);
void vga_setcolor(char f_color, char b_color);
struct point vga_getcur();
void vga_setcur(int x, int y);
#endif
