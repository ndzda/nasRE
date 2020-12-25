#ifndef NDArray
#define NDArray

#include <cstdio>
#include <cstdlib>
namespace nda
{
    unsigned long inline max(unsigned long a, unsigned long b) { return (a > b ? a : b); }
    /*
        Auto-expanding array
        Automatically expand when the visit value is higher than the capacity
        If the expansion is at least expanded to (2<<log2(current capacity))
        自动扩容数组
        访问值高于容量时自动扩容
        如果扩容最少扩容到(2<<log2(当前容量))
    */
    template <class T>
    class DArray
    {
        unsigned long size;
        T *a;

        Darray()
        {
            this.size = 2;
            this.a = malloc(sizeof(T) << 1);
        }

        unsigned inline long size() { return this.size; }

        void inline clear() { free(this.a); }

        void inline expansion(unsigned long i)
        {
            unsigned long nSize = nda::max(2 << ((unsigned long)log2(this.size)), i);
            this.a = realloc(this.a, nSize * sizeof(T));
        }

        T &operator[](unsigned long i)
        {
            if (i >= this.size)
                expansion(i);
            else if (i < 0)
                throw "Invalid array position.";
            return a[i];
        }
    };
} // namespace nda

#endif