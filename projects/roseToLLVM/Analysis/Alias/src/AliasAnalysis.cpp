#include <iostream>
#include <rose.h>
#include <AliasAnalysisModule.h>
#include <llvm/Module.h>
#include <llvm/Pass.h>
#include <llvm/PassManager.h>
#include <llvm/Analysis/AliasSetTracker.h>
#include <llvm/Analysis/AliasAnalysis.h>
#include <llvm/Analysis/Passes.h>

using namespace std;

int main(int argc, char *argv[])
{
    SgStringList args = CommandlineProcessing::generateArgListFromArgcArgv(argc, argv);

    /*
     * Generate AST
     */
    SgProject *astRoot = frontend(args);

    AliasAnalysisModule AAModule(args);

    /*
     * Generate LLVM Modules for all files
     */
    AAModule.visit(astRoot);

    llvm::Module *ModRef = AAModule.getModule(0);
/*    llvm::AliasAnalysis AA;
    llvm::AliasSetTracker _ASTracker(AA);

    llvm::Module::iterator _fiterator = ModRef->begin();
    llvm::Module::iterator _fiterator_end = ModRef->end();

    for( ; _fiterator != _fiterator_end; ++_fiterator) {
        llvm::Function *func = dyn_cast<llvm::Function> (_fiterator);

        llvm::Function::iterator _bbiterator = func->begin();
        llvm::Function::iterator _bbiterator_end = func->end();

        for( ; _bbiterator != _bbiterator_end; ++_bbiterator) {
            llvm::BasicBlock *BB = dyn_cast<llvm::BasicBlock> (_bbiterator);
            _ASTracker.add(*BB);
        }

    }*/

    llvm::PassManager PM;

    PM.add(createBasicAliasAnalysisPass());
    PM.run(*ModRef);
   
    return 0;
}
