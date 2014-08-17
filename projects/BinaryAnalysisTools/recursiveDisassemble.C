#include <rose.h>
#include <rosePublicConfig.h>
#include <AsmUnparser.h>
#include <BinaryControlFlow.h>
#include <DisassemblerArm.h>
#include <DisassemblerPowerpc.h>
#include <DisassemblerMips.h>
#include <DisassemblerX86.h>
#include <DisassemblerM68k.h>
#include <Partitioner2/Partitioner.h>
#include <Partitioner2/Utility.h>

#include <sawyer/Assert.h>
#include <sawyer/CommandLine.h>
#include <sawyer/ProgressBar.h>

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

// Convenient struct to hold settings from the command-line all in one place.
struct Settings {
    std::string isaName;                                // instruction set architecture name
    rose_addr_t mapVa;                                  // where to map the specimen in virtual memory
    bool followGhostEdges;                              // do we ignore opaque predicates?
    bool findDeadCode;                                  // do we look for unreachable basic blocks?
    bool intraFunctionData;                             // suck up unused addresses as intra-function data
    bool doListCfg;                                     // list the control flow graph
    bool doListAum;                                     // list the address usage map
    bool doListAsm;                                     // produce an assembly-like listing with AsmUnparser
    bool doListFunctionAddresses;                       // list function entry addresses
    bool doShowStats;                                   // show some statistics
    bool doListUnused;                                  // list unused addresses
    Settings()
        : mapVa(0), followGhostEdges(false), findDeadCode(true), intraFunctionData(true),
          doListCfg(false), doListAum(false), doListAsm(true), doListFunctionAddresses(false),
          doShowStats(false), doListUnused(false) {}
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

    // Switches for disassembly
    SwitchGroup dis;
    dis.doc("Switches for disassembly:");
    dis.insert(Switch("isa")
               .argument("architecture", anyParser(settings.isaName))
               .doc("Instruction set architecture. Specify \"list\" to see a list of possible ISAs."));
    dis.insert(Switch("map")
               .argument("virtual-address", nonNegativeIntegerParser(settings.mapVa))
               .doc("The first byte of the file is mapped at the specified @v{virtual-address}, which defaults "
                    "to " + StringUtility::addrToString(settings.mapVa) + "."));
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
class Monitor: public P2::Partitioner::CfgAdjustmentCallback {
public:
    static Ptr instance() { return Ptr(new Monitor); }
    virtual bool operator()(bool chain, const AttachedBasicBlock &args) {
        std::cerr <<"+";
        if (args.bblock)
            std::cerr <<std::string(args.bblock->nInsns(), '.');
        return chain;
    }
    virtual bool operator()(bool chain, const DetachedBasicBlock &args) {
        std::cerr <<"-";
        if (args.bblock)
            std::cerr <<"-" <<std::string(args.bblock->nInsns(), '.');
        return chain;
    }
};

// Example of making adjustments to basic block successors.  If this callback is registered with the partitioner, then it will
// add ghost edges (non-taken side of branches with opaque predicates) to basic blocks as their instructions are discovered.
// An alternative method is to investigate ghost edges after functions are discovered in order to find dead code.
class AddGhostSuccessors: public P2::Partitioner::SuccessorCallback {
public:
    static Ptr instance() { return Ptr(new AddGhostSuccessors); }

    virtual bool operator()(bool chain, const Args &args) {
        size_t nBits = args.partitioner->instructionProvider().instructionPointerRegister().get_nbits();
        BOOST_FOREACH (rose_addr_t successorVa, args.partitioner->basicBlockGhostSuccessors(args.bblock)) {
            args.bblock->insertSuccessor(successorVa, nBits);
            P2::mlog[INFO] <<"opaque predicate at "
                           <<StringUtility::addrToString(args.bblock->instructions().back()->get_address())
                           <<": branch " <<StringUtility::addrToString(successorVa) <<" never taken\n";
        }
        return chain;
    }
};

// Looks for m68k instruction prologues;  Look for a LINK.W whose first argument is A6
class MatchM68kLink: public P2::Partitioner::FunctionPrologueMatcher {
protected:
    P2::Function::Ptr function_;
public:
    static Ptr instance() { return Ptr(new MatchM68kLink); }

    virtual P2::Function::Ptr function() const {
        return function_;
    }

    virtual bool match(P2::Partitioner *partitioner, rose_addr_t anchor) /*override*/ {
        if (anchor & 1)
            return false;                               // m68k instructions must be 16-bit aligned
        static const RegisterDescriptor REG_A6(m68k_regclass_addr, 6, 0, 32);
        if (SgAsmM68kInstruction *insn = isSgAsmM68kInstruction(partitioner->discoverInstruction(anchor))) {
            const SgAsmExpressionPtrList &args = insn->get_operandList()->get_operands();
            if (insn->get_kind()==m68k_link && args.size()==2) {
                if (SgAsmDirectRegisterExpression *rre = isSgAsmDirectRegisterExpression(args[0])) {
                    if (rre->get_descriptor()==REG_A6) {
                        function_ = P2::Function::instance(anchor);
                        return true;
                    }
                }
            }
        }
        return false;
    }
};

#if 0 // [Robb P. Matzke 2014-08-13]
class MatchX86Prologue: public P2::FunctionPrologueMatcher {
protected:
    P2::Function::Ptr function_;
public:
    static Ptr instance() { return Ptr(new MatchX86Prologue); }

