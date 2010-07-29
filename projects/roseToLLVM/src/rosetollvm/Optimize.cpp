#include <rosetollvm/Optimize.h>

using namespace std;
using namespace llvm;

/**
 * This function was adapted from LLVM's opt.cpp.
 */


//
/// AddOptimizationPasses - This routine adds optimization passes
/// based on selected optimization level, OptLevel. This routine
/// duplicates llvm-gcc behaviour.
///
/// OptLevel - Optimization Level
//
void Optimize::AddOptimizationPasses(PassManager &MPM, FunctionPassManager &FPM, unsigned OptLevel) {
    createStandardFunctionPasses(&FPM, OptLevel);

    llvm::Pass *InliningPass = 0;
    /*
    if (DisableInline) {
        // No inlining pass
    } else */ if (OptLevel) {
        unsigned Threshold = 200;
        if (OptLevel > 2)
            Threshold = 250;
        InliningPass = createFunctionInliningPass(Threshold);
    } else {
        InliningPass = createAlwaysInlinerPass();
    }
    createStandardModulePasses(&MPM, OptLevel,
                               /*OptimizeSize=*/ false,
                               UnitAtATime,
                               /*UnrollLoops=*/ OptLevel > 1,
                               /*!DisableSimplifyLibCalls */ true,
                               /*HaveExceptions=*/ true,
                               InliningPass);
}

void Optimize::process(Module *module) {
  //    if (false) {
        //
        // Create a PassManager to hold and optimize the collection of passes we are
        // about to build...
        //
        PassManager Passes;

        //
        // Add an appropriate TargetData instance for this module...
        //
        TargetData *TD = 0;
        const std::string &ModuleDataLayout = module -> getDataLayout();
        if (! ModuleDataLayout.empty())
            TD = new TargetData(ModuleDataLayout);
        /*
        else if (!DefaultDataLayout.empty())
            TD = new TargetData(DefaultDataLayout);
        */

        if (TD)
            Passes.add(TD);

        FunctionPassManager *FPasses = NULL;
        if (OptLevelO1 || OptLevelO2 || OptLevelO3) {
            FPasses = new FunctionPassManager(module);
            if (TD)
                FPasses -> add(new TargetData(*TD));
        }

        //
        // If the -strip-debug command line option was specified, add it.  If
        // -std-compile-opts was also specified, it will handle StripDebug.
        //
        /*
        if (StripDebug && !StandardCompileOpts)
            addPass(Passes, createStripSymbolsPass(true));
        */

        //
        // Create a new optimization pass for each one specified on the command line
        //
        /*
        for (unsigned i = 0; i < PassList.size(); ++i) {
            //
            // Check to see if -std-compile-opts was specified before this option.  If
            // so, handle it.
            //
            if (StandardCompileOpts &&
                StandardCompileOpts.getPosition() < PassList.getPosition(i)) {
                AddStandardCompilePasses(Passes);
                StandardCompileOpts = false;
            }

            if (StandardLinkOpts &&
                StandardLinkOpts.getPosition() < PassList.getPosition(i)) {
                AddStandardLinkPasses(Passes);
                StandardLinkOpts = false;
            }

            if (OptLevelO1 && OptLevelO1.getPosition() < PassList.getPosition(i)) {
                AddOptimizationPasses(Passes, *FPasses, 1);
                OptLevelO1 = false;
            }

            if (OptLevelO2 && OptLevelO2.getPosition() < PassList.getPosition(i)) {
                AddOptimizationPasses(Passes, *FPasses, 2);
                OptLevelO2 = false;
            }

            if (OptLevelO3 && OptLevelO3.getPosition() < PassList.getPosition(i)) {
                AddOptimizationPasses(Passes, *FPasses, 3);
                OptLevelO3 = false;
            }

            const PassInfo *PassInf = PassList[i];
            Pass *P = 0;
            if (PassInf->getNormalCtor())
                P = PassInf->getNormalCtor()();
            else
                errs() << argv[0] << ": cannot create pass: "
                       << PassInf->getPassName() << "\n";
            if (P) {
                PassKind Kind = P->getPassKind();
                addPass(Passes, P);

                if (AnalyzeOnly) {
                    switch (Kind) {
                        case PT_BasicBlock:
                             Passes.add(new BasicBlockPassPrinter(PassInf));
                             break;
                        case PT_Loop:
                             Passes.add(new LoopPassPrinter(PassInf));
                             break;
                        case PT_Function:
                             Passes.add(new FunctionPassPrinter(PassInf));
                             break;
                        case PT_CallGraphSCC:
                             Passes.add(new CallGraphSCCPassPrinter(PassInf));
                             break;
                        default:
                             Passes.add(new ModulePassPrinter(PassInf));
                             break;
                    }
                }
            }

            if (PrintEachXForm)
                Passes.add(createPrintModulePass(&errs()));
        }
        */

        //
        // If -std-compile-opts was specified at the end of the pass list, add them.
        //
        /*
        if (StandardCompileOpts) {
            AddStandardCompilePasses(Passes);
            StandardCompileOpts = false;
        }

        if (StandardLinkOpts) {
            AddStandardLinkPasses(Passes);
            StandardLinkOpts = false;
        }
        */

        if (OptLevelO1)
            AddOptimizationPasses(Passes, *FPasses, 1);

        if (OptLevelO2)
            AddOptimizationPasses(Passes, *FPasses, 2);

        if (OptLevelO3)
            AddOptimizationPasses(Passes, *FPasses, 3);

        if (OptLevelO1 || OptLevelO2 || OptLevelO3) {
            FPasses -> doInitialization();
            for (Module ::iterator I = module -> begin(), E = module -> end(); I != E; ++I)
                FPasses -> run(*I);
        }

        //
        // Check that the module is well formed on completion of optimization
        //
        /*
        if (!NoVerify && !VerifyEach)
        */
            Passes.add(createVerifierPass());

        //
        // Write bitcode or assembly out to disk or outs() as the last step...
        //
        /*
        if (! NoOutput && !AnalyzeOnly) {
            if (OutputAssembly)
                Passes.add(createPrintModulePass(Out));
            else
                Passes.add(createBitcodeWriterPass(*Out));
        }
        */

        //
        // Now that we have all of the passes ready, run them.
        //
        Passes.run(*module);
	//    }
}
