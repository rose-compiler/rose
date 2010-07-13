#include <rosetollvm/ObjectGenerator.h>

using namespace std;
using namespace llvm;

/**
 * This function was adapted from LLVM's llc.cpp.
 */

//
// General options for llc.  Other pass-specific options are specified
// within the corresponding llc passes, and target-specific options
// and back-end code generation options are specified with the target machine.
//
static cl::opt<std::string>  InputFilename(cl::Positional, cl::desc("<input bitcode>"), cl::init("-"));
static cl::opt<std::string>  OutputFilename("o", cl::desc("Output filename"), cl::value_desc("filename"));
static cl::opt<std::string>  TargetTriple("mtriple", cl::desc("Override target triple for module"));
static cl::opt<std::string>  MArch("march", cl::desc("Architecture to generate code for (see --version)"));
static cl::opt<std::string>  MCPU("mcpu", cl::desc("Target a specific cpu type (-mcpu=help for details)"), cl::value_desc("cpu-name"), cl::init(""));
static cl::list<std::string> MAttrs("mattr", cl::CommaSeparated, cl::desc("Target specific attributes (-mattr=help for details)"), cl::value_desc("a1,+a2,-a3,..."));
static cl::opt<bool>         DisableRedZone("disable-red-zone", cl::desc("Do not emit code that uses the red zone."), cl::init(false));
static cl::opt<bool>         NoImplicitFloats("no-implicit-float", cl::desc("Don't generate implicit floating point instructions (x86-only)"), cl::init(false));
cl::opt<bool>                NoVerify("disable-verify", cl::Hidden, cl::desc("Do not verify input module"));

cl::opt<TargetMachine::CodeGenFileType> FileType("filetype", cl::init(TargetMachine::CGFT_AssemblyFile),
                                                             cl::desc("Choose a file type (not all types are supported by all targets):"),
                                                             cl::values(clEnumValN(TargetMachine::CGFT_AssemblyFile, "asm", "Emit an assembly ('.s') file"),
                                                                        clEnumValN(TargetMachine::CGFT_ObjectFile, "obj", "Emit a native object ('.o') file [experimental]"),
                                                                        clEnumValN(TargetMachine::CGFT_Null, "null", "Emit nothing, for performance testing"),
                                                                        clEnumValEnd));
//
// GetFileNameRoot - Helper function to get the basename of a filename.
//
inline std::string ObjectGenerator::GetFileNameRoot(const std::string &InputFilename) {
    std::string IFN = InputFilename;
    std::string outputFilename;
    int Len = IFN.length();
    if ((Len > 2) &&
        IFN[Len-3] == '.' &&
        ((IFN[Len-2] == 'b' && IFN[Len-1] == 'c') ||
         (IFN[Len-2] == 'l' && IFN[Len-1] == 'l'))) {
      outputFilename = std::string(IFN.begin(), IFN.end()-3); // s/.bc/.s/
    }
    else {
        outputFilename = IFN;
    }
    return outputFilename;
}

