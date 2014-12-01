#include <rose.h>
#include <rosePublicConfig.h>
#include <AsmFunctionIndex.h>
#include <AsmUnparser.h>
#include <BinaryControlFlow.h>
#include <BinaryLoader.h>
#include <Disassembler.h>
#include <Partitioner2/Engine.h>
#include <Partitioner2/ModulesM68k.h>
#include <Partitioner2/ModulesPe.h>
#include <Partitioner2/Utility.h>

#include <sawyer/Assert.h>
#include <sawyer/CommandLine.h>
#include <sawyer/ProgressBar.h>
#include <sawyer/Stopwatch.h>

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

using namespace rose;
using namespace rose::BinaryAnalysis;
using namespace rose::Diagnostics;

namespace P2 = Partitioner2;

static const rose_addr_t NO_ADDRESS(-1);

// Convenient struct to hold settings from the command-line all in one place.
struct Settings {
    std::string isaName;                                // instruction set architecture name
    size_t deExecuteZeros;                              // threshold for removing execute permissions of zeros (zero disables)
    bool useSemantics;                                  // should we use symbolic semantics?
    bool followGhostEdges;                              // do we ignore opaque predicates?
    bool allowDiscontiguousBlocks;                      // can basic blocks be discontiguous in memory?
    bool findFunctionPadding;                           // look for pre-entry-point padding?
    bool findDeadCode;                                  // do we look for unreachable basic blocks?
    bool intraFunctionCode;                             // suck up unused addresses as intra-function code
    bool intraFunctionData;                             // suck up unused addresses as intra-function data
    bool doListCfg;                                     // list the control flow graph
    bool doListAum;                                     // list the address usage map
    bool doListAsm;                                     // produce an assembly-like listing with AsmUnparser
    bool doListFunctions;                               // produce a function index
    bool doListFunctionAddresses;                       // list function entry addresses
    bool doListInstructionAddresses;                    // show instruction addresses
    bool doShowMap;                                     // show the memory map
    bool doShowStats;                                   // show some statistics
    bool doListUnused;                                  // list unused addresses
    bool assumeFunctionsReturn;                         // do functions usually return to their caller?
    std::vector<std::string> triggers;                  // debugging aids
    Settings()
        : deExecuteZeros(0), useSemantics(false), followGhostEdges(false), allowDiscontiguousBlocks(true),
          findFunctionPadding(true), findDeadCode(true), intraFunctionCode(true), intraFunctionData(true), doListCfg(false),
          doListAum(false), doListAsm(true), doListFunctions(false), doListFunctionAddresses(false),
          doListInstructionAddresses(false), doShowMap(false), doShowStats(false), doListUnused(false),
          assumeFunctionsReturn(true) {}
};

