int osmain(void)
{
    char *video_memory = (char *)0xb8000;
    int i;
    for (i = 0; i < 10; i+= 2) video_memory[i] = 'Y';;
    for (;;);
}
