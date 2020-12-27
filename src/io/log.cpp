#include <cstdio>

namespace io
{
    void inline info(const char *s)
    {
        printf("info: %s\n", s);
    }

    void inline debug(const char *s)
    {
        printf("debug: %s\n", s);
    }

    void inline error(const char *s)
    {
        printf("error: %s\n", s);
    }
}