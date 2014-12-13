/* control the display of char, 
 * you must invoke init_vga before use other func
 */
#include <sys.h>

/* it is strange that i can't use 
 * char* vgamem = 0xb8000, 
 * it can only disp blank char */
struct vga_char{
    char _char: 8;
    char f_color: 4;
    char b_color: 4;
};
struct vga_char *vgamem;
struct vga_char color;  // use vag_char structure to store color

int cur_x, cur_y;

void move_cur(){
    unsigned temp;
    temp = cur_y * 80 + cur_x;

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
    if (cur_y >= 25){   // it is strange, so be it temporarily  
        memcpy (vgamem, vgamem + 80, 80*24*2);
        memsetw((unsigned short *)vgamem + 80*24, blank, 80);
        cur_y--;
    }
}

void vga_init() {
    cur_x = 0;
    cur_y = 5; // a ugly practice 
    move_cur();
    vgamem = (struct vga_char *)0xb8000;
    color._char = ' ';
    color.f_color = COL_L_GREY;
    color.b_color = COL_BLACK;
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
    move_cur();
}

void putchar(char ch){
    switch (ch){
        case '\r': cur_x = 0; break;
        case '\n': cur_y = cur_y + 1; break;
        default: {
                     vgamem[cur_y*80 + cur_x]._char = ch;
                     vgamem[cur_y*80 + cur_x].f_color = color.f_color;
                     vgamem[cur_y*80 + cur_x].b_color = color.b_color;
                     cur_y += (cur_x + 1)/80;
                     cur_x = (cur_x + 1)%80;
                 }
    }
    scroll();   // you should scroll before updata the cursor, or error
    move_cur();
}

void puts(char *str){
    for (; *str != '\0'; str++) putchar(*str);
}

void setcolor(char f_color, char b_color){
    color.f_color = f_color;
    color.b_color = b_color;
}

void vga_test(){
    scroll(); 
}

