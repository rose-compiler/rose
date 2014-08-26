#include <rose.h>
#include <rosePublicConfig.h>
#include <AsmFunctionIndex.h>
#include <AsmUnparser.h>
#include <BinaryControlFlow.h>
#include <BinaryLoader.h>
#include <DisassemblerArm.h>
#include <DisassemblerPowerpc.h>
#include <DisassemblerMips.h>
#include <DisassemblerX86.h>
#include <DisassemblerM68k.h>
#include <Partitioner2/ModulesElf.h>
#include <Partitioner2/ModulesM68k.h>
#include <Partitioner2/ModulesPe.h>
#include <Partitioner2/ModulesX86.h>
#include <Partitioner2/Partitioner.h>
#include <Partitioner2/Utility.h>

#include <sawyer/Assert.h>
#include <sawyer/CommandLine.h>
#include <sawyer/ProgressBar.h>

#if 1 // DEBUGGING [Robb P. Matzke 2014-08-24]
#include "AsmUnparser_compat.h"
#endif


// FIXME[Robb P. Matzke 2014-08-24]: These matchers still need to be implemented:
/* 
 | name                   | purpose                                                   |
 |------------------------+-----------------------------------------------------------|
 | mark_call_insns        | find CALL insns and make functions at their target        |
 | FindFunctionFragments  | uses code criteria to make code from data                 |
 | FindThunks             | makes functions from BB's containing only a JMP           |
 | FindInterpadFunctions  | find functions between other functions                    |
 | FindThunkTables        | find long sequences of JMP instructions                   |
 | FindPostFunctionInsns  |                                                           |
 */

using namespace rose;
using namespace rose::BinaryAnalysis;
using namespace rose::Diagnostics;

namespace P2 = Partitioner2;

static Disassembler *
getDisassembler(const std::string &name)
{
    if (0==name.compare("list")) {
        std::cout <<"The following ISAs are supported:\n"
                  <<"  amd64\n"
                  <<"  arm\n"
                  <<"  coldfire\n"
                  <<"  i386\n"
                  <<"  m68040\n"
                  <<"  mips\n"
                  <<"  ppc\n";
        exit(0);
    } else if (0==name.compare("arm")) {
        return new DisassemblerArm();
    } else if (0==name.compare("ppc")) {
        return new DisassemblerPowerpc();
    } else if (0==name.compare("mips")) {
        return new DisassemblerMips();
    } else if (0==name.compare("i386")) {
        return new DisassemblerX86(4);
    } else if (0==name.compare("amd64")) {
        return new DisassemblerX86(8);
    } else if (0==name.compare("m68040")) {
        return new DisassemblerM68k(m68k_68040);
    } else if (0==name.compare("coldfire")) {
        return new DisassemblerM68k(m68k_freescale_emacb);
    } else {
        throw std::runtime_error("invalid ISA name \""+name+"\"; use --isa=list");
    }
}

static const rose_addr_t NO_ADDRESS(-1);

// Convenient struct to hold settings from the command-line all in one place.
struct Settings {
    std::string isaName;                                // instruction set architecture name
    rose_addr_t mapVa;                                  // where to map the specimen in virtual memory
    bool useSemantics;                                  // should we use symbolic semantics?
    bool followGhostEdges;                              // do we ignore opaque predicates?
    bool allowDiscontiguousBlocks;                      // can basic blocks be discontiguous in memory?
    bool findSwitchCases;                               // search for C-like "switch" statement cases
    bool findDeadCode;                                  // do we look for unreachable basic blocks?
    bool intraFunctionData;                             // suck up unused addresses as intra-function data
    bool doListCfg;                                     // list the control flow graph
    bool doListAum;                                     // list the address usage map
    bool doListAsm;                                     // produce an assembly-like listing with AsmUnparser
    bool doListFunctions;                               // produce a function index
    bool doListFunctionAddresses;                       // list function entry addresses
    bool doShowStats;                                   // show some statistics
    bool doListUnused;                                  // list unused addresses
    Settings()
        : mapVa(NO_ADDRESS), useSemantics(true), followGhostEdges(false), allowDiscontiguousBlocks(true),
          findSwitchCases(true), findDeadCode(true), intraFunctionData(true), doListCfg(false), doListAum(false),
          doListAsm(true), doListFunctions(false), doListFunctionAddresses(false), doShowStats(false), doListUnused(false) {}
};

