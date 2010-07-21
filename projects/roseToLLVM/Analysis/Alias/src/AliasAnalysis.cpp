#include <iostream>
#include <rose.h>
#include <AliasAnalysisModule.h>
#include <llvm/Module.h>
#include <llvm/Pass.h>
#include <llvm/PassManager.h>
#include <llvm/Analysis/AliasSetTracker.h>
#include <llvm/Analysis/AliasAnalysis.h>
#include <llvm/Analysis/Passes.h>
#include <AliasAnalysisGatherer.h>

using namespace std;

int main(int argc, char *argv[])
{
    SgStringList args = CommandlineProcessing::generateArgListFromArgcArgv(argc, argv);

    /*
     * Generate AST
     */
    SgProject *astRoot = frontend(args);

    AliasAnalysisModule *AAModule = new AliasAnalysisModule(args);

    /*
     * Generate LLVM Modules for all files
     */
    AAModule->visit(astRoot);

    // Create Pass Manager to manage LLVM Passes
    llvm::PassManager *PM = new llvm::PassManager();

    /*
     * Apply Alias Analysis passes on all LLVM Modules
     */
    for(int i = 0; i < AAModule->getLLVMModuleSize(); ++i) {
        llvm::Module *ModRef = AAModule->getModule(i);
        assert(ModRef != NULL);

        /*
         * Add Alias Analysis Passes
         */
        PM->add(createBasicAliasAnalysisPass());
        PM->add(createAAGathererPass());
        PM->run(*ModRef);       
    }

    AAModule->annotateAST(astRoot);   

    delete AAModule;
    delete PM;
   
    return 0;
}
