#include "io.cpp"
#include "vm.cpp"
using namespace io;


int main(int argc,char *argv[])
{
    if(argc<=1)
    {
        error("Please pass in the binary file name as the startup parameter.");
        return 1;
    }

    char *binCode;
    long bcLen=readBFile(argv[1],&binCode);

    return 0;
}