// Describe and parse the command-line
static Sawyer::CommandLine::ParserResult
parseCommandLine(int argc, char *argv[], Settings &settings)
{
    using namespace Sawyer::CommandLine;

    // Generic switches
    SwitchGroup gen;
    gen.doc("General switches:");
    gen.insert(Switch("help", 'h')
               .doc("Show this documentation.")
               .action(showHelpAndExit(0)));
    gen.insert(Switch("log", 'L')
               .action(configureDiagnostics("log", Sawyer::Message::mfacilities))
               .argument("config")
               .whichValue(SAVE_ALL)
               .doc("Configures diagnostics.  Use \"@s{log}=help\" and \"@s{log}=list\" to get started."));
    gen.insert(Switch("version", 'V')
               .action(showVersionAndExit(version_message(), 0))
               .doc("Shows version information for various ROSE components and then exits."));
    gen.insert(Switch("use-semantics")
               .intrinsicValue(true, settings.useSemantics)
               .doc("The partitioner can either use quick and naive methods of determining instruction characteristics, or "
                    "it can use slower but more accurate methods, such as symbolic semantics.  This switch enables use of "
                    "the slower symbolic semantics, or the feature can be disabled with @s{no-use-semantics}. The default is " +
                    std::string(settings.useSemantics?"true":"false") + "."));
    gen.insert(Switch("no-use-semantics")
               .key("use-semantics")
               .intrinsicValue(false, settings.useSemantics)
               .hidden(true));

    // Switches for disassembly
    SwitchGroup dis;
    dis.doc("Switches for disassembly:");
    dis.insert(Switch("isa")
               .argument("architecture", anyParser(settings.isaName))
               .doc("Instruction set architecture. Specify \"list\" to see a list of possible ISAs."));
    dis.insert(Switch("map")
               .argument("virtual-address", nonNegativeIntegerParser(settings.mapVa))
               .doc("If this switch is present, then the specimen is treated as raw data and mapped in its entirety "
                    "into the address space beginning at the address specified for this switch. Otherwise the file "
                    "is interpreted as an ELF or PE container."));
    dis.insert(Switch("allow-discontiguous-blocks")
               .intrinsicValue(true, settings.allowDiscontiguousBlocks)
               .doc("This setting allows basic blocks to contain instructions that are discontiguous in memory as long as "
                    "the other requirements for a basic block are still met. Discontiguous blocks can be formed when a "
                    "compiler fails to optimize away an opaque predicate for a conditional branch, or when basic blocks "
                    "are scattered in memory by the introduction of unconditional jumps.  The @s{no-allow-discontiguous-blocks} "
                    "switch disables this feature and can slightly improve partitioner performance by avoiding cases where "
                    "an unconditional branch initially creates a larger basic block which is later discovered to be "
                    "multiple blocks.  The default is to " + std::string(settings.allowDiscontiguousBlocks?"":"not ") +
                    "allow discontiguous basic blocks."));
    dis.insert(Switch("no-allow-discontiguous-blocks")
               .key("allow-discontiguous-blocks")
               .intrinsicValue(false, settings.allowDiscontiguousBlocks)
               .hidden(true));
    dis.insert(Switch("follow-ghost-edges")
               .intrinsicValue(true, settings.followGhostEdges)
               .doc("When discovering the instructions for a basic block, treat instructions individually rather than "
                    "looking for opaque predicates.  The @s{no-follow-ghost-edges} switch turns this off.  The default "
                    "is " + std::string(settings.followGhostEdges?"true":"false") + "."));
    dis.insert(Switch("no-follow-ghost-edges")
               .key("follow-ghost-edges")
               .intrinsicValue(false, settings.followGhostEdges)
               .hidden(true));
    dis.insert(Switch("find-dead-code")
               .intrinsicValue(true, settings.findDeadCode)
               .doc("Use ghost edges (non-followed control flow from branches with opaque predicates) to locate addresses "
                    "for unreachable code, then recursively discover basic blocks at those addresses and add them to the "
                    "same function.  The @s{no-find-dead-code} switch turns this off.  The default is " +
                    std::string(settings.findDeadCode?"true":"false") + "."));
    dis.insert(Switch("no-find-dead-code")
               .key("find-dead-code")
               .intrinsicValue(false, settings.findDeadCode)
               .hidden(true));
    dis.insert(Switch("find-switch-cases")
               .intrinsicValue(true, settings.findSwitchCases)
               .doc("Scan for common encodings of C-like \"switch\" statements so that the cases can be disassembled. The "
                    "@s{no-find-switch-cases} switch turns this off.  The default is " +
                    std::string(settings.findSwitchCases?"true":"false") + "."));
    dis.insert(Switch("no-find-switch-cases")
               .key("find-switch-cases")
               .intrinsicValue(false, settings.findSwitchCases)
               .hidden(true));
    dis.insert(Switch("intra-function-data")
               .intrinsicValue(true, settings.intraFunctionData)
               .doc("Near the end of processing, if there are regions of unused memory that are immediately preceded and "
                    "followed by the same function then add that region of memory to that function as a static data block."
                    "The @s{no-intra-function-data} switch turns this feature off.  The default is " +
                    std::string(settings.intraFunctionData?"true":"false") + "."));
    dis.insert(Switch("no-intra-function-data")
               .key("intra-function-data")
               .intrinsicValue(false, settings.intraFunctionData)
               .hidden(true));

    // Switches for output
    SwitchGroup out;
    out.doc("Switches that affect output:");
    out.insert(Switch("list-asm")
               .intrinsicValue(true, settings.doListAsm)
               .doc("Produce an assembly listing.  This is the default; it can be turned off with @s{no-list-asm}."));
    out.insert(Switch("no-list-asm")
               .key("list-asm")
               .intrinsicValue(false, settings.doListAsm)
               .hidden(true));

    out.insert(Switch("list-aum")
               .intrinsicValue(true, settings.doListAum)
               .doc("Emit a listing of the address usage map after the CFG is discovered.  The @s{no-list-aum} switch is "
                    "the inverse."));
    out.insert(Switch("no-list-aum")
               .key("list-aum")
               .intrinsicValue(false, settings.doListAum)
               .hidden(true));

    out.insert(Switch("list-cfg")
               .intrinsicValue(true, settings.doListCfg)
               .doc("Emit a listing of the CFG after it is discovered."));
    out.insert(Switch("no-list-cfg")
               .key("list-cfg")
               .intrinsicValue(false, settings.doListCfg)
               .hidden(true));

    out.insert(Switch("list-functions")
               .intrinsicValue(true, settings.doListFunctions)
               .doc("Produce a table of contents showing all the functions that were detected.  The @s{no-list-functions} "
                    "switch disables this.  The default is to " + std::string(settings.doListFunctions?"":"not ") +
                    "show this information."));
    out.insert(Switch("no-list-functions")
               .key("list-functions")
               .intrinsicValue(false, settings.doListFunctions)
               .hidden(true));

    out.insert(Switch("list-function-entries")
               .intrinsicValue(true, settings.doListFunctionAddresses)
               .doc("Produce a listing of function entry addresses, one address per line in hexadecimal format. Each address "
                    "is followed by the word \"existing\" or \"missing\" depending on whether a non-empty basic block exists "
                    "in the CFG for the function entry address.  The listing is disabled with @s{no-list-function-entries}."));
    out.insert(Switch("no-list-function-entries")
               .key("list-function-entries")
               .intrinsicValue(false, settings.doListFunctionAddresses)
               .hidden(true));

    out.insert(Switch("list-unused")
               .intrinsicValue(true, settings.doListUnused)
               .doc("Produce a listing of all specimen addresses that are not represented in the control flow graph. This "
                    "listing can be disabled with @s{no-list-unused}."));
    out.insert(Switch("no-list-unused")
               .key("list-unused")
               .intrinsicValue(false, settings.doListUnused)
               .hidden(true));

    out.insert(Switch("show-stats")
               .intrinsicValue(true, settings.doShowStats)
               .doc("Emit some information about how much of the input was disassembled."));
    out.insert(Switch("no-show-stats")
               .key("show-stats")
               .intrinsicValue(false, settings.doShowStats)
               .hidden(true));
    

    Parser parser;
    parser
        .purpose("tests new partitioner architecture")
        .doc("synopsis",
             "@prop{programName} [@v{switches}] @v{specimen_name}")
        .doc("description",
             "This program tests the new partitioner architecture by disassembling the specified raw file.");
    
    return parser.with(gen).with(dis).with(out).parse(argc, argv).apply();
}

