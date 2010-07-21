#ifndef _AAGATHERER_H
#define _AAGATHERER_H

#include <rose.h>
#include <llvm/Pass.h>
#include <llvm/Analysis/Passes.h>
#include <llvm/Analysis/AliasAnalysis.h>
#include <llvm/Analysis/AliasSetTracker.h>
#include <string>

namespace llvm {
    llvm::FunctionPass* createAAGathererPass();
}

using namespace llvm;

class AAGatherer: public FunctionPass
{
    AliasSetTracker *_ASTracker;

    public:
        static char ID;
        AAGatherer() : FunctionPass(&ID) {}

        virtual void getAnalysisUsage(AnalysisUsage &AU) const {
            AU.addRequired<AliasAnalysis>();
            AU.setPreservesAll();            
        }

        bool doInitialization(Module &M) {
            return false;
        }

        /*
         * Perform Alias Analysis query on this function
         */
        bool runOnFunction(Function &F);

        bool doFinalization(Module &M);

    protected:
       /*
        * Write function to write PointerRec as String
        */

};

#endif
