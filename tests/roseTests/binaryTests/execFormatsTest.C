// This simple test code is used to read binary files for testing 
// of large collections of binaries using the runExecFormatsTest
// bash script.  This test code take any binary as input (technically
// it takes any source code (C, C++, Fortran, etc.) as well, since 
// nothing here is specific to binaries).

#include "rose.h"

int
main(int argc, char *argv[])
{

    SgProject *project= frontend(argc,argv);

 // Previous details to regenerate the binary from the AST for testing (*.new files) 
 // and to output a dump of the binary executable file format (*.dump files) has
 // been combined with the output of the disassembled instructions in the "backend()".

    return backend(project);
}

