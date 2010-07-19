#ifndef OBJECT_GENERATOR
#define OBJECT_GENERATOR

#include <assert.h>
#include <iostream>
#include <vector>
#include <string>
#include <rosetollvm/Option.h>

#include "llvm/LLVMContext.h"
#include "llvm/Module.h"
#include "llvm/PassManager.h"
#include "llvm/Pass.h"
#include "llvm/ADT/Triple.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/Support/IRReader.h"
#include "llvm/CodeGen/LinkAllAsmWriterComponents.h"
#include "llvm/CodeGen/LinkAllCodegenComponents.h"
#include "llvm/Config/config.h"
#include "llvm/LinkAllVMCore.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/FileUtilities.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/ManagedStatic.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/PluginLoader.h"
#include "llvm/Support/PrettyStackTrace.h"
#include "llvm/System/Host.h"
#include "llvm/System/Signals.h"
#include "llvm/Target/SubtargetFeature.h"
#include "llvm/Target/TargetData.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetRegistry.h"
#include "llvm/Target/TargetSelect.h"
#include "llvm/Transforms/Scalar.h"
#include <memory>

using namespace llvm;

class ObjectGenerator {
public: 
    ObjectGenerator(Option &option_) : option(option_)
    {
        std::string opt = option.getOptimize();
        OptLevelO1 = (opt.compare("-O1") == 0);
        OptLevelO2 = (opt.compare("-O2") == 0 || opt.compare("-O") == 0);
        OptLevelO3 = (opt.compare("-O3") == 0);

        OptLevel = (OptLevelO1 ? '1'
                               : OptLevelO2 ? '2'
                                            : OptLevelO3 ? '3'
                                                         : '0');
    }

    int process(llvm::Module *);

private:

    Option &option;

    static inline std::string GetFileNameRoot(const std::string &InputFilename);
    static formatted_raw_ostream *GetOutputStream(const char *TargetName, const char *ProgName);

    char OptLevel;

    bool OptLevelO1,
         OptLevelO2,
         OptLevelO3;

};

#endif
