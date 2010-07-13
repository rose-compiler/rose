#include <vector>
#include <rose.h>
#include <rosetollvm/RoseToLLVMModule.hpp>

using namespace std;

/**
 * Main program for Rose to LLVM translator.
 */
int main(int argc, char *argv[]) {

    // 1- Get arguments
    SgStringList args = CommandlineProcessing::generateArgListFromArgcArgv (argc, argv);

    /**
     * Start pace-cc translation... First, reprocess the options to retrieve the ones that are relevant to pace-cc.
     */
    RoseToLLVMModule roseToLLVM(args);

    /**
     * Call Rose to construct AST.
     */
    SgProject *astRoot = frontend(args); // Invoke Rose front end to process input

    return roseToLLVM.visit(astRoot);
}
