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
#include <Partitioner2/Engine.h>
#include <Partitioner2/ModulesM68k.h>
#include <Partitioner2/ModulesPe.h>

#include <sawyer/Assert.h>
#include <sawyer/CommandLine.h>
#include <sawyer/ProgressBar.h>

#include <boost/algorithm/string/predicate.hpp>


// FIXME[Robb P. Matzke 2014-08-24]: These matchers still need to be implemented:
/* 
 | name                   | purpose                                                   |
 |------------------------+-----------------------------------------------------------|
 | FindFunctionFragments  | uses code criteria to make code from data                 |
 | FindThunks             | makes functions from BB's containing only a JMP           |
 | FindInterpadFunctions  | find functions between other functions                    |
 | FindThunkTables        | find long sequences of JMP instructions                   |
 | FindPostFunctionInsns  |                                                           |
 */

// FIXME[Robb P. Matzke 2014-08-28]: SgAsmInstruction::is_function_call repeats most of the instruction semantics that occur in
// the partitioner and is called when the partitioner semantics failed or were disabled.  Therefore, turning off semantics in
// the partitioner actually makes the partitioner run slower than normal (because is_function_call is stateless), but if you
// comment out the semantics inside is_function_call then the speed increase is huge.

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
    size_t deExecuteZeros;                              // threshold for removing execute permissions of zeros (zero disables)
    bool useSemantics;                                  // should we use symbolic semantics?
    bool followGhostEdges;                              // do we ignore opaque predicates?
    bool allowDiscontiguousBlocks;                      // can basic blocks be discontiguous in memory?
    bool findFunctionPadding;                           // look for pre-entry-point padding?
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
        : mapVa(NO_ADDRESS), deExecuteZeros(0), useSemantics(true), followGhostEdges(false), allowDiscontiguousBlocks(true),
          findFunctionPadding(true), findSwitchCases(true), findDeadCode(true), intraFunctionData(true),
          doListCfg(false), doListAum(false), doListAsm(true), doListFunctions(false), doListFunctionAddresses(false),
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
    dis.insert(Switch("find-function-padding")
               .intrinsicValue(true, settings.findFunctionPadding)
               .doc("Look for padding such as zero bytes and certain instructions like no-ops that occur prior to the "
                    "lowest address of a function and attach them to the function as static data.  The "
                    "@s{no-find-function-padding} switch turns this off.  The default is to " +
                    std::string(settings.findFunctionPadding?"":"not ") + "search for padding."));
    dis.insert(Switch("no-find-function-padding")
               .key("find-function-padding")
               .intrinsicValue(false, settings.findFunctionPadding)
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
    dis.insert(Switch("remove-zeros")
               .argument("size", nonNegativeIntegerParser(settings.deExecuteZeros), "128")
               .doc("This switch causes execute permission to be removed from sequences of contiguous zero bytes. The "
                    "switch argument is the minimum number of consecutive zeros that will trigger the removal, and "
                    "defaults to 128.  An argument of zero disables the removal.  When this switch is not specified at "
                    "all, this tool assumes a value of " + StringUtility::plural(settings.deExecuteZeros, "bytes") + "."));

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
             "This program tests the new partitioner architecture by disassembling the specified file.");
    
    return parser.with(gen).with(dis).with(out).parse(argc, argv).apply();
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                              Callbacks
//
// Callbacks are a way of adapting the partitioner to a particular task, architecture, etc. They are different than subclassing
// in that they are more of a pluggable-module approach where one can mix and match different behaviors in a non-linear way.
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


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



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                              Function-making
//
// These functions demonstrate how to make a function at a particular address. See also the "make*" functions in
// rose::BinaryAnalysis::Partitioner2::Engine.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Make functions for any x86 CALL instruction. This is intended to be a demonstration of how to search for specific
// instruction patterns and do something when the pattern is found, and probably isn't all that useful since function calls in
// reachable code are already detected anyway as part of Partitioner::discoverBasicBlock.
void
makeCallTargetFunctions(P2::Partitioner &partitioner, size_t alignment=1) {
    std::set<rose_addr_t> targets;                      // distinct call targets

    // Iterate over every executable address in the memory map
    for (rose_addr_t va=0; partitioner.memoryMap().atOrAfter(va).require(MemoryMap::EXECUTABLE).next().assignTo(va); ++va) {
        if (alignment>1)                                // apply alignment here as an optimization
            va = ((va+alignment-1)/alignment)*alignment;

        // Disassemble an instruction (or get one that was previously disassembled) and see if it's a function call. The
        // function call detection normally operates at a basic block level, so we make a singleton basic block since we're
        // interested only in single instructions.
        if (SgAsmInstruction *insn = partitioner.discoverInstruction(va)) {
            std::vector<SgAsmInstruction*> bb(1, insn);
            rose_addr_t target = NO_ADDRESS;
            if (insn->is_function_call(bb, &target, NULL) &&
                partitioner.memoryMap().at(target).require(MemoryMap::EXECUTABLE).exists()) {
                targets.insert(target);
            }
        }
    }

    // Now create functions at each target address
    BOOST_FOREACH (rose_addr_t entryVa, targets) {
        P2::Function::Ptr function = P2::Function::instance(entryVa, SgAsmFunction::FUNC_CALL_INSN);
        partitioner.attachOrMergeFunction(function);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
        throw std::runtime_error("no specimen specified; see --help");
    if (positionalArgs.size()>1)
        throw std::runtime_error("too many specimens specified; see --help");
    std::string specimenName = positionalArgs[0];

    // Create an engine to drive the partitioning.  This is entirely optional.  All an engine does is define the sequence of
    // partitioning calls that need to be made in order to recognize instructions, basic blocks, data blocks, and functions.
    // We instantiate the engine early because it has some nice methods that we can use.
    P2::Engine engine;

    // Load the specimen as raw data or an ELF or PE container
    SgAsmInterpretation *interp = NULL;
    MemoryMap map;
    if (settings.mapVa!=NO_ADDRESS) {
        if (!disassembler)
            throw std::runtime_error("an instruction set architecture must be specified with the \"--isa\" switch");
        size_t nBytesMapped = map.insertFile(specimenName, settings.mapVa);
        if (0==nBytesMapped)
            throw std::runtime_error("problem reading file: " + specimenName);
        map.at(settings.mapVa).limit(nBytesMapped).changeAccess(MemoryMap::EXECUTABLE, 0);
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
        disassembler = engine.loadSpecimen(interp, disassembler);
        ASSERT_not_null(interp->get_map());
        map = *interp->get_map();
    }
    disassembler->set_progress_reporting(-1.0);         // turn it off

#if 0 // [Robb P. Matzke 2014-08-29]
    // Remove execute permission from all segments of memory except those with ".text" as part of their name.
    BOOST_FOREACH (const MemoryMap::Segments::Node &node, map.segments().nodes()) {
        if (!boost::contains(node.value().get_name(), ".text")) {
            std::cerr <<"ROBB: removing execute from " <<node.value().get_name() <<"\n";
            unsigned newPerms = node.value().get_mapperms() & ~MemoryMap::MM_PROT_EXEC;
            map.mprotect(node.key(), newPerms);
        }
    }
#endif

    // Remove execute permission from regions of memory that contain only zero.  This will prevent them from being disassembled
    // since they're almost always padding at the end of sections (e.g., MVC pads all sections to 256 bytes by appending zeros).
    P2::Modules::deExecuteZeros(map /*in,out*/, settings.deExecuteZeros);

    // Some analyses need to know what part of the address space is being disassembled.
    AddressIntervalSet executableSpace;
    BOOST_FOREACH (const MemoryMap::Node &node, map.nodes()) {
        if ((node.value().accessibility() & MemoryMap::EXECUTABLE)!=0)
            executableSpace.insert(node.key());
    }

    // Create a partitioner that's tuned for a certain architecture, and then tune it even more depending on our command-line.
    P2::Partitioner partitioner = engine.createTunedPartitioner(disassembler, map);
    partitioner.enableSymbolicSemantics(settings.useSemantics);
    if (settings.findSwitchCases)
        partitioner.basicBlockCallbacks().append(P2::ModulesM68k::SwitchSuccessors::instance());
    if (settings.followGhostEdges)
        partitioner.basicBlockCallbacks().append(P2::Modules::AddGhostSuccessors::instance());
    if (!settings.allowDiscontiguousBlocks)
        partitioner.basicBlockCallbacks().append(P2::Modules::PreventDiscontiguousBlocks::instance());
    if (false)
        partitioner.cfgAdjustmentCallbacks().append(Monitor::instance());// fun, but very verbose
    if (false)
        makeCallTargetFunctions(partitioner);           // not useful; see documentation at function definition
    partitioner.memoryMap().dump(std::cout);            // show what we'll be working on

    // Find interesting places at which to disassemble.  This traverses the interpretation (if any) to find things like
    // specimen entry points, exception handling, imports and exports, and symbol tables.
    engine.makeContainerFunctions(partitioner, interp);

    // Do an initial pass to discover functions and partition them into basic blocks and functions. Functions for which the CFG
    // is a bit wonky won't get assigned any basic blocks (other than the entry blocks we just added above).
    engine.discoverFunctions(partitioner);

    // Various fix-ups
    if (settings.findDeadCode)
        engine.attachDeadCodeToFunctions(partitioner);  // find unreachable code and add it to functions
    if (settings.findFunctionPadding)
        engine.attachPaddingToFunctions(partitioner);   // find function alignment padding before entry points
    if (settings.intraFunctionData)
        engine.attachSurroundedDataToFunctions(partitioner); // find data areas that are enclosed by functions

    // Perform a final pass over all functions and issue reports about which functions have unreasonable control flow.
    engine.attachBlocksToFunctions(partitioner, true/*emit warnings*/);

    // Now that the partitioner's work is all done, try to give names to some things.  Most functions will have been given
    // names when we marked their locations, but import thunks can be scattered all over the place and its nice if we give them
    // the same name as the imported function to which they point.  This is especially important if there's no basic block at
    // the imported function's address (i.e., the dynamic linker hasn't run) because ROSE's AST can't represent basic blocks
    // that have no instructions, and therefore the imported function's address doesn't even show up in ROSE.
    engine.postPartitionFixups(partitioner, interp);

    SgAsmBlock *globalBlock = NULL;

    //-------------------------------------------------------------- 
    // The rest of main() is just about showing the results...
    //-------------------------------------------------------------- 
    
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
        AddressIntervalSet unusedAddresses = partitioner.aum().unusedExtent(executableSpace);
        std::cout <<"Unused addresses: " <<StringUtility::plural(unusedAddresses.size(), "bytes")
                  <<" in " <<StringUtility::plural(unusedAddresses.nIntervals(), "intervals") <<"\n";
        BOOST_FOREACH (const AddressInterval &unused, unusedAddresses.intervals())
            std::cout <<unused <<"\t" <<StringUtility::plural(unused.size(), "bytes") <<"\n";
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
        AsmUnparser unparser;
        unparser.set_registers(disassembler->get_registers());
        unparser.add_control_flow_graph(ControlFlow().build_block_cfg_from_ast<ControlFlow::BlockGraph>(globalBlock));
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