// Example of watching CFG changes.  Sort of stupid, but fun to watch.  A more useful monitor might do things like adjust
// work-lists that are defined by the user.  In any case, a CFG monitor is a good place to track progress if you need to do
// that.
class Monitor: public P2::CfgAdjustmentCallback {
public:
    static Ptr instance() { return Ptr(new Monitor); }
    virtual bool operator()(bool chain, const AttachedBasicBlock &args) {
        std::cerr <<"+";
        if (args.bblock)
            std::cerr <<std::string(args.bblock->nInstructions(), '.');
        return chain;
    }
    virtual bool operator()(bool chain, const DetachedBasicBlock &args) {
        std::cerr <<"-";
        if (args.bblock)
            std::cerr <<"-" <<std::string(args.bblock->nInstructions(), '.');
        return chain;
    }
};

// Example of making adjustments to basic block successors.  If this callback is registered with the partitioner, then it will
// add ghost edges (non-taken side of branches with opaque predicates) to basic blocks as their instructions are discovered.
// An alternative method is to investigate ghost edges after functions are discovered in order to find dead code.
class AddGhostSuccessors: public P2::BasicBlockCallback {
public:
    static Ptr instance() { return Ptr(new AddGhostSuccessors); }

    virtual bool operator()(bool chain, const Args &args) {
        if (chain) {
            size_t nBits = args.partitioner->instructionProvider().instructionPointerRegister().get_nbits();
            BOOST_FOREACH (rose_addr_t successorVa, args.partitioner->basicBlockGhostSuccessors(args.bblock)) {
                args.bblock->insertSuccessor(successorVa, nBits);
                P2::mlog[INFO] <<"opaque predicate at "
                               <<StringUtility::addrToString(args.bblock->instructions().back()->get_address())
                               <<": branch " <<StringUtility::addrToString(successorVa) <<" never taken\n";
            }
        }
        return chain;
    }
};

// Example successor callback that forces basic blocks to terminate at unconditional branch instructions whose target is not
// the fall-through address of the branch.
class PreventDiscontiguousBlocks: public P2::BasicBlockCallback {
public:
    static Ptr instance() { return Ptr(new PreventDiscontiguousBlocks); }

    virtual bool operator()(bool chain, const Args &args) {
        if (chain) {
            bool complete;
            std::vector<rose_addr_t> successors = args.partitioner->basicBlockConcreteSuccessors(args.bblock, &complete);
            if (complete && 1==successors.size() && successors[0]!=args.bblock->fallthroughVa())
                args.results.terminate = TERMINATE_NOW;
        }
        return chain;
    }
};

