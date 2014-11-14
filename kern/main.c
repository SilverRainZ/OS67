int osmain(void)
{
    char *video_memory = (char *)0xb8000;
    int i;
    char a = 'A';
    for (i = 0; i < 100; i += 2) video_memory[i] = (a++)%256;
    for (;;);
}
