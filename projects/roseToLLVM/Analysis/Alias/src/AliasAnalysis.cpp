#include <iostream>
#include "rose.h"
#include "rosetollvm/RoseToLLVMModule.hpp"

using namespace std;

int main(int argc, char *argv[])
{
    SgStringList args = CommandlineProcessing::generateArgListFromArgcArgv(argc, argv);
    RoseToLLVMModule roseToLLVM(args);
    SgProject *astRoot = frontend(args);
    return roseToLLVM.visit(astRoot);   
}