// Matches x86 "MOV EDI, EDI; PUSH ESI" as a function prologue
class X86AbbreviatedPrologue: public P2::FunctionPrologueMatcher {
protected:
    P2::Function::Ptr function_;
public:
    static Ptr instance() { return Ptr(new X86AbbreviatedPrologue); }
    virtual P2::Function::Ptr function() const /*override*/ { return function_; }
    virtual bool match(const P2::Partitioner *partitioner, rose_addr_t anchor) /*override*/ {
        SgAsmx86Instruction *insn = NULL;
        // Look for MOV EDI, EDI
        {
            static const RegisterDescriptor REG_EDI(x86_regclass_gpr, x86_gpr_di, 0, 32);
            rose_addr_t moveVa = anchor;
            if (partitioner->instructionExists(moveVa))
                return false;                               // already in the CFG/AUM
            insn = isSgAsmx86Instruction(partitioner->discoverInstruction(moveVa));
            if (!insn || insn->get_kind()!=x86_mov)
                return false;
            const SgAsmExpressionPtrList &opands = insn->get_operandList()->get_operands();
            if (opands.size()!=2)
                return false;
            SgAsmDirectRegisterExpression *dst = isSgAsmDirectRegisterExpression(opands[0]);
            if (!dst || dst->get_descriptor()!=REG_EDI)
                return false;
            SgAsmDirectRegisterExpression *src = isSgAsmDirectRegisterExpression(opands[1]);
            if (!src || dst->get_descriptor()!=src->get_descriptor())
                return false;
        }

        // Look for PUSH ESI
        {
            static const RegisterDescriptor REG_ESI(x86_regclass_gpr, x86_gpr_si, 0, 32);
            rose_addr_t pushVa = insn->get_address() + insn->get_size();
            insn = isSgAsmx86Instruction(partitioner->discoverInstruction(pushVa));
            if (partitioner->instructionExists(pushVa))
                return false;                               // already in the CFG/AUM
            if (!insn || insn->get_kind()!=x86_push)
                return false;
            const SgAsmExpressionPtrList &opands = insn->get_operandList()->get_operands();
            if (opands.size()!=1)
                return false;                               // crazy operands!
            SgAsmRegisterReferenceExpression *rre = isSgAsmRegisterReferenceExpression(opands[0]);
            if (!rre || rre->get_descriptor()!=REG_ESI)
                return false;
        }

        // Seems good!
        function_ = P2::Function::instance(anchor);
        return true;
    }
};

// Make functions at specimen entry addresses
static void
markEntryFunctions(P2::Partitioner &partitioner, SgAsmInterpretation *interp) {
    if (interp) {
        BOOST_FOREACH (SgAsmGenericHeader *fileHeader, interp->get_headers()->get_headers()) {
            BOOST_FOREACH (const rose_rva_t &rva, fileHeader->get_entry_rvas()) {
                rose_addr_t va = rva.get_rva() + fileHeader->get_base_va();
                partitioner.attachOrMergeFunction(P2::Function::instance(va));
            }
        }
    }
}

// Make functions at error handling addresses
static void
markErrorHandlingFunctions(P2::Partitioner &partitioner, SgAsmInterpretation *interp) {
    if (interp) {
        BOOST_FOREACH (SgAsmGenericHeader *fileHeader, interp->get_headers()->get_headers()) {
            if (SgAsmElfFileHeader *elfHeader = isSgAsmElfFileHeader(fileHeader)) {
                std::vector<P2::Function::Ptr> functions = P2::ModulesElf::findErrorHandlingFunctions(elfHeader);
                BOOST_FOREACH (const P2::Function::Ptr &function, functions)
                    partitioner.attachOrMergeFunction(function);
            }
        }
    }
}

// Make functions at import trampolines
static void
markImportFunctions(P2::Partitioner &partitioner, SgAsmInterpretation *interp) {
    // Windows PE imports
    P2::ModulesPe::rebaseImportAddressTables(partitioner, P2::ModulesPe::getImportIndex(interp));
    BOOST_FOREACH (const P2::Function::Ptr &function, P2::ModulesPe::findImportFunctions(partitioner, interp))
        partitioner.attachOrMergeFunction(function);
    
    // ELF imports
    if (interp) {
        BOOST_FOREACH (SgAsmGenericHeader *fileHeader, interp->get_headers()->get_headers()) {
            if (SgAsmElfFileHeader *elfHeader = isSgAsmElfFileHeader(fileHeader)) {
                std::vector<P2::Function::Ptr> functions = P2::ModulesElf::findPltFunctions(partitioner, elfHeader);
                BOOST_FOREACH (const P2::Function::Ptr &function, functions)
                    partitioner.attachOrMergeFunction(function);
            }
        }
    }
}

// Make functions at export addresses
static void
markExportFunctions(P2::Partitioner &partitioner, SgAsmInterpretation *interp) {
    if (interp) {
        BOOST_FOREACH (SgAsmGenericHeader *fileHeader, interp->get_headers()->get_headers()) {
            if (SgAsmPEFileHeader *peHeader = isSgAsmPEFileHeader(fileHeader)) {
                std::vector<P2::Function::Ptr> functions = P2::ModulesPe::findExportFunctions(partitioner, peHeader);
                BOOST_FOREACH (const P2::Function::Ptr &function, functions)
                    partitioner.attachOrMergeFunction(function);
            }
        }
    }
}

