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
struct tpoint{
    int x;
    int y;
};
void vga_init();
void cls();
void putchar_at(int x, int y, char ch);
void putchar(char ch);
void puts(char *str);
void setcolor(char b_color, char f_color);
struct tpoint getcur();
void setcur(int x, int y);
#endif
