#include <lib.h>

int strlen(const char *str){
    int len;
    for(len = 0; *str != '\0'; str++) len++;
    return len;
}

int strcmp(const char *str1, const char *str2){
    while (*str1 != '\0' || *str2 != '\0'){
        if (*str1 == *str2){
            str1++;
            str2++;
        }
        else return *str1 - *str2;
    }
    return 0;
}

void strcpy(const char *src, char *dist){
    //TODO
}

char *strcat(char dest const *src){
    //TODO
}


