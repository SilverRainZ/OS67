#include <sys.h> 

/* read a byte from a port */
unsigned char inportb (unsigned short _port){
    unsigned char rv;
    __asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (_port));
    return rv;
}

/*write a byte to a port */
void outportb (unsigned short _port, unsigned char _data){
    __asm__ __volatile__ ("outb %1, %0" : : "dN" (_port), "a" (_data));
}

/* clear interrupt */
void cli(){
    __asm__ __volatile__ ("cli");
}

/* set interrupt */
void sti(){
    __asm__ __volatile__ ("sti");
}

/* enter halt state, until int comeup */
void hlt(){
    __asm__ __volatile__ ("hlt");
}

