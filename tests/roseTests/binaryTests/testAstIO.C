/* Reads a binary file, writes out the AST, then reads the AST */
#include "rose.h"

int
main(int argc, char *argv[])
{
    SgProject *p1 = frontend(argc, argv);
    ROSE_ASSERT(p1!=NULL);
    AST_FILE_IO::startUp(p1);
    AST_FILE_IO::writeASTToFile("x.ast");
    
    AST_FILE_IO::clearAllMemoryPools();
    SgProject *p2 = AST_FILE_IO::readASTFromFile("x.ast");
    ROSE_ASSERT(p2!=NULL);

    return 1;
}
