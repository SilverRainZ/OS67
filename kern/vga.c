/* control the display of char, you must invoke init_vga before use other func
 */
#include <sys.h>

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
// L_ == Light D_ == Dark

// it is strange that i can't use unsignd int vgamem = 0xb8000, it can only disp blank char
struct vga_char{
    char _char: 8;
    char f_color: 4;
    char b_color: 4;
};
struct vga_char *vgamem;

int cur_x, cur_y;
void init_vga(){
    cur_x = 0;
    cur_y = 4;
    vgamem = (struct vga_char *)0xb8000;
}

void cls(){
    int x, y;
    struct vga_char blank = {' ',COL_L_GREY,COL_BLACK};
    for (y = 0; y < 25; y++){
        for (x = 0; x < 80; x++){
            vgamem[y*80 + x] = blank;
        }
    }
    cur_x = cur_y = 0;
}

void putchar(unsigned char ch){
    switch (ch){
        case '\r': cur_x = 0; break;
        case '\n': cur_y++; break;
        default: {
                     vgamem[cur_y*80 + cur_x]._char = ch;
                     vgamem[cur_y*80 + cur_x].f_color = COL_L_GREY;
                     vgamem[cur_y*80 + cur_x].b_color = COL_BLACK;
                     cur_y += (cur_x + 1)/80;
                     cur_x = (cur_x + 1)%80;
                 }
    }
}

void puts(char *str){
    int i = 0;
    for (i = 0; i < 6; i ++) putchar(str[i]);
    putchar('A');
}
