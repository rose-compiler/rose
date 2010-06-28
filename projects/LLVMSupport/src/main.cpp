#include "rose.h"
#include <iostream>
#include "VisitorTraversal.h"
#include "llvm/Module.h"
#include "llvm/Function.h"
#include "llvm/PassManager.h"
#include "llvm/CallingConv.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/Assembly/PrintModulePass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm-interface.h"
#include "llvm/LLVMContext.h"
#include "FunctionToLLVM.h"
#include "util.h"

using namespace llvm;

int main(int argc, char *argv[])
{
    SgProject *project = frontend(argc, argv);
    ROSE_ASSERT(project != NULL);
    initialize_LLVM();
    
    // query function declaration nodes for function definitions
    Rose_STL_Container<SgNode*> fndecl_list = NodeQuery::querySubTree(project, &queryForFunctionDefs);
    for(Rose_STL_Container<SgNode*>::iterator i = fndecl_list.begin(); i != fndecl_list.end(); i++)
    {
        FunctionToLLVM toLLVM(*i);
        toLLVM.EmitFunction();
    }

    verifyModule(*TheModule, PrintMessageAction);
    PassManager PM;
    PM.add(createPrintModulePass(&outs()));
    PM.run(*TheModule);
    delete TheModule;

    return 0;
}
