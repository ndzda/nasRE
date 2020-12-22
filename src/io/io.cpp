#include <stdio.h>

namespace io
{
    #include "io/file.cpp"
    #include "io/math.cpp"


    void inline info(char *s)
    {
        printf("info:%s", s);
    }

    void inline error(char *s)
    {
        printf("error:%s", s);
    }
}