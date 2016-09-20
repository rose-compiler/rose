/*
Bug report submitted by Jeremiah Willcock:
   ROSE specific options put onto the command line are not removed 
before handing off the commandline to the g++ compiler (backend compiler).

Almost any file will work to demonstrate this problem.

This output demonstrates that the file is processed normally:
tux49.llnl.gov{dquinlan}34: /home/dquinlan2/ROSE/LINUX-3.3.2/bin/identityTranslator foo.C
This SgProject constructor is deprecated, do not use.
Finished building EDG AST, now build the SAGE AST ...
/* AST Fixes started. */
/* AST Fixes fixup initializers */
/* AST Fixes empty operator nodes */
/* AST Fixes reset pointers */
/* AST Fixes finished */
C++ source(s) parsed. AST generated.
Now call the backend (vendor's) compiler for file = rose_foo.C


This output demonstrates the problem:
tux49.llnl.gov{dquinlan}35: /home/dquinlan2/ROSE/LINUX-3.3.2/bin/identityTranslator -rose:unparse_includes foo.C
This SgProject constructor is deprecated, do not use.
option -rose:unparse_includes found
Finished building EDG AST, now build the SAGE AST ...
/* AST Fixes started. */
/* AST Fixes fixup initializers */
/* AST Fixes empty operator nodes */
/* AST Fixes reset pointers */
/* AST Fixes finished */
C++ source(s) parsed. AST generated.
Now call the backend (vendor's) compiler for file = rose_foo.C
g++: unrecognized option `-rose:unparse_includes'

*/


#include <stdio.h>

int main(int, char**) {
        printf("Hello, world!\n");
        return 0;
}
