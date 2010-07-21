#include <AliasAnalysisGatherer.h>
#include <llvm/Analysis/AliasSetTracker.h>
#include <llvm/Function.h>
#include <llvm/ADT/DenseSet.h>

using namespace llvm;

char AAGatherer::ID = 0;

static RegisterPass<AAGatherer>
X("aa-gather", "Gather Alias Analysis Results", false, true);

FunctionPass* llvm::createAAGathererPass() { return new AAGatherer(); }


bool AAGatherer::runOnFunction(Function &F) {
    AliasAnalysis &AA = getAnalysis<AliasAnalysis> ();    
    AliasSetTracker *_ASTracker = new AliasSetTracker(AA);

    // Gather Alias Sets
    Function::iterator iter;
    for( iter = F.begin(); iter != F.end(); ++iter) {
        BasicBlock *_bb = dyn_cast<BasicBlock> (iter);
        _ASTracker->add(*_bb);
    }

    AliasSetTracker::iterator _as_iter;

/*    for(_as_iter = _ASTracker->begin(); _as_iter != _ASTracker->end(); ++_as_iter) {        
        AliasSet *_aliasset = static_cast<AliasSet*> (_as_iter);     
    }*/
    _ASTracker->dump();

    delete _ASTracker;
}

bool AAGatherer::doFinalization(Module &M) {
    std::cout << "End of AA Gatherer Pass " << std::endl;
    return false;
}