// Make functions that have symbols
static void
markSymbolFunctions(P2::Partitioner &partitioner, SgAsmInterpretation *interp) {
    if (interp) {
        BOOST_FOREACH (SgAsmGenericHeader *fileHeader, interp->get_headers()->get_headers()) {
            if (SgAsmElfFileHeader *elfHeader = isSgAsmElfFileHeader(fileHeader)) {
                std::vector<P2::Function::Ptr> functions = P2::Modules::findSymbolFunctions(partitioner, elfHeader);
                BOOST_FOREACH (const P2::Function::Ptr &function, functions)
                    partitioner.attachOrMergeFunction(function);
            }
        }
    }
}

// Make functions for any x86 CALL instruction. This is intended to be a demonstration of how to search for specific
// instruction patterns, and probably isn't all that useful since function calls in reachable code are already detected anyway
// as part of Partitioner::discoverBasicBlock.
static void
markCallTargets(P2::Partitioner &partitioner, size_t alignment=1) {
    std::set<rose_addr_t> targets;                      // distinct call targets

    // Iterate over every executable address in the memory map
    for (rose_addr_t va=0; partitioner.memoryMap().next(va, MemoryMap::MM_PROT_EXEC).assignTo(va); ++va) {
        if (alignment>1)                                // apply alignment here as an optimization
            va = ((va+alignment-1)/alignment)*alignment;

        // Disassemble an instruction (or get one that was previously disassembled) and see if it's a function call. The
        // function call detection normally operates at a basic block level, so we make a singleton basic block since we're
        // interested only in single instructions.
        if (SgAsmInstruction *insn = partitioner.discoverInstruction(va)) {
            std::vector<SgAsmInstruction*> bb(1, insn);
            rose_addr_t target(-1);
            if (insn->is_function_call(bb, &target, NULL) && partitioner.memoryMap().exists(target, MemoryMap::MM_PROT_EXEC))
                targets.insert(target);
        }
    }

    // Now create functions at each target address
    BOOST_FOREACH (rose_addr_t entryVa, targets) {
        P2::Function::Ptr function = P2::Function::instance(entryVa, SgAsmFunction::FUNC_CALL_INSN);
        partitioner.attachOrMergeFunction(function);
    }
}

// Looks for one basic block that hasn't been discovered and tries to find instructions for it.  Returns true if a basic block
// was processed, false if not.
static bool
findBasicBlock(P2::Partitioner &partitioner) {
    P2::ControlFlowGraph::VertexNodeIterator worklist = partitioner.undiscoveredVertex();
    if (worklist->nInEdges() > 0) {
        P2::ControlFlowGraph::VertexNodeIterator placeholder = worklist->inEdges().begin()->source();
        P2::mlog[WHERE] <<"\n  processing block " <<partitioner.vertexName(*placeholder) <<"\n";
        P2::BasicBlock::Ptr bb = partitioner.discoverBasicBlock(placeholder);
        partitioner.attachBasicBlock(placeholder, bb);
        return true;
    }
    return false;
}

// Looks for a function prologue at or above the specified starting address.  If one is found, then create a new function, add
// it to the partitioner, increment the starting address, and return true.  Otherwise do nothing and return false.
static bool
findFunctionPrologue(P2::Partitioner &partitioner, rose_addr_t &startVa /*in,out*/) {
    if (P2::Function::Ptr function = partitioner.nextFunctionPrologue(startVa)) {
        P2::mlog[WHERE] <<"\nFound prologue for " <<partitioner.functionName(function) <<"\n";
        startVa = function->address() + 1;              // advance the search point past the start of the function
        partitioner.attachFunction(function);           // also adds the function entry address to our work list
        return true;
    }
    return false;
}

// Tries to assign basic blocks to functions.  Returns the number of functions that were successfully processed.  Get a list of
// functions (those we already found, and those created due to function call edges), and discover basic blocks for each. Attach
// the functions and their blocks to the CFG.  The discoverFunctionEntryVertices returns a list of functions, some of which are
// already attached to the CFG/AUM and some which are detached.  Before we add new basic blocks to a function we must detach it
// from the CFG/AUM.
static size_t
findFunctionBlocks(P2::Partitioner &partitioner) {
    size_t nProcessed = 0;
    std::vector<P2::Function::Ptr> functions = partitioner.discoverFunctionEntryVertices();
    BOOST_FOREACH (const P2::Function::Ptr &function, functions)
        partitioner.attachFunction(function);
    BOOST_FOREACH (const P2::Function::Ptr &function, functions) {
        P2::mlog[WHERE] <<"Discovering blocks for " <<partitioner.functionName(function) <<"\n";
        partitioner.detachFunction(function);
        if (0==partitioner.discoverFunctionBasicBlocks(function, NULL, NULL))
            ++nProcessed;
        partitioner.attachFunction(function);
    }
    return nProcessed;
}

