#include "rose.h"
#include <iostream>
#include "VisitorTraversal.h"
#include "llvm-interface.h"

using namespace std;

int main(int argc, char *argv[])
{
    SgProject *project = frontend(argc, argv);
    ROSE_ASSERT(project != NULL);
    VisitorTraversal Traversal;
    Traversal.traverseInputFiles(project, preorder);
    return 0;
}
