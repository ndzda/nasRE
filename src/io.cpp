#ifndef nasIo
#define nasIo

#include <cstdio>
#include "io/math.cpp"
#include "io/file.cpp"
#include "io/time.cpp"
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