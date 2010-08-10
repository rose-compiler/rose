#ifndef OPTIMIZE
#define OPTIMIZE

#include <assert.h>
#include <iostream>
#include <vector>
#include <string>
#include <rosetollvm/Option.h>

#include "llvm/LLVMContext.h"
#include "llvm/Module.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/Support/SourceMgr.h" // contains class SMDiagnostic;
#include "llvm/Assembly/Parser.h"
#include "llvm/Support/raw_ostream.h" // contains raw_fd
#include "llvm/PassManager.h"
#include "llvm/CallGraphSCCPass.h"
#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/Assembly/AsmAnnotationWriter.h"
#include "llvm/Assembly/PrintModulePass.h"
#include "llvm/Analysis/LoopPass.h"
#include "llvm/Analysis/CallGraph.h"
#include "llvm/Target/TargetData.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Support/PassNameParser.h"
#include "llvm/System/Signals.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/IRReader.h"
#include "llvm/Support/ManagedStatic.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/PluginLoader.h"
#include "llvm/Support/PrettyStackTrace.h"
#include "llvm/Support/StandardPasses.h"
#include "llvm/Support/SystemUtils.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/LinkAllPasses.h"
#include "llvm/LinkAllVMCore.h"
#include <memory>

class Optimize {
public: 
    Optimize(Option &option_) : option(option_)
    {
        std::string opt = option.getOptimize();
        OptLevelO1 = (opt.compare("-O1") == 0);
        OptLevelO2 = (opt.compare("-O2") == 0 || opt.compare("-O") == 0);
        OptLevelO3 = (opt.compare("-O3") == 0);
        UnitAtATime = true;
    }

    void process(llvm::Module *);

private:

    void AddOptimizationPasses(llvm::PassManager &, llvm::FunctionPassManager &, unsigned);

    Option &option;

    bool OptLevelO1,
         OptLevelO2,
         OptLevelO3,

         UnitAtATime;

};

#endif