formatted_raw_ostream *ObjectGenerator::GetOutputStream(const char *TargetName, const char *ProgName) {
    if (OutputFilename != "") {
        if (OutputFilename == "-")
            return &fouts();

        //
        // Make sure that the Out file gets unlinked from the disk if we get a
        // SIGINT
        //
        sys::RemoveFileOnSignal(sys::Path(OutputFilename));

        std::string error;
        raw_fd_ostream *FDOut = new raw_fd_ostream(OutputFilename.c_str(), error, raw_fd_ostream::F_Binary);
        if (! error.empty()) {
            errs() << error << '\n';
            delete FDOut;
            return 0;
        }
        formatted_raw_ostream *Out = new formatted_raw_ostream(*FDOut, formatted_raw_ostream::DELETE_STREAM);

        return Out;
    }

    if (InputFilename == "-") {
        OutputFilename = "-";
        return &fouts();
    }

    OutputFilename = GetFileNameRoot(InputFilename);

    bool Binary = false;
    switch (FileType) {
        default: assert(0 && "Unknown file type");
        case TargetMachine::CGFT_AssemblyFile:
            if (TargetName[0] == 'c') {
                if (TargetName[1] == 0)
                     OutputFilename += ".cbe.c";
                else if (TargetName[1] == 'p' && TargetName[2] == 'p')
                     OutputFilename += ".cpp";
                else OutputFilename += ".s";
            }
            else OutputFilename += ".s";
            break;
        case TargetMachine::CGFT_ObjectFile:
            OutputFilename += ".o";
            Binary = true;
            break;
	    //        case TargetMachine::CGFT_Null:
	    //            OutputFilename += ".null";
	    //            Binary = true;
	    //            break;
    }

    //
    // Make sure that the Out file gets unlinked from the disk if we get a
    // SIGINT
    //
    sys::RemoveFileOnSignal(sys::Path(OutputFilename));

    std::string error;
    unsigned OpenFlags = 0;
    if (Binary) OpenFlags |= raw_fd_ostream::F_Binary;
    raw_fd_ostream *FDOut = new raw_fd_ostream(OutputFilename.c_str(), error, OpenFlags);
    if (!error.empty()) {
        errs() << error << '\n';
        delete FDOut;
        return 0;
    }

    formatted_raw_ostream *Out = new formatted_raw_ostream(*FDOut, formatted_raw_ostream::DELETE_STREAM);

    return Out;
}