static size_t
findDeadCode(P2::Partitioner &partitioner, const P2::Function::Ptr &function) {
    ASSERT_not_null(function);
    size_t nProcessed = 0;
    while (1) {
        std::set<rose_addr_t> ghosts = partitioner.functionGhostSuccessors(function);
        if (ghosts.empty())
            break;
        partitioner.detachFunction(function);           // so we can modify its basic block ownership list
        BOOST_FOREACH (rose_addr_t ghost, ghosts) {
            P2::mlog[INFO] <<partitioner.functionName(function) <<" has dead code at "
                           <<StringUtility::addrToString(ghost) <<"\n";
            partitioner.insertPlaceholder(ghost);       // ensure a basic block gets created here
            function->insertBasicBlock(ghost);          // the function will own this basic block
        }
        while (findBasicBlock(partitioner))/*void*/;    // discover basic blocks recursively
        if (partitioner.discoverFunctionBasicBlocks(function, NULL, NULL)) {
            P2::mlog[WARN] <<"cannot create " <<partitioner.functionName(function) <<"\n";
            break;
        }
        partitioner.attachFunction(function);           // reattach the function to the CFG/AUM
        ++nProcessed;
    }
    return nProcessed;
}

// Finds dead code and adds it to the function to which it seems to belong.
static size_t
findDeadCode(P2::Partitioner &partitioner) {
    size_t nProcessed = 0;
    BOOST_FOREACH (const P2::Function::Ptr &function, partitioner.functions())
        nProcessed += findDeadCode(partitioner, function);
    return nProcessed;
}

// Find padding that appears before the entry address of a function that aligns the entry address on a 4-byte boundary.
// For m68k, padding is either 2-byte TRAPF instructions (0x51 0xfc) or zero bytes.  Patterns we've seen are 51 fc, 51 fc 00
// 51 fc, 00 00, 51 fc 51 fc, but we'll allow any combination.
static size_t
findFunctionPadding(P2::Partitioner &partitioner) {
    size_t nProcessed = 0;
    BOOST_FOREACH (const P2::Function::Ptr &function, partitioner.functions()) {
        rose_addr_t entryVa = function->address();
        rose_addr_t paddingVa = entryVa;
        uint8_t buf[2];
        size_t nread;
        while ((nread=std::min(paddingVa, (rose_addr_t)2))>0 &&
               (nread=partitioner.memoryMap().readBackward(buf, paddingVa, nread, MemoryMap::MM_PROT_EXEC))) {
            if (2==nread && ((0==buf[0] && 0==buf[1]) || (0x51==buf[0] && 0xfc==buf[1]))) {
                paddingVa -= 2;
            } else if ((2==nread && 0==buf[1]) || (1==nread && 0==buf[0])) {
                paddingVa -= 1;
            } else {
                break;
            }
        }
        if (paddingVa < entryVa) {
            partitioner.attachFunctionDataBlock(function, paddingVa, entryVa-paddingVa);
            ++nProcessed;
        }
    }
    return nProcessed;
}

// Finds unused areas that are surrounded by a function and adds them as static data to the function.
static size_t
findIntraFunctionData(P2::Partitioner &partitioner, const AddressIntervalSet &executableSpace)
{
    size_t nProcessed = 0;
    Sawyer::Container::IntervalSet<AddressInterval> unused = partitioner.aum().unusedExtent(executableSpace);
    BOOST_FOREACH (const AddressInterval &interval, unused.nodes()) {
        if (interval.least()<=executableSpace.least() || interval.greatest()>=executableSpace.greatest())
            continue;
        typedef std::vector<P2::Function::Ptr> Functions;
        Functions beforeFuncs = partitioner.functionsOverlapping(interval.least()-1);
        Functions afterFuncs = partitioner.functionsOverlapping(interval.greatest()+1);

        // What functions are in both sets?
        Functions enclosingFuncs(beforeFuncs.size());
        Functions::iterator final = std::set_intersection(beforeFuncs.begin(), beforeFuncs.end(),
                                                          afterFuncs.begin(), afterFuncs.end(), enclosingFuncs.begin());
        enclosingFuncs.resize(final-enclosingFuncs.begin());

        // Add the data block to all enclosing functions
        if (!enclosingFuncs.empty()) {
            BOOST_FOREACH (const P2::Function::Ptr &function, enclosingFuncs) {
                P2::mlog[INFO] <<partitioner.functionName(function) <<" has "
                               <<StringUtility::plural(interval.size(), "bytes") <<" of static data at "
                               <<StringUtility::addrToString(interval.least()) <<"\n";
                partitioner.attachFunctionDataBlock(function, interval.least(), interval.size());
            }
            ++nProcessed;
        }
    }
    return nProcessed;
}