    virtual P2::Function::Ptr function() const {
        return function_;
    }

    virtual bool match(P2::Partitioner *partitioner, rose_addr_t anchor) /*override*/ {
        // Look for optional MOV RDI, RDI
        rose_addr_t va = anchor;
        do {
            SgAsmx86Instruction *insn = isSgAsmx86Instruction(partitioner->discoverInstruction(va));
            if (!insn || insn->get_kind()!=x86_mov)
                break;
            const SgAsmExpressionPtrList &opands = insn->get_operandList()->get_operands();
            if (opands.size()!=2)
                break;
            SgAsmRegisterReferenceExpression *rre = isSgAsmRegisterReferenceExpression(opands[0]);
            if (!rre ||
                rre->get_descriptor().get_major()!=x86_regclass_gpr ||
                rre->get_descriptor().get_minor()!=x86_gpr_di)
                break;
            rre = isSgAsmRegisterReferenceExpression(opands[1]);
            if (!rre ||
                rre->get_descriptor().get_major()!=x86_regclass_gpr ||
                rre->get_descriptor().get_minor()!=x86_gpr_di)
                break;
            va += insn->get_size();
        } while (0);

        // Look for PUSH RBP at the (adjusted) anchor address. It must not already be in the CFG
        SgAsmx86Instruction *insn = isSgAsmx86Instruction(partitioner->discoverInstruction(va));
        {
            if (partitioner->instructionExists(va))
                return false;
            if (!insn || insn->get_kind()!=x86_push)
                return false;
            const SgAsmExpressionPtrList &opands = insn->get_operandList()->get_operands();
            if (opands.size()!=1)
                return false;
            SgAsmRegisterReferenceExpression *rre = isSgAsmRegisterReferenceExpression(opands[0]);
            if (!rre ||
                rre->get_descriptor().get_major()!=x86_regclass_gpr ||
                rre->get_descriptor().get_minor()!=x86_gpr_bp)
                return false;
        }

        // Look for MOV RBP,RSP following the PUSH. It must not already be in the CFG
        {
            rose_addr_t moveVa = insn->get_address() + insn->get_size();
            if (partitioner->instructionExists(moveVa))
                return false;
            insn = isSgAsmx86Instruction(partitioner->discoverInstruction(moveVa));
            if (!insn || insn->get_kind()!=x86_mov)
                return false;
            const SgAsmExpressionPtrList &opands = insn->get_operandList()->get_operands();
            if (opands.size()!=2)
                return false;
            SgAsmRegisterReferenceExpression *rre = isSgAsmRegisterReferenceExpression(opands[0]);
            if (!rre ||
                rre->get_descriptor().get_major()!=x86_regclass_gpr ||
                rre->get_descriptor().get_minor()!=x86_gpr_bp)
                return false;
            rre = isSgAsmRegisterReferenceExpression(opands[1]);
            if (!rre ||
                rre->get_descriptor().get_major()!=x86_regclass_gpr ||
                rre->get_descriptor().get_minor()!=x86_gpr_sp)
                return false;
        }

        function_ = P2::Function::instance(anchor);
        return true;
    }
};
#endif

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

// Tries to assign basic blocks to functions.  Returns the number of functions that were successfully processed.
static size_t
findFunctionBlocks(P2::Partitioner &partitioner) {
    size_t nProcessed = 0;
    // Get a list of functions (those we already found, and those created due to function call edges), and discover basic
    // blocks for each. Attach the functions and their blocks to the CFG.  The discoverFunctionEntryVertices returns a list of
    // functions, some of which are already attached to the CFG/AUM and some which are detached.  Before we add new basic
    // blocks to a function we must detach it from the CFG/AUM.
    BOOST_FOREACH (const P2::Function::Ptr &function, partitioner.discoverFunctionEntryVertices()) {
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
findIntraFunctionData(P2::Partitioner &partitioner, const AddressInterval textArea)
{
    size_t nProcessed = 0;
    Sawyer::Container::IntervalSet<AddressInterval> unused = partitioner.aum().unusedExtent(textArea);
    BOOST_FOREACH (const AddressInterval &interval, unused.nodes()) {
        if (interval.least()<=textArea.least() || interval.greatest()>=textArea.greatest())
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

    // Obtain a disassembler (do this before opening the specimen so "--isa=list" has a chance to run)
    Disassembler *disassembler = getDisassembler(settings.isaName);
    ASSERT_not_null(disassembler);
    disassembler->set_progress_reporting(-1.0);         // turn it off

    // Open the file that needs to be disassembled and map it with read and execute permission
    if (positionalArgs.empty())
        throw std::runtime_error("no file name specified; see --help");
    if (positionalArgs.size()>1)
        throw std::runtime_error("too many files specified; see --help");
    std::string specimenName = positionalArgs[0];
    MemoryMap map;
    size_t nBytesMapped = map.insert_file(specimenName, settings.mapVa);
    if (0==nBytesMapped)
        throw std::runtime_error("problem reading file: " + specimenName);
    map.mprotect(AddressInterval::baseSize(settings.mapVa, nBytesMapped), MemoryMap::MM_PROT_RX);
    map.dump(std::cerr);                                // debugging so the user can see the map
    AddressInterval addressSpace = AddressInterval::baseSize(settings.mapVa, nBytesMapped);

    // Create the partitioner
    P2::Partitioner partitioner(disassembler, map);
    partitioner.functionPrologueMatchers().push_back(MatchM68kLink::instance());
    if (settings.followGhostEdges)
        partitioner.successorCallbacks().append(AddGhostSuccessors::instance());
#if 0 // [Robb P. Matzke 2014-08-16]: fun to watch, but verbose!
    partitioner.cfgAdjustmentCallbacks().append(Monitor::instance());
#endif

    // Disassemble as many basic blocks as we can find by following the control flow and looking for function prologues.
    rose_addr_t prologueVa = 0;
    while (findBasicBlock(partitioner) || findFunctionPrologue(partitioner, prologueVa)) /*void*/;
    
    findFunctionBlocks(partitioner);                    // organize existing basic blocks into functions
    if (settings.findDeadCode)
        findDeadCode(partitioner);                      // find unreachable code and add it to functions
    findFunctionPadding(partitioner);                   // find function alignment padding before entry points
    if (settings.intraFunctionData)
        findIntraFunctionData(partitioner, addressSpace); // find data areas that are enclosed by functions
    

    // Perform a final pass over all functions and issue reports about which functions have unreasonable control flow.
    BOOST_FOREACH (const P2::Function::Ptr &function, partitioner.functions()) {
        P2::EdgeList inwardConflictEdges, outwardConflictEdges;
        partitioner.detachFunction(function);           // temporarily detach so we can call discoverFunctionBasicBlocks
        if (0==partitioner.discoverFunctionBasicBlocks(function, &inwardConflictEdges, &outwardConflictEdges)) {
            partitioner.attachFunction(function);
        } else {
            P2::mlog[WARN] <<"discovery for function " <<StringUtility::addrToString(function->address())
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

    // Show the results as requested
    if (settings.doShowStats) {
        std::cout <<"CFG contains " <<StringUtility::plural(partitioner.nFunctions(), "functions") <<"\n";
        std::cout <<"CFG contains " <<StringUtility::plural(partitioner.nBasicBlocks(), "basic blocks") <<"\n";
        std::cout <<"CFG contains " <<StringUtility::plural(partitioner.nInstructions(), "instructions") <<"\n";
        std::cout <<"CFG contains " <<StringUtility::plural(partitioner.nBytes(), "bytes") <<"\n";
        std::cout <<"Instruction cache contains "
                  <<StringUtility::plural(partitioner.instructionProvider().nCached(), "instructions") <<"\n";
        std::cout <<"Specimen contains " <<StringUtility::plural(nBytesMapped, "bytes") <<"\n";
        std::cout <<"CFG covers " <<(100.0*partitioner.nBytes()/nBytesMapped) <<"% of specimen\n";
        std::cout <<"Specimen not covered by CFG: " <<StringUtility::plural(nBytesMapped-partitioner.nBytes(), "bytes") <<"\n";
    }

    if (settings.doListCfg) {
        std::cout <<"Final control flow graph:\n";
        partitioner.dumpCfg(std::cout, "  ", true);
    }

    if (settings.doListAum) {
        std::cout <<"Final address usage map:\n";
        partitioner.aum().print(std::cout, "  ");
    }
    
    if (settings.doListUnused) {
        Sawyer::Container::IntervalSet<AddressInterval> unusedAddresses = partitioner.aum().unusedExtent(addressSpace);
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
        SgAsmBlock *globalBlock = partitioner.buildAst();
        ControlFlow::BlockGraph cfg = ControlFlow().build_block_cfg_from_ast<ControlFlow::BlockGraph>(globalBlock);
        AsmUnparser unparser;
        unparser.set_registers(disassembler->get_registers());
        unparser.add_control_flow_graph(cfg);
        unparser.staticDataDisassembler.init(disassembler);
        unparser.unparse(std::cout, globalBlock);
    }
    
    exit(0);
}