// Describe and parse the command-line
static Sawyer::CommandLine::ParserResult
parseCommandLine(int argc, char *argv[], Settings &settings)
{
    using namespace Sawyer::CommandLine;

    Parser parser;
    parser
        .purpose("disassembles and partitions binary specimens")
        .version(std::string(ROSE_SCM_VERSION_ID).substr(0, 8), ROSE_CONFIGURE_DATE)
        .chapter(1, "ROSE Command-line Tools")
        .doc("synopsis",
             "@prop{programName} [@v{switches}] @v{specimen_names}")
        .doc("description",
             "Parses, disassembles and partitions the specimens given as positional arguments on the command-line.")
        .doc("Specimens", P2::Engine::specimenNameDocumentation())
        .doc("Bugs", "[999]-bugs",
             "Probably many, and we're interested in every one.  Send bug reports to <matzke1@llnl.gov>.");
    
    // Generic switches
    SwitchGroup gen = CommandlineProcessing::genericSwitches();
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
    SwitchGroup dis("Disassembly switches");
    dis.insert(Switch("isa")
               .argument("architecture", anyParser(settings.isaName))
               .doc("Instruction set architecture. Specify \"list\" to see a list of possible ISAs."));

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

    dis.insert(Switch("intra-function-code")
               .intrinsicValue(true, settings.intraFunctionCode)
               .doc("Near the end of processing, if there are regions of unused memory that are immediately preceded and "
                    "followed by the same single function then a basic block is create at the beginning of that region and "
                    "added as a member of the surrounding function.  A function block discover phase follows in order to "
                    "find the instructions for the new basic blocks and to follow their control flow to add additional "
                    "blocks to the functions.  These two steps are repeated until no new code can be created.  This step "
                    "occurs before the @s{intra-function-data} step if both are enabled.  The @s{no-intra-function-code} "
                    "switch turns this off. The default is to " + std::string(settings.intraFunctionCode?"":"not ") +
                    "perform this analysis."));
    dis.insert(Switch("no-intra-function-code")
               .key("intra-function-code")
               .intrinsicValue(false, settings.intraFunctionCode)
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

    dis.insert(Switch("functions-return")
               .argument("boolean", booleanParser(settings.assumeFunctionsReturn))
               .doc("If the disassembler's may-return analysis is inconclusive then either assume that such functions may "
                    "return to their caller or never return.  The default is that they " +
                    std::string(settings.assumeFunctionsReturn?"may":"never") + " return."));

    // Switches for output
    SwitchGroup out("Output switches");
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

    out.insert(Switch("list-function-addresses")
               .intrinsicValue(true, settings.doListFunctionAddresses)
               .doc("Produce a listing of function entry addresses, one address per line in hexadecimal format. Each address "
                    "is followed by the word \"existing\" or \"missing\" depending on whether a non-empty basic block exists "
                    "in the CFG for the function entry address.  The listing is disabled with @s{no-list-function-addresses}."));
    out.insert(Switch("no-list-function-addresses")
               .key("list-function-addresses")
               .intrinsicValue(false, settings.doListFunctionAddresses)
               .hidden(true));

    out.insert(Switch("list-instruction-addresses")
               .intrinsicValue(true, settings.doListInstructionAddresses)
               .doc("Produce a listing of instruction addresses.  Each line of output will contain three space-separated "
                    "items: the address interval for the instruction (address followed by \"+\" followed by size), the "
                    "address of the basic block to which the instruction belongs, and the address of the function to which "
                    "the basic block belongs.  If the basic block doesn't belong to a function then the string \"nil\" is "
                    "printed for the function address field.  This listing is disabled with the "
                    "@s{no-list-instruction-addresses} switch.  The default is to " +
                    std::string(settings.doListInstructionAddresses?"":"not ") + "show this information."));
    out.insert(Switch("no-list-instruction-addresses")
               .key("list-instruction-addresses")
               .intrinsicValue(false, settings.doListInstructionAddresses)
               .hidden(true));

    out.insert(Switch("list-unused-addresses")
               .intrinsicValue(true, settings.doListUnused)
               .doc("Produce a listing of all specimen addresses that are not represented in the control flow graph. This "
                    "listing can be disabled with @s{no-list-unused}."));
    out.insert(Switch("no-list-unused-addresses")
               .key("list-unused-addresses")
               .intrinsicValue(false, settings.doListUnused)
               .hidden(true));

    out.insert(Switch("show-map")
               .intrinsicValue(true, settings.doShowMap)
               .doc("Show the memory map that was used for disassembly.  The @s{no-show-map} switch turns this off. The "
                    "default is to " + std::string(settings.doShowMap?"":"not ") + "show the memory map."));
    out.insert(Switch("no-show-map")
               .key("show-map")
               .intrinsicValue(false, settings.doShowMap)
               .hidden(true));

    out.insert(Switch("show-stats")
               .intrinsicValue(true, settings.doShowStats)
               .doc("Emit some information about how much of the input was disassembled."));
    out.insert(Switch("no-show-stats")
               .key("show-stats")
               .intrinsicValue(false, settings.doShowStats)
               .hidden(true));

    SwitchGroup dbg("Debugging switches");
    dbg.insert(Switch("trigger")
               .argument("what", anyParser(settings.triggers))
               .whichValue(SAVE_ALL)
               .doc("Activates a debugging aid triggered by a certain event. For instance, if you're trying to figure "
                    "out why a function prologue pattern created a function at a location where you think there should "
                    "have already been an instruction, then it would be useful to have a list of CFG-attached instructions "
                    "at the exact point when the function prologue was matched. The switch "
                    "\"@s{trigger} insn-list:bblock=0x0804cfa1:insns=0x0804ca00,0x0804da10\" will do such a thing. Namely, "
                    "the first time a basic block at 0x0804cfa1 is added to the CFG it will list all CFG-attached instructions "
                    "between the addresses 0x0804ca00 and 0x0804da10.  Multiple @s{trigger} switches can be specified. Each "
                    "one takes a value which is the name of the debugging aid (e.g., \"insn-list\") and zero or more "
                    "configuration arguments with a colon between the name and each argument."
                    "\n\n"
                    "In the descriptions that follow, leading hyphens can be omitted and an equal sign can separate the "
                    "name and value. That is, \":bblock=0x123\" works just as well as \":--bblock 0x123\".  Integers may "
                    "generally be specified in C/C++ syntax: hexadecimal (leading \"0x\"), binary (leading \"0b\"), "
                    "octal (leading \"0\"), or decimal.  Intervals can be specified with a single integer to represent a "
                    "singleton interval, a minimum and maximum value separated by a comma as in \"20,29\", a beginning "
                    "and exclusive end separated by a hpyhen as in \"20-30\", or a beginning and size separated by a \"+\" "
                    "as in \"20+10\"."
                    "\n\n"
                    "Debugging aids generally send their output to the rose::BinaryAnalysis::Partitioner2[DEBUG] stream. "
                    "There is no need to turn this stream on explicitly from the command line since the debugging aids "
                    "temporarily enable it.  They assume that if you took the time to specify their parameters then you "
                    "probably want to see their output!"
                    "\n\n"
                    "The following debugging aids are available:"
                    "@named{cfg-dot}{" + P2::Modules::CfgGraphVizDumper::docString() + "}"
                    "@named{hexdump}{" + P2::Modules::HexDumper::docString() + "}"
                    "@named{insn-list}{" + P2::Modules::InstructionLister::docString() + "}"
                    "@named{debugger}{" + P2::Modules::Debugger::docString() + "}"
                    ));
    

    return parser.with(gen).with(dis).with(out).with(dbg).parse(argc, argv).apply();
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
            if (insn->isFunctionCallFast(bb, &target, NULL) &&
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
    std::vector<std::string> specimenNames = cmdline.unreachedArgs();
    Disassembler *disassembler = NULL;
    if (!settings.isaName.empty())
        disassembler = Disassembler::lookup(settings.isaName);
    if (specimenNames.empty())
        throw std::runtime_error("no specimen specified; see --help");

    // Create an engine to drive the partitioning.  This is entirely optional.  All an engine does is define the sequence of
    // partitioning calls that need to be made in order to recognize instructions, basic blocks, data blocks, and functions.
    // We instantiate the engine early because it has some nice methods that we can use.
    P2::Engine engine;

    // Load the specimen as raw data or an ELF or PE container.
    MemoryMap map = engine.load(specimenNames);
    SgAsmInterpretation *interp = engine.interpretation();
    if (NULL==(disassembler = engine.obtainDisassembler(disassembler)))
        throw std::runtime_error("an instruction set architecture must be specified with the \"--isa\" switch");

#if 0 // [Robb P. Matzke 2014-08-29]
    // Remove execute permission from all segments of memory except those with ".text" as part of their name.
    BOOST_FOREACH (MemoryMap::Segment &segment, map.segments()) {
        if (!boost::contains(segment.name(), ".text")) {
            std::cerr <<"ROBB: removing execute from " <<segment.name() <<"\n";
            unsigned newPerms = segment.accessibility() & ~MemoryMap::EXECUTABLE;
            segment.accessibility(newPerms);
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
    Stream info(mlog[INFO] <<"Disassembling and partitioning");
    Sawyer::Stopwatch partitionTime;
    P2::Partitioner partitioner = engine.createTunedPartitioner();
    partitioner.enableSymbolicSemantics(settings.useSemantics);
    partitioner.assumeFunctionsReturn(settings.assumeFunctionsReturn);
    if (settings.followGhostEdges)
        partitioner.basicBlockCallbacks().append(P2::Modules::AddGhostSuccessors::instance());
    if (!settings.allowDiscontiguousBlocks)
        partitioner.basicBlockCallbacks().append(P2::Modules::PreventDiscontiguousBlocks::instance());
    if (false)
        partitioner.cfgAdjustmentCallbacks().append(Monitor::instance());// fun, but very verbose
    if (false)
        makeCallTargetFunctions(partitioner);           // not useful; see documentation at function definition

    // Insert debugging aids
    BOOST_FOREACH (const std::string &s, settings.triggers) {
        if (boost::starts_with(s, "cfg-dot:")) {
            P2::Modules::CfgGraphVizDumper::Ptr aid = P2::Modules::CfgGraphVizDumper::instance(s.substr(8));
            partitioner.cfgAdjustmentCallbacks().append(aid);
        } else if (boost::starts_with(s, "hexdump:")) {
            P2::Modules::HexDumper::Ptr aid = P2::Modules::HexDumper::instance(s.substr(8));
            partitioner.cfgAdjustmentCallbacks().append(aid);
        } else if (boost::starts_with(s, "insn-list:")) {
            P2::Modules::InstructionLister::Ptr aid = P2::Modules::InstructionLister::instance(s.substr(10));
            partitioner.cfgAdjustmentCallbacks().append(aid);
        } else if (boost::starts_with(s, "debugger:")) {
            P2::Modules::Debugger::Ptr aid = P2::Modules::Debugger::instance(s.substr(9));
            partitioner.cfgAdjustmentCallbacks().append(aid);
        } else {
            throw std::runtime_error("invalid debugging aid for \"trigger\" switch: " + s);
        }
    }

    // Show what we'll be working on (stdout for the record, and diagnostics also)
    partitioner.memoryMap().dump(mlog[INFO]);
    if (settings.doShowMap)
        partitioner.memoryMap().dump(std::cout);

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
    if (settings.intraFunctionCode) {
        while (engine.attachSurroundedCodeToFunctions(partitioner)) {
#if 1 // DEBUGGING [Robb P. Matzke 2014-11-06]
            mlog[INFO] <<"ROBB: attached surrounded code to functions\n";
#endif
            engine.discoverBasicBlocks(partitioner);    // discover instructions and more basic blocks by following control
            engine.makeCalledFunctions(partitioner);    // we might have found more function calls from the new blocks
            engine.attachBlocksToFunctions(partitioner);// attach new blocks to functions wherever possible
        }
    }
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

    // Analyze each basic block and function and cache results.  We do this before listing the CFG or building the AST.
    engine.updateAnalysisResults(partitioner);

    info <<"; completed in " <<partitionTime <<" seconds.\n";
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

    if (settings.doListInstructionAddresses) {
        std::vector<P2::BasicBlock::Ptr> bblocks = partitioner.basicBlocks();
        BOOST_FOREACH (const P2::BasicBlock::Ptr &bblock, bblocks) {
            P2::Function::Ptr function = partitioner.findFunctionOwningBasicBlock(bblock);
            BOOST_FOREACH (SgAsmInstruction *insn, bblock->instructions()) {
                std::cout <<StringUtility::addrToString(insn->get_address()) <<"+" <<insn->get_size();
                std::cout <<"\t" <<StringUtility::addrToString(bblock->address());
                std::cout <<"\t" <<(function ? StringUtility::addrToString(function->address()) : std::string("nil")) <<"\n";
            }
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
