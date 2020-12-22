#ifndef nasIo
#define nasIo

#include <stdio.h>
#include "io/math.cpp"
#include "io/file.cpp"
namespace io
{
    void inline info(const char *s)
    {
        printf("info:%s", s);
    }

    void inline error(const char *s)
    {
        printf("error:%s", s);
    }
}

#endif