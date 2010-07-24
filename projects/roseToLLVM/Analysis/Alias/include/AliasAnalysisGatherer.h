#ifndef _AAGATHERER_H
#define _AAGATHERER_H

#include <rose.h>
#include <llvm/Pass.h>
#include <llvm/Analysis/Passes.h>
#include <llvm/Analysis/AliasAnalysis.h>
#include <llvm/Analysis/AliasSetTracker.h>
#include <llvm/Function.h>
#include <string>
#include <AliasSetContainer.h>

namespace llvm {
    llvm::FunctionPass* createAAGathererPass();
}

using namespace llvm;

class AAGatherer: public FunctionPass
{
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
         * Write Aliasing locations as string
         */                          
        std::string WriteAsString(AliasSetTracker *_asTracker, Function& F);

        /*
         * Set of aliasing locations for each memory reference
         */
        

        /*
         * Add blocks of interest for Alias Analysis
         */
        void addblocks(AliasSetTracker* _asTracker, Function& F);

};

#endif
