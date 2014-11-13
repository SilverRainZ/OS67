void main(void)
{
	unsigned short *vidmem = (unsigned short*) 0xb8000;
    vidmem[12] = 'A';
    for (;;);
}
