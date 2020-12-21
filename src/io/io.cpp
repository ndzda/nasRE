#include <stdio.h>

void inline info(char s[])
{
    printf("info:%s", s);
}

void inline error(char s[])
{
    printf("error:%s", s);
}