#include "io/io.cpp"
#include "vm/vm.cpp"



int main(int argc,char *argv[])
{
    if(argc<=1){
        error("Please pass in the binary file name as the startup parameter.");
        return 1;
    }

    char *binCode;
    long bcLen=readFile(argv[1],&binCode);

    return 0;
}