int ObjectGenerator::process(Module *module) {
    cerr << "Starting object generation" << endl;
    cerr.flush();
//    if (false) {
        //
        // Initialize targets first, so that --version shows registered targets.
        //
        InitializeAllTargets();
        InitializeAllAsmPrinters();

        //
        // Load the module to be compiled...
        //
        SMDiagnostic Err;
        Module &mod = *module;

        //
        // If we are supposed to override the target triple, do so now.
        //
        if (! TargetTriple.empty())
            mod.setTargetTriple(TargetTriple);

        Triple TheTriple(mod.getTargetTriple());
        if (TheTriple.getTriple().empty())
            TheTriple.setTriple(sys::getHostTriple());

        //
        // Allocate target machine.  First, check whether the user has explicitly
        // specified an architecture to compile for. If so we have to look it up by
        // name, because it might be a backend that has no mapping to a target triple.
        //
        const Target *TheTarget = 0;
        if (! MArch.empty()) {
            for (TargetRegistry::iterator it = TargetRegistry::begin(), ie = TargetRegistry::end(); it != ie; ++it) {
                if (MArch == it -> getName()) {
                    TheTarget = &*it;
                    break;
                }
            }

            if (! TheTarget) {
                errs() << "RoseToLLVM" /*argv[0]*/ << ": error: invalid target '" << MArch << "'.\n";
                return 1;
            }

            //
            // Adjust the triple to match (if known), otherwise stick with the
            // module/host triple.
            //
            Triple::ArchType Type = Triple::getArchTypeForLLVMName(MArch);
            if (Type != Triple::UnknownArch)
                TheTriple.setArch(Type);
        } else {

        std::string Err;
        TheTarget = TargetRegistry::lookupTarget(TheTriple.getTriple(), Err);
        if (TheTarget == 0) {
            errs() << "RoseToLLVM" /*argv[0]*/ << ": error auto-selecting target for module '"
                   << Err << "'.  Please use the -march option to explicitly "
                   << "pick a target.\n";
            return 1;
        }
    }

    //
    // Package up features to be passed to target/subtarget
    //
    std::string FeaturesStr;
    if (MCPU.size() || MAttrs.size()) {
        SubtargetFeatures Features;
        Features.setCPU(MCPU);
        for (unsigned i = 0; i != MAttrs.size(); ++i)
            Features.AddFeature(MAttrs[i]);
        FeaturesStr = Features.getString();
    }

    std::auto_ptr<TargetMachine> target(TheTarget->createTargetMachine(TheTriple.getTriple(), FeaturesStr));
    assert(target.get() && "Could not allocate target machine!");
    TargetMachine &Target = *target.get();

    //
    // Figure out where we are going to send the output...
    //
    formatted_raw_ostream *Out = GetOutputStream(TheTarget -> getName(), "RoseToLLVM"/*argv[0]*/);
    if (Out == 0) return 1;

    CodeGenOpt::Level OLvl = CodeGenOpt::Default;
    switch (OptLevel) {
        default:
cerr << "The optimization level is " << OptLevel << endl;
cerr.flush();
            errs() << "RoseToLLVM" /*argv[0]*/ << ": invalid optimization level.\n";
            return 1;
        case ' ': break;
        case '0': OLvl = CodeGenOpt::None; break;
        case '1': OLvl = CodeGenOpt::Less; break;
        case '2': OLvl = CodeGenOpt::Default; break;
        case '3': OLvl = CodeGenOpt::Aggressive; break;
    }

    //
    // Request that addPassesToEmitFile run the Verifier after running
    // passes which modify the IR.
    //
#ifndef NDEBUG
    bool DisableVerify = false;
#else
    bool DisableVerify = true;
#endif

    //
    // If this target requires addPassesToEmitWholeFile, do it now.  This is
    // used by strange things like the C backend.
    //
    if (Target.WantsWholeFile()) {
        PassManager PM;

        //
        // Add the target data from the target machine, if it exists, or the module.
        //
        if (const TargetData *TD = Target.getTargetData())
             PM.add(new TargetData(*TD));
        else PM.add(new TargetData(&mod));

        if (! NoVerify)
            PM.add(createVerifierPass());

        //
        // Ask the target to add backend passes as necessary.
        //
	//        if (Target.addPassesToEmitWholeFile(PM, *Out, FileType, OLvl, DisableVerify)) {
        if (Target.addPassesToEmitWholeFile(PM, *Out, FileType, OLvl)) {
            errs() << "RoseToLLVM" /*argv[0]*/ << ": target does not support generation of this"
                   << " file type!\n";
            if (Out != &fouts()) delete Out;
            // And the Out file is empty and useless, so remove it now.
            sys::Path(OutputFilename).eraseFromDisk();
            return 1;
        }
        PM.run(mod);
    }
    else {
        //
        // Build up all of the passes that we want to do to the module.
        //
        FunctionPassManager Passes(module);

        //
        // Add the target data from the target machine, if it exists, or the module.
        //
        if (const TargetData *TD = Target.getTargetData())
             Passes.add(new TargetData(*TD));
        else Passes.add(new TargetData(&mod));

#ifndef NDEBUG
        if (! NoVerify)
            Passes.add(createVerifierPass());
#endif

        //
        // Override default to generate verbose assembly.
        //
        Target.setAsmVerbosityDefault(true);

cerr << "The file type is " << FileType << endl;
cerr.flush();
//        if (Target.addPassesToEmitFile(Passes, *Out, FileType, OLvl, DisableVerify)) {
        if (Target.addPassesToEmitFile(Passes, *Out, FileType, OLvl)) {
            errs() << "RoseToLLVM" /*argv[0]*/ << ": target does not support generation of this" << " file type!\n";
            if (Out != &fouts()) delete Out;
            // And the Out file is empty and useless, so remove it now.
            sys::Path(OutputFilename).eraseFromDisk();
            return 1;
        }

        Passes.doInitialization();

        //
        // Run our queue of passes all at once now, efficiently.
        // TODO: this could lazily stream functions out of the module.
        //
        for (Module::iterator I = mod.begin(), E = mod.end(); I != E; ++I)
            if (! I -> isDeclaration()) {
                if (DisableRedZone)
                    I -> addFnAttr(Attribute::NoRedZone);
                if (NoImplicitFloats)
                    I -> addFnAttr(Attribute::NoImplicitFloat);
                Passes.run(*I);
            }

            Passes.doFinalization();
        }

        //
        // Delete the ostream if it's not a stdout stream
        //
        if (Out != &fouts()) delete Out;
//    }
    cerr << "Done with object generation" << endl;
    cerr.flush();
}