int main(int argc, char *argv[]) {
    // Do this explicitly since librose doesn't do this automatically yet
    Diagnostics::initialize();

#if 0 // DEBUGGING [Robb P. Matzke 2014-08-16]: make progress reporting more fluid than normal
    Sawyer::ProgressBarSettings::initialDelay(0.5);
    Sawyer::ProgressBarSettings::minimumUpdateInterval(0.05);
#endif

    // Parse the command-line
    Settings settings;
    Sawyer::CommandLine::ParserResult cmdline = parseCommandLine(argc, argv, settings);
    std::vector<std::string> positionalArgs = cmdline.unreachedArgs();
    Disassembler *disassembler = NULL;
    if (!settings.isaName.empty())
        disassembler = getDisassembler(settings.isaName);// do this before we check for positional arguments (for --isa=list)
    if (positionalArgs.empty())
        throw std::runtime_error("no file name specified; see --help");
    if (positionalArgs.size()>1)
        throw std::runtime_error("too many files specified; see --help");
    std::string specimenName = positionalArgs[0];

    // Load the specimen as raw data or an ELF or PE container
    SgAsmInterpretation *interp = NULL;
    MemoryMap map;
    if (settings.mapVa!=NO_ADDRESS) {
        if (!disassembler)
            throw std::runtime_error("an instruction set architecture must be specified with the \"--isa\" switch");
        size_t nBytesMapped = map.insert_file(specimenName, settings.mapVa);
        if (0==nBytesMapped)
            throw std::runtime_error("problem reading file: " + specimenName);
        map.mprotect(AddressInterval::baseSize(settings.mapVa, nBytesMapped), MemoryMap::MM_PROT_RX);
    } else {
        std::vector<std::string> args;
        args.push_back(argv[0]);
        args.push_back("-rose:binary");
        args.push_back("-rose:read_executable_file_format_only");
        args.push_back(specimenName);
        SgProject *project = frontend(args);
        std::vector<SgAsmInterpretation*> interps = SageInterface::querySubTree<SgAsmInterpretation>(project);
        if (interps.empty())
            throw std::runtime_error("a binary specimen container must have at least one SgAsmInterpretation");
        interp = interps.back();    // windows PE is always after DOS
        BinaryLoader *loader = BinaryLoader::lookup(interp)->clone();
        loader->remap(interp);
        ASSERT_not_null(interp->get_map());
        map = *interp->get_map();
        if (!disassembler && !(disassembler = Disassembler::lookup(interp)))
            throw std::runtime_error("an instruction set architecture could not be discerned");
    }
    disassembler->set_progress_reporting(-1.0);         // turn it off
    size_t wordSize = disassembler->instructionPointerRegister().get_nbits();

    // Some analyses need to know what part of the address space is being disassembled.
    AddressIntervalSet executableSpace;
    BOOST_FOREACH (const MemoryMap::Segments::Node &node, map.segments().nodes()) {
        if ((node.value().get_mapperms() & MemoryMap::MM_PROT_EXEC)!=0)
            executableSpace.insert(node.key());
    }

    // Create the partitioner
    P2::Partitioner partitioner(disassembler, map);
    partitioner.enableSymbolicSemantics(settings.useSemantics);
    partitioner.functionPrologueMatchers().push_back(P2::ModulesX86::MatchHotPatchPrologue::instance());
    partitioner.functionPrologueMatchers().push_back(P2::ModulesX86::MatchStandardPrologue::instance());
    partitioner.functionPrologueMatchers().push_back(X86AbbreviatedPrologue::instance());
    partitioner.functionPrologueMatchers().push_back(P2::ModulesM68k::MatchLink::instance());
    partitioner.functionPrologueMatchers().push_back(P2::ModulesX86::MatchEnterPrologue::instance());
    if (settings.findSwitchCases)
        partitioner.basicBlockCallbacks().append(P2::ModulesM68k::SwitchSuccessors::instance());
    if (settings.followGhostEdges)
        partitioner.basicBlockCallbacks().append(AddGhostSuccessors::instance());
    if (!settings.allowDiscontiguousBlocks)
        partitioner.basicBlockCallbacks().append(PreventDiscontiguousBlocks::instance());
#if 0 // [Robb P. Matzke 2014-08-16]: fun to watch, but verbose!
    partitioner.cfgAdjustmentCallbacks().append(Monitor::instance());
#endif


    markEntryFunctions(partitioner, interp);            // make functions at program entry points
    markErrorHandlingFunctions(partitioner, interp);    // make functions based on exception handling information
    markImportFunctions(partitioner, interp);           // make functions for things that are imported
    markExportFunctions(partitioner, interp);           // make functions for things that are exported
    markSymbolFunctions(partitioner, interp);           // make functions for symbols in the symbol table
    //markCallTargets(partitioner);                     // make functions at all CALL targets
    rose_addr_t prologueVa = 0;
    partitioner.memoryMap().dump(std::cerr);
    while (findBasicBlock(partitioner) || findFunctionPrologue(partitioner, prologueVa)) /*void*/;
    findFunctionBlocks(partitioner);                    // organize existing basic blocks into functions
    if (settings.findDeadCode)
        findDeadCode(partitioner);                      // find unreachable code and add it to functions
    findFunctionPadding(partitioner);                   // find function alignment padding before entry points
    if (settings.intraFunctionData)
        findIntraFunctionData(partitioner, executableSpace); // find data areas that are enclosed by functions
    

    // Perform a final pass over all functions and issue reports about which functions have unreasonable control flow.
    BOOST_FOREACH (const P2::Function::Ptr &function, partitioner.functions()) {
        P2::EdgeList inwardConflictEdges, outwardConflictEdges;
        partitioner.detachFunction(function);           // temporarily detach so we can call discoverFunctionBasicBlocks
        if (0==partitioner.discoverFunctionBasicBlocks(function, &inwardConflictEdges, &outwardConflictEdges)) {
            partitioner.attachFunction(function);
        } else {
            P2::mlog[WARN] <<"discovery for " <<partitioner.functionName(function)
                           <<" had " <<StringUtility::plural(inwardConflictEdges.size(), "inward conflicts")
                           <<" and " <<StringUtility::plural(outwardConflictEdges.size(), "outward conflicts") <<"\n";
            BOOST_FOREACH (const P2::ControlFlowGraph::EdgeNodeIterator &edge, inwardConflictEdges) {
                using namespace P2;                     // to pick up operator<< for *edge
                P2::mlog[WARN] <<"  inward conflict " <<*edge
                               <<" from " <<partitioner.functionName(edge->source()->value().function()) <<"\n";
            }
            BOOST_FOREACH (const P2::ControlFlowGraph::EdgeNodeIterator &edge, outwardConflictEdges) {
                using namespace P2;                     // to pick up operator<< for *edge
                P2::mlog[WARN] <<"  outward conflict " <<*edge
                               <<" to " <<partitioner.functionName(edge->target()->value().function()) <<"\n";
            }
#if 0 // [Robb P. Matzke 2014-08-13]
            // Forcibly insert the target vertex for inward-conflicting edges and try again.
            BOOST_FOREACH (P2::ControlFlowGraph::EdgeNodeIterator &edge, inwardConflictEdges)
                function->insertBasicBlock(edge->target()->value().address());
            if (0==partitioner.discoverFunctionBasicBlocks(function, NULL, NULL)) {
                P2::mlog[WARN] <<"  conflicts have been overridden\n";
                partitioner.insertFunction(function);
            }
#endif
        }
    }

    // Now that the partitioner's work is all done, try to give names to some things.
    if (interp)
        P2::ModulesPe::nameImportThunks(partitioner, interp);

    SgAsmBlock *globalBlock = NULL;

    // Show the results as requested
    if (settings.doShowStats) {
        std::cout <<"CFG contains " <<StringUtility::plural(partitioner.nFunctions(), "functions") <<"\n";
        std::cout <<"CFG contains " <<StringUtility::plural(partitioner.nBasicBlocks(), "basic blocks") <<"\n";
        std::cout <<"CFG contains " <<StringUtility::plural(partitioner.nDataBlocks(), "data blocks") <<"\n";
        std::cout <<"CFG contains " <<StringUtility::plural(partitioner.nInstructions(), "instructions") <<"\n";
        std::cout <<"CFG contains " <<StringUtility::plural(partitioner.nBytes(), "bytes") <<"\n";
        std::cout <<"Instruction cache contains "
                  <<StringUtility::plural(partitioner.instructionProvider().nCached(), "instructions") <<"\n";
        std::cout <<"Specimen contains " <<StringUtility::plural(executableSpace.size(), "executable bytes") <<"\n";
        size_t nMapped = executableSpace.size();
        std::cout <<"CFG covers " <<(100.0*partitioner.nBytes()/nMapped) <<"% of executable bytes\n";
        std::cout <<"Executable bytes not covered by CFG: " <<(nMapped-partitioner.nBytes()) <<"\n";
    }

    if (settings.doListCfg) {
        std::cout <<"Final control flow graph:\n";
        partitioner.dumpCfg(std::cout, "  ", true);
    }

    if (settings.doListFunctions) {
        if (!globalBlock)
            globalBlock = partitioner.buildAst();
        std::cout <<AsmFunctionIndex(globalBlock);
    }

    if (settings.doListAum) {
        std::cout <<"Final address usage map:\n";
        partitioner.aum().print(std::cout, "  ");
    }
    
    if (settings.doListUnused) {
        Sawyer::Container::IntervalSet<AddressInterval> unusedAddresses = partitioner.aum().unusedExtent(wordSize);
        std::cout <<"Unused addresses: " <<StringUtility::plural(unusedAddresses.size(), "bytes")
                  <<" in " <<StringUtility::plural(unusedAddresses.nIntervals(), "intervals") <<"\n";
        BOOST_FOREACH (const AddressInterval &unused, unusedAddresses.nodes()) {
            std::cout <<unused;
            std::cout <<"\t" <<StringUtility::plural(unused.size(), "bytes") <<"\n";
        }
    }

    if (settings.doListFunctionAddresses) {
        BOOST_FOREACH (P2::Function::Ptr function, partitioner.functions()) {
            rose_addr_t entryVa = function->address();
            P2::BasicBlock::Ptr bb = partitioner.basicBlockExists(entryVa);
            std::cout <<partitioner.functionName(function) <<": "
                      <<(bb && !bb->isEmpty() ? "exists" : "missing") <<"\n";
        }
    }

    // Build the AST and unparse it.
    if (settings.doListAsm) {
        if (!globalBlock)
            globalBlock = partitioner.buildAst();
        ControlFlow::BlockGraph cfg = ControlFlow().build_block_cfg_from_ast<ControlFlow::BlockGraph>(globalBlock);
        AsmUnparser unparser;
        unparser.set_registers(disassembler->get_registers());
        unparser.add_control_flow_graph(cfg);
        unparser.staticDataDisassembler.init(disassembler);
        unparser.unparse(std::cout, globalBlock);
    }

#if 0 // DEBUGGING [Robb P. Matzke 2014-08-23]
    // This should free all symbolic expressions except for perhaps a few held by something we don't know about.
    partitioner.clear();
    InsnSemanticsExpr::TreeNode::poolAllocator().showInfo(std::cerr);
    std::cerr <<"all done; entering busy loop\n";
    while (1);                                          // makes us easy to find in process listings
#endif

    exit(0);
}
