/* asm.c
 * some short, assembly function.
 */
// std
#include <type.h>
#include <asm.h> 

/* read a byte from a port */
inline uint8_t inb (uint16_t port){
    uint8_t rv;
    __asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (port));
    return rv;
}

/* write a byte to a port */
inline void outb (uint16_t port, uint8_t data){
    __asm__ __volatile__ ("outb %1, %0" : : "dN" (port), "a" (data));
}

/* write a word to a port */
inline void outw (uint16_t port, uint16_t data){
    __asm__ __volatile__ ("outw %1, %0" : : "dN" (port), "a" (data));
}

/* read a word from a port */
inline uint16_t inw (uint16_t port){
    uint16_t rv;
    __asm__ __volatile__ ("inw %1, %0" : "=a" (rv) : "dN" (port));
    return rv;
}


/* read a string(start at addr, length cnt) from a port */
inline void insl(uint32_t port, void *addr, uint32_t cnt){
    __asm__ __volatile__("cld; rep insl" 
                        : "=D" (addr), "=c" (cnt) 
                        : "d" (port), "0" (addr), "1" (cnt) 
                        : "memory", "cc"
                        );
}

/* write a string(start at addr, length cnt) to a port */
inline void outsl(uint32_t port, const void *addr, uint32_t cnt) {
    __asm__ __volatile__("cld; rep outsl" 
                        : "=S" (addr), "=c" (cnt) 
                        : "d" (port), "0" (addr), "1" (cnt) 
                        : "cc"
                        );
}

/* clear interrupt */
inline void cli(){
    __asm__ __volatile__ ("cli");
}

/* set interrupt */
inline void sti(){
    __asm__ __volatile__ ("sti");
}

/* enter halt state, until int comeup */
inline void hlt(){
    __asm__ __volatile__ ("hlt");
}
