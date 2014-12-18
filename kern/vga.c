/* control the display of char, 
 * you must invoke init_vga before use other func
 */
#include <sys.h>
#include <const.h>
#include <lib.h>

/* it is strange that i can't use 
 * char* vgamem = 0xb8000, 
 * it can only disp blank char */
struct vga_char{
    char _char: 8;
    char f_color: 4;
    char b_color: 4;
};
struct vga_char *vgamem;    // vga[25][80] 
struct vga_char color;      // use vag_char structure to store color

struct tpoint cur;

void move_cur(){
    unsigned temp;
    temp = cur.y * 80 + cur.x;

    /* This sends a command to indicies 14 and 15 in the
    *  CRT Control Register of the VGA controller. These
    *  are the high and low bytes of the index that show
    *  where the hardware cursor is to be 'blinking'. To
    *  learn more, you should look up some VGA specific
    *  programming documents. A great start to graphics:
    *  http://www.brackeen.com/home/vga */
    outportb(0x3D4, 14);
    outportb(0x3D5, temp >> 8);
    outportb(0x3D4, 15);
    outportb(0x3D5, temp);
}

void scroll(){
    unsigned short blank = color._char|((color.b_color<<4)|color.f_color)<<8;
    if (cur.y >= 25){   // it is strange, so be it temporarily  
        memcpy (vgamem, vgamem + 80, 80*24*2);
        memsetw((unsigned short *)vgamem + 80*24, blank, 80);
        cur.y--;
    }
}

void cls(){
    int x, y;
    struct vga_char blank = {' ',COL_L_GREY,COL_BLACK};
    for (y = 0; y < 25; y++){
        for (x = 0; x < 80; x++){
            vgamem[y*80 + x] = blank;
        }
    }
    cur.x = cur.y = 0;
    move_cur();
}

void vga_init(){
    cur.x = 0;
    cur.y = 5; // a ugly practice 
    move_cur();
    vgamem = (struct vga_char *)0xb8000;
    color._char = ' ';
    color.f_color = COL_L_GREY;
    color.b_color = COL_BLACK;
}

void putchar(char ch){
    switch (ch){
        case '\r': cur.x = 0; break;
        case '\n': cur.y = cur.y + 1; break;
        case '\b': cur.y -= (cur.x == 0)?1:0;
                   cur.x = (cur.x + 80 - 1)%80; break;
        default: {
                     vgamem[cur.y*80 + cur.x]._char = ch;
                     vgamem[cur.y*80 + cur.x].f_color = color.f_color;
                     vgamem[cur.y*80 + cur.x].b_color = color.b_color;
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

void setcolor(char f_color, char b_color){
    if (f_color < 0||f_color > 16||b_color < 0||b_color > 16)
        return;
    color.f_color = f_color;
    color.b_color = b_color;
}

struct tpoint getcur(){
    return cur;
}

void setcur(int x, int y){
    if (x < 0||x >= 80||y < 0||y >= 25){
        return;
    }
    cur.x = x;
    cur.y = y;
}

void vga_test(){
    scroll(); 
}

