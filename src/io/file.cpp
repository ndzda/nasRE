#include <cstdio>
#include <cstdlib>

namespace io
{
    long readBFile(const char *path, char **s)
    {
        FILE *file = fopen(path, "r");

        fseek(file, 0, SEEK_END);

        long fileSize = ftell(file);
        *s = (char *)malloc(fileSize + 1);

        rewind(file);

        fread(*s,sizeof(char),fileSize,file);

        *((*s)+fileSize) = 0;

        if (fclose(file) == 0)
            return fileSize;
        else
            return -1L;
    }
}