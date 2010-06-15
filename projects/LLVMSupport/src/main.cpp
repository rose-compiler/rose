#include "rose.h"
#include <iostream>
#include "VisitorTraversal.h"
#include "llvm/Module.h"
#include "llvm/LLVMContext.h"

using namespace llvm;

int main(int argc, char *argv[])
{
    SgProject *project = frontend(argc, argv);
    ROSE_ASSERT(project != NULL);
    
    LLVMContext &context = getGlobalContext();
    Module *module = new Module("", context);
    module->dump();
    return 0;
}