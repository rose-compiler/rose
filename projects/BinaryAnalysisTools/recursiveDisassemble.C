#include <rose.h>
#include <rosePublicConfig.h>
#include <BinaryUnparserBase.h>
#include <CommandLine.h>

#include <boost/algorithm/string/predicate.hpp>
#include <AsmFunctionIndex.h>
#include <AsmUnparser.h>
#include <BinaryControlFlow.h>
#include <BinaryLoader.h>
#include <BinaryString.h>
#include <Disassembler.h>
#include <Partitioner2/BasicTypes.h>
#include <Partitioner2/Engine.h>
#include <Partitioner2/GraphViz.h>
#include <Partitioner2/ModulesM68k.h>
#include <Partitioner2/ModulesPe.h>
#include <Partitioner2/Modules.h>
#include <Partitioner2/Utility.h>
#include <rose_strtoull.h>
#include <Sawyer/Assert.h>
#include <Sawyer/CommandLine.h>
#include <Sawyer/ProgressBar.h>
#include <Sawyer/Stopwatch.h>
#include <stringify.h>


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

using namespace Rose;
using namespace Rose::BinaryAnalysis;
using namespace Rose::Diagnostics;

namespace P2 = Partitioner2;

static const rose_addr_t NO_ADDRESS(-1);

enum FunctionSelector {
    ALL_FUNCTIONS,                                      // select all functions
    CALLED_FUNCTIONS,                                   // select called functions
    CONSTADDR_FUNCTIONS,                                // select functions having address mentioned by instructions
};

// Convenient struct to hold settings specific to this tool. Settings related to disassembling are in the engine.
struct Settings {
    bool doListCfg;                                     // list the control flow graph
    bool doListAum;                                     // list the address usage map
    bool doListAsm;                                     // produce an assembly-like listing with AsmUnparser
    bool doListFunctions;                               // produce a function index
    bool doListFunctionAddresses;                       // list function entry addresses
    bool doListInstructionAddresses;                    // show instruction addresses
    bool doListContainer;                               // generate information about the containers if present
    bool doListStrings;                                 // show string constants
    bool doShowMap;                                     // show the memory map
    bool doShowStats;                                   // show some statistics
    bool doListUnused;                                  // list unused addresses
    FunctionSelector selectFunctions;                   // which functions should be shown
    bool selectFunctionsInverted;                       // invert sense of selectFunctions?
    std::vector<std::string> triggers;                  // debugging aids
    std::string gvBaseName;                             // base name for GraphViz files
    bool gvUseFunctionSubgraphs;                        // use subgraphs in GraphViz files?
    bool gvShowInstructions;                            // show disassembled instructions in GraphViz files?
    bool gvShowFunctionReturns;                         // show edges from function return to indeterminate?
    std::vector<std::string> gvCfgFunctions;            // produce CFG dot files for these functions
    bool gvCfgGlobal;                                   // produce GraphViz file containing a global CFG?
    AddressInterval gvCfgInterval;                      // show part of the global CFG
    bool gvCallGraph;                                   // produce a function call graph?
    bool gvInlineImports;                               // inline imports when emitting a call graph?
    bool usingOldUnparser;                              // use the old binary unparser?
    BinaryAnalysis::Unparser::Settings unparserSettings;// what assembly listings should look like
    Settings()
        : doListCfg(false), doListAum(false), doListAsm(true), doListFunctions(false), doListFunctionAddresses(false),
          doListInstructionAddresses(false), doListContainer(false), doListStrings(false), doShowMap(false),
          doShowStats(false), doListUnused(false), selectFunctions(ALL_FUNCTIONS), selectFunctionsInverted(false),
          gvUseFunctionSubgraphs(true), gvShowInstructions(true), gvShowFunctionReturns(false), gvCfgGlobal(false),
          gvCallGraph(false), gvInlineImports(false), usingOldUnparser(false) {}
};

// Describe and parse the command-line
static std::vector<std::string>
parseCommandLine(int argc, char *argv[], P2::Engine &engine, Settings &settings)
{
    using namespace Sawyer::CommandLine;

    std::string purpose = "disassembles binary specimens";
    std::string description =
        "Disassembles the specimens and presents various information depending on switches.";
    Parser parser = engine.commandLineParser(purpose, description);
    parser.errorStream(mlog[FATAL]);

    // Switches for output
    SwitchGroup out("Output switches");
    out.name("out");
    out.doc("These switches control the various ways that this tool produces output. Switches related to GraphViz output "
            "are described in their own section.");

    out.insert(Switch("select-functions")
               .argument("how", enumParser(settings.selectFunctions)
                         ->with("all", ALL_FUNCTIONS)
                         ->with("called", CALLED_FUNCTIONS)
                         ->with("constaddr", CONSTADDR_FUNCTIONS))
               .doc("Determines which functions to use for certain other operations.  The values are \"all\", the default; "
                    "\"called\", to select only those functions that are called; and \"constaddr\", to select those "
                    "functions whose entry address is mentioned as a constant in some instruction.  Any operation that "
                    "uses this filter is documented as such.  See also, @s{select-functions-invert}."));

    out.insert(Switch("select-functions-invert")
               .argument("bool", booleanParser(settings.selectFunctionsInverted), "true")
               .doc("Inverts the sense of the @s{select-functions} switch.  For instance, inverting \"@s{select-functions} "
                    "called\" will select all functions that are not statically called."));

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
                    "show this information."
                    "\n\n"
                    "The may-return column of the results is the output from the true may-return analysis. However, the "
                    "partitioning engine might assume a different value depending on the @s{functions-return} switch."
                    "\n\n"
                    "See also @s{select-functions}."));
    out.insert(Switch("no-list-functions")
               .key("list-functions")
               .intrinsicValue(false, settings.doListFunctions)
               .hidden(true));

    out.insert(Switch("list-function-addresses")
               .intrinsicValue(true, settings.doListFunctionAddresses)
               .doc("Produce a listing of function entry addresses, one address per line in hexadecimal format. Each address "
                    "is followed by the word \"existing\" or \"missing\" depending on whether a non-empty basic block exists "
                    "in the CFG for the function entry address.  The listing is disabled with @s{no-list-function-addresses}. "
                    "See also, @s{select-functions}."));
    out.insert(Switch("no-list-function-addresses")
               .key("list-function-addresses")
               .intrinsicValue(false, settings.doListFunctionAddresses)
               .hidden(true));

    out.insert(Switch("list-instruction-addresses")
               .intrinsicValue(true, settings.doListInstructionAddresses)
               .doc("Produce a listing of instruction addresses.  Each line of output will contain at least two "
                    "space-separated items: the address interval for the instruction (address followed by \"+\" followed by "
                    "size), and the address of the basic block to which the instruction belongs. These fields are followed "
                    "by zero or more function entry addresses for the functions that own the basic block. Basic blocks are "
                    "usually owned by zero or one function.  This listing is disabled with the "
                    "@s{no-list-instruction-addresses} switch.  The default is to " +
                    std::string(settings.doListInstructionAddresses?"":"not ") + "show this information."));
    out.insert(Switch("no-list-instruction-addresses")
               .key("list-instruction-addresses")
               .intrinsicValue(false, settings.doListInstructionAddresses)
               .hidden(true));

    out.insert(Switch("list-strings")
               .intrinsicValue(true, settings.doListStrings)
               .doc("Produce a listing of all string constants.  The listing is disabled with the @s{no-list-strings} "
                    "switch. The default is to " + std::string(settings.doListStrings?"":"not ") +
                    "show this information."));
    out.insert(Switch("no-list-strings")
               .key("list-strings")
               .intrinsicValue(false, settings.doListStrings)
               .hidden(true));

    out.insert(Switch("list-unused-addresses")
               .intrinsicValue(true, settings.doListUnused)
               .doc("Produce a listing of all specimen addresses that are not represented in the control flow graph. This "
                    "listing can be disabled with @s{no-list-unused-addresses}."));
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

    out.insert(Switch("list-container")
               .intrinsicValue(true, settings.doListContainer)
               .doc("Emit detailed information about all binary containers (ELF, PE, etc).  The @s{no-list-container} "
                    "switch turns this off. The default is to " + std::string(settings.doListContainer?"":"not ") +
                    "show this information.\n"));
    out.insert(Switch("no-list-container")
               .key("list-container")
               .intrinsicValue(false, settings.doListContainer)
               .hidden(true));

    CommandLine::insertBooleanSwitch(out, "old-unparser", settings.usingOldUnparser,
                                     "Use the old assembly listing format instead of the more modern one. This switch "
                                     "is mostly here for backward compatibility when @prop{programName} is called by "
                                     "shell scripts and testing harnesses.");

    // Assembly output
    SwitchGroup ass = BinaryAnalysis::Unparser::commandLineSwitches(settings.unparserSettings);
    ass.name("assembly");
    ass.doc("These switches control the assembly listing output, but they're ignored if @s{old-unparser} is specified.");

    // Switches controlling GraphViz output
    SwitchGroup dot("Graphviz switches");
    dot.name("gv");
    dot.doc("Switches that affect GraphViz output for control flow graphs (CFG) and function call graphs (CG).");

    dot.insert(Switch("basename")
               .argument("path", anyParser(settings.gvBaseName))
               .doc("Base name for GraphViz dot files.  The full name is created by appending details about what is "
                    "contained in the file.  For instance, a control flow graph for the function \"main\" has the "
                    "string \"cfg-main.dot\" appended.  The default is \"" + settings.gvBaseName + "\"."));

    dot.insert(Switch("subgraphs")
               .intrinsicValue(true, settings.gvUseFunctionSubgraphs)
               .doc("Organize GraphViz output into subgraphs, one per function.  The @s{no-subgraphs} switch disables "
                    "subgraphs. The default is to " + std::string(settings.gvUseFunctionSubgraphs?"":"not ") + "emit "
                    "subgraphs for those GraphViz files where it makes sense."));
    dot.insert(Switch("no-subgraphs")
               .key("subgraphs")
               .intrinsicValue(false, settings.gvUseFunctionSubgraphs)
               .hidden(true));

    dot.insert(Switch("show-insns")
               .intrinsicValue(true, settings.gvShowInstructions)
               .doc("Show disassembled instructions in the GraphViz output rather than only starting addresses. Emitting "
                    "just addresses makes the GraphViz files much smaller but requires a separate assembly listing to "
                    "interpret the graphs.  The @s{no-show-insns} causes only addresses to be emitted.  The "
                    "default is to emit " + std::string(settings.gvShowInstructions?"instructions":"only addresses") + "."));
    dot.insert(Switch("no-show-insns")
               .key("show-insns")
               .intrinsicValue(false, settings.gvShowInstructions)
               .hidden(true));

    dot.insert(Switch("show-funcret")
               .intrinsicValue(true, settings.gvShowFunctionReturns)
               .doc("Show the function return edges in control flow graphs. These are the edges originating at a basic block "
                    "that serves as a function return and usually lead to the indeterminate vertex.  Including them in "
                    "multi-function graphs makes the graphs more complicated than they need to be for visualization. The "
                    "@s{no-show-funcret} switch disables these edges. The default is to " +
                    std::string(settings.gvShowFunctionReturns?"":"not ") + "show these edges."));
    dot.insert(Switch("no-show-funcret")
               .key("show-funcret")
               .intrinsicValue(false, settings.gvShowFunctionReturns)
               .hidden(true));

    dot.insert(Switch("cfg-function")
               .argument("name", listParser(anyParser(settings.gvCfgFunctions)))
               .explosiveLists(true)
               .whichValue(SAVE_ALL)
               .doc("Emits a function control flow graph. The @v{name} can be the name of a function as a string, the "
                    "entry address for the function as an decimal, octal, or hexadecimal number, or the string \"all\" "
                    "(they are matched in that order).  One file will be created for each output and the name of the file "
                    "is constructed by appending the following hyphen-separated parts to the GraphViz base name specified "
                    "with @s{basename}: the string \"cfg\", the hexadecimal entry address for the function, the name "
                    "of the function with special characters replaced by underscores, and the string \".dot\".  This switch "
                    "may occur multiple times or multiple @v{name} values may be separated by commas."));

    dot.insert(Switch("cfg-global")
               .intrinsicValue(true, settings.gvCfgGlobal)
               .doc("Emits a global control flow graph saving it in a file whose name is the @s{basename} suffixed with "
                    "the string \"cfg-global.dot\". The @s{no-cfg-global} switch disables this. The default is to " +
                    std::string(settings.gvCfgGlobal?"":"not ") + "produce this file."));
    dot.insert(Switch("no-cfg-global")
               .key("cfg-global")
               .intrinsicValue(false, settings.gvCfgGlobal)
               .hidden(true));

    dot.insert(Switch("cfg-interval")
               .argument("interval", P2::addressIntervalParser(settings.gvCfgInterval))
               .doc("Emits a control flow graph for those basic blocks that begin within the specified interval. " +
                    P2::AddressIntervalParser::docString() + " The name of the GraphViz file will be the prefix "
                    "specified via @s{basename} followed by the following hyphen-separated components: "
                    "the string \"cfg\", the interval starting address in hexadecimal, and the interval inclusive final "
                    "address in hexadecimal. The extension \".dot\" is appended."));

    dot.insert(Switch("call-graph")
               .intrinsicValue(true, settings.gvCallGraph)
               .doc("Emit a function call graph to the GraphViz file whose name is specified by the @s{basename} prefix "
                    "followed by the string \"cg.dot\". The @s{no-call-graph} switch disables this output. The default "
                    "is to " + std::string(settings.gvCallGraph?"":"not ") + "produce this file.\n"));
    dot.insert(Switch("no-call-graph")
               .key("call-graph")
               .intrinsicValue(false, settings.gvCallGraph)
               .hidden(true));

    dot.insert(Switch("inline-imports")
               .intrinsicValue(true, settings.gvInlineImports)
               .doc("When emitting a function call graph, inline imports into their callers and display the names of inlined "
                    "functions in the output.  This sometimes makes the output much cleaner.  Import functions are identified "
                    "by their names only: any name ending with \".dll\" or \"@@plt\" is considered an imported function. This "
                    "feature is disabled with the @s{no-inline-imports} switch.  The default is to " +
                    std::string(settings.gvInlineImports?"":"not ") + "perform this inlining."));
    dot.insert(Switch("no-inline-imports")
               .key("inline-imports")
               .intrinsicValue(false, settings.gvInlineImports)
               .hidden(true));

    // Switches for debugging
    SwitchGroup dbg("Debugging switches");
    dbg.name("debug");
    dbg.doc("These debugging switches are intended mostly for ROSE developers and direct users of the ROSE library. "
            "Interpretation of the results often requires considerable knowledge of implementation details.");

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
                    "Debugging aids generally send their output to the Rose::BinaryAnalysis::Partitioner2[DEBUG] stream. "
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
    

    return parser.with(out).with(ass).with(dot).with(dbg).parse(argc, argv).apply().unreachedArgs();
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
// Rose::BinaryAnalysis::Partitioner2::Engine.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Make functions for any x86 CALL instruction. This is intended to be a demonstration of how to search for specific
// instruction patterns and do something when the pattern is found, and probably isn't all that useful since function calls in
// reachable code are already detected anyway as part of Partitioner::discoverBasicBlock.
void
makeCallTargetFunctions(P2::Partitioner &partitioner, size_t alignment=1) {
    std::set<rose_addr_t> targets;                      // distinct call targets

    // Iterate over every executable address in the memory map
    ASSERT_not_null(partitioner.memoryMap());
    for (rose_addr_t va=0; partitioner.memoryMap()->atOrAfter(va).require(MemoryMap::EXECUTABLE).next().assignTo(va); ++va) {
        if (alignment>1)                                // apply alignment here as an optimization
            va = ((va+alignment-1)/alignment)*alignment;

        // Disassemble an instruction (or get one that was previously disassembled) and see if it's a function call. The
        // function call detection normally operates at a basic block level, so we make a singleton basic block since we're
        // interested only in single instructions.
        if (SgAsmInstruction *insn = partitioner.discoverInstruction(va)) {
            std::vector<SgAsmInstruction*> bb(1, insn);
            rose_addr_t target = NO_ADDRESS;
            if (insn->isFunctionCallFast(bb, &target, NULL) &&
                partitioner.memoryMap()->at(target).require(MemoryMap::EXECUTABLE).exists()) {
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
//                                      GraphViz output
//
// These functions are for producing GraphViz output for things like control flow graphs.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Replaces characters that can't appear in a file name component with underscores.  If the whole return string would be
// underscores then return the empty string instead.
static std::string
escapeFileNameComponent(const std::string &s) {
    std::string retval;
    bool hasNonUnderscore = false;
    BOOST_FOREACH (char ch, s) {
        if (isalnum(ch) || strchr("@$%=+:,.", ch)) {
            retval += ch;
            hasNonUnderscore = true;
        } else {
            retval += '_';
        }
    }
    return hasNonUnderscore ? retval : std::string();
}

static std::string
makeGraphVizFileName(const std::string &prefix, const std::string &p1, const P2::Function::Ptr &function) {
    std::vector<std::string> parts;
    parts.push_back(p1);
    parts.push_back(StringUtility::addrToString(function->address()).substr(2));
    parts.push_back(escapeFileNameComponent(function->name()));
    parts.erase(std::remove(parts.begin(), parts.end(), std::string()), parts.end());
    return prefix + StringUtility::join("-", parts) + ".dot";
}

static std::string
makeGraphVizFileName(const std::string &prefix, const std::string &p1, const AddressInterval &interval) {
    std::vector<std::string> parts;
    if (!p1.empty())
        parts.push_back(p1);
    parts.push_back(StringUtility::addrToString(interval.least()).substr(2));
    parts.push_back(StringUtility::addrToString(interval.greatest()).substr(2));
    return prefix + StringUtility::join("-", parts) + ".dot";
}

static void
emitControlFlowGraphs(const P2::Partitioner &partitioner, const Settings &settings) {
    // Get a list of functions that need to be emitted so that we don't produce multiple files per function.
    std::set<P2::Function::Ptr> selectedFunctions;
    if (!settings.gvCfgFunctions.empty()) {
        std::vector<P2::Function::Ptr> allFunctions = partitioner.functions();
        BOOST_FOREACH (const std::string &specified, settings.gvCfgFunctions) {
            bool inserted = false;
            BOOST_FOREACH (const P2::Function::Ptr &function, allFunctions) {
                if (function->name() == specified) {
                    selectedFunctions.insert(function);
                    inserted = true;
                    break;
                }
            }
            if (!inserted) {
                errno = 0;
                const char *s = specified.c_str();
                char *rest = NULL;
                rose_addr_t specifiedVa = rose_strtoull(s, &rest, 0);
                if (0==errno && *rest=='\0') {
                    BOOST_FOREACH (const P2::Function::Ptr &function, allFunctions) {
                        if (function->address() == specifiedVa) {
                            selectedFunctions.insert(function);
                            inserted = true;
                            break;
                        }
                    }
                }
            }
            if (!inserted && specified=="all") {
                selectedFunctions.insert(allFunctions.begin(), allFunctions.end());
                break;
            }
            if (!inserted)
                mlog[WARN] <<"no such function for CFG: " <<specified <<"\n";
        }
    }
    
    BOOST_FOREACH (const P2::Function::Ptr &function, selectedFunctions) {
        std::string fileName = makeGraphVizFileName(settings.gvBaseName, "cfg", function);
        std::ofstream out(fileName.c_str());
        if (out.fail()) {
            mlog[ERROR] <<"cannot write to CFG file \"" <<fileName <<"\"\n";
        } else {
            mlog[INFO] <<"generating CFG GraphViz file: " <<fileName <<"\n";
            P2::GraphViz::CfgEmitter gv(partitioner);
            gv.defaultGraphAttributes().insert("overlap", "scale");
            gv.useFunctionSubgraphs(false);             // since we're dumping only one function
            gv.showInstructions(settings.gvShowInstructions);
            gv.showReturnEdges(settings.gvShowFunctionReturns);
            gv.showInNeighbors(true);
            gv.showOutNeighbors(true);
            gv.emitFunctionGraph(out, function);
        }
    }

    if (settings.gvCfgGlobal) {
        std::string fileName = settings.gvBaseName + "cfg-global.dot";
        std::ofstream out(fileName.c_str());
        if (out.fail()) {
            mlog[ERROR] <<"cannot write to CFG file \"" <<fileName <<"\"\n";
        } else {
            mlog[INFO] <<"generating CFG GraphViz file: " <<fileName <<"\n";
            P2::GraphViz::CfgEmitter gv(partitioner);
            gv.defaultGraphAttributes().insert("overlap", "scale");
            gv.useFunctionSubgraphs(settings.gvUseFunctionSubgraphs);
            gv.showInstructions(settings.gvShowInstructions);
            gv.showReturnEdges(settings.gvShowFunctionReturns);
            gv.emitWholeGraph(out);
        }
    }

    if (!settings.gvCfgInterval.isEmpty()) {
        std::string fileName = makeGraphVizFileName(settings.gvBaseName, "cfg", settings.gvCfgInterval);
        std::ofstream out(fileName.c_str());
        if (out.fail()) {
            mlog[ERROR] <<"cannot write to CFG file \"" <<fileName <<"\"\n";
        } else {
            mlog[INFO] <<"generating CFG GraphViz file: " <<fileName <<"\n";
            P2::GraphViz::CfgEmitter gv(partitioner);
            gv.defaultGraphAttributes().insert("overlap", "scale");
            gv.useFunctionSubgraphs(settings.gvUseFunctionSubgraphs);
            gv.showInstructions(settings.gvShowInstructions);
            gv.showReturnEdges(settings.gvShowFunctionReturns);
            gv.emitIntervalGraph(out, settings.gvCfgInterval);
        }
    }
}

static void
emitFunctionCallGraph(const P2::Partitioner &partitioner, const Settings &settings) {
    if (!settings.gvCallGraph)
        return;
    std::string fileName = settings.gvBaseName + "cg.dot";
    std::ofstream out(fileName.c_str());
    if (out.fail()) {
        mlog[ERROR] <<"cannot write to CG file \"" <<fileName <<"\"\n";
    } else {
        mlog[INFO] <<"generating call graph: " <<fileName <<"\n";
        if (settings.gvInlineImports) {
            P2::GraphViz::CgInlinedEmitter gv(partitioner, boost::regex("(\\.dll|@plt)$"));
            gv.highlight(boost::regex("."));            // highlight anything with a name
            gv.defaultGraphAttributes().insert("overlap", "scale");
            gv.emitCallGraph(out);
        } else {
            P2::GraphViz::CgEmitter gv(partitioner);
            gv.defaultGraphAttributes().insert("overlap", "scale");
            gv.emitCallGraph(out);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Analysis
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Convert partitioner data to ROSE AST.
static SgAsmBlock *
buildAst(P2::Engine &engine, const P2::Partitioner &partitioner) {
    static SgAsmBlock *gblock = NULL;
    if (NULL==gblock)
        gblock = P2::Modules::buildAst(partitioner, engine.interpretation(), engine.settings().astConstruction);
    return gblock;
}

// Returns all constants found in instructions.
static std::set<rose_addr_t>
findInstructionConstants(P2::Engine &engine, const P2::Partitioner &partitioner) {
    struct T1: AstSimpleProcessing {
        std::set<rose_addr_t> constants;
        void visit(SgNode *node) {
            if (SgAsmIntegerValueExpression *ive = isSgAsmIntegerValueExpression(node))
                constants.insert(ive->get_absoluteValue());
        }
    } t1;
    t1.traverse(buildAst(engine, partitioner), preorder);
    return t1.constants;
}

// Returns functions that have callers (or not).
static std::vector<P2::Function::Ptr>
findCalledFunctions(const P2::Partitioner &partitioner, bool selectCalledFunctions) {
    std::vector<P2::Function::Ptr> retval;
    P2::FunctionCallGraph cg = partitioner.functionCallGraph(P2::AllowParallelEdges::NO);
    BOOST_FOREACH (const P2::Function::Ptr &function, partitioner.functions()) {
        bool isCalled = !cg.callers(function).empty();
        if ((selectCalledFunctions && isCalled) || (!selectCalledFunctions && !isCalled))
            retval.push_back(function);
    }
    return retval;
}

// Returns a list of selected functions.
static std::vector<P2::Function::Ptr>
selectFunctions(P2::Engine &engine, const P2::Partitioner &partitioner, const Settings &settings) {
    std::vector<P2::Function::Ptr> retval;
    switch (settings.selectFunctions) {
        case ALL_FUNCTIONS:
            return settings.selectFunctionsInverted ? retval : partitioner.functions();
        case CALLED_FUNCTIONS:
            return findCalledFunctions(partitioner, !settings.selectFunctionsInverted);
        case CONSTADDR_FUNCTIONS: {
            std::set<rose_addr_t> constants = findInstructionConstants(engine, partitioner);
            BOOST_FOREACH (const P2::Function::Ptr &function, partitioner.functions()) {
                bool isConstant = constants.find(function->address()) != constants.end();
                if ((isConstant && !settings.selectFunctionsInverted) || (!isConstant && settings.selectFunctionsInverted))
                    retval.push_back(function);
            }
            return retval;
        }
    }
    ASSERT_not_implemented("function selection criteria is not implemented yet");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]) {
    ROSE_INITIALIZE;                                    // see Rose::initialize

    // Use a partitioning engine since this makes this tool much easier to write.
    P2::Engine engine;

    // Parse the command-line
    Settings settings;
    std::vector<std::string> specimenNames = parseCommandLine(argc, argv, engine, settings);
    if (specimenNames.empty())
        throw std::runtime_error("no specimen specified; see --help");

    // Load the specimen as raw data or an ELF or PE container.
    engine.loadSpecimens(specimenNames);
    SgAsmInterpretation *interp = engine.interpretation();

    // Some analyses need to know what part of the address space is being disassembled.
    ASSERT_not_null(engine.memoryMap());
    AddressIntervalSet executableSpace;
    BOOST_FOREACH (const MemoryMap::Node &node, engine.memoryMap()->nodes()) {
        if ((node.value().accessibility() & MemoryMap::EXECUTABLE)!=0)
            executableSpace.insert(node.key());
    }

    // Create a partitioner that's tuned for a certain architecture, and then tune it even more depending on our command-line.
    P2::Partitioner partitioner = engine.createPartitioner();
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
    partitioner.memoryMap()->dump(mlog[INFO]);
    if (settings.doShowMap)
        partitioner.memoryMap()->dump(std::cout);

    // Run the partitioner
    bool mustDisassemble = settings.doListCfg || settings.doListAum || settings.doListAsm || settings.doListFunctions ||
                           settings.doListFunctionAddresses || settings.doListInstructionAddresses ||
                           settings.doShowStats || settings.doListUnused || !settings.triggers.empty() ||
                           !settings.gvCfgFunctions.empty() || settings.gvCfgGlobal || !settings.gvCfgInterval.isEmpty() ||
                           settings.gvCallGraph;

    if (mustDisassemble) {
        engine.runPartitioner(partitioner);
        if (partitioner.functions().empty() && engine.startingVas().empty())
            mlog[WARN] <<"no starting points for recursive disassembly; perhaps you need --start?\n";
    }

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

    std::vector<P2::Function::Ptr> selectedFunctions = selectFunctions(engine, partitioner, settings);

    if (settings.doListCfg) {
        std::cout <<"Final control flow graph:\n";
        partitioner.dumpCfg(std::cout, "  ", true);
    }

    emitControlFlowGraphs(partitioner, settings);
    emitFunctionCallGraph(partitioner, settings);

    // List selected functions.  This is a bit convoluted because the function lister (AsmFunctionIndex) operates on an AST,
    // but our function selection uses Partitioner2 data structures.
    if (settings.doListFunctions) {
        std::set<rose_addr_t> selectedVas;
        BOOST_FOREACH (const P2::Function::Ptr &function, selectedFunctions)
            selectedVas.insert(function->address());
        AsmFunctionIndex index;
        struct T1: AstSimpleProcessing {
            AsmFunctionIndex &index;
            const std::set<rose_addr_t> &selectedVas;
            T1(AsmFunctionIndex &index, const std::set<rose_addr_t> &selectedVas): index(index), selectedVas(selectedVas) {}
            void visit(SgNode *node) {
                if (SgAsmFunction *function = isSgAsmFunction(node)) {
                    if (selectedVas.find(function->get_entry_va()) != selectedVas.end())
                        index.add_function(function);
                }
            }
        } t1(index, selectedVas);
        t1.traverse(buildAst(engine, partitioner), preorder);
        std::cout <<index;
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
        BOOST_FOREACH (P2::Function::Ptr function, selectedFunctions) {
            rose_addr_t entryVa = function->address();
            P2::BasicBlock::Ptr bb = partitioner.basicBlockExists(entryVa);
            std::cout <<partitioner.functionName(function) <<": "
                      <<(bb && !bb->isEmpty() ? "exists" : "missing") <<"\n";
        }
    }

    if (settings.doListInstructionAddresses) {
        std::vector<P2::BasicBlock::Ptr> bblocks = partitioner.basicBlocks();
        BOOST_FOREACH (const P2::BasicBlock::Ptr &bblock, bblocks) {
            std::vector<P2::Function::Ptr> functions = partitioner.functionsOwningBasicBlock(bblock);
            BOOST_FOREACH (SgAsmInstruction *insn, bblock->instructions()) {
                std::cout <<StringUtility::addrToString(insn->get_address()) <<"+" <<insn->get_size();
                std::cout <<"\t" <<StringUtility::addrToString(bblock->address());
                BOOST_FOREACH (const P2::Function::Ptr &function, functions)
                    std::cout <<"\t" <<StringUtility::addrToString(function->address());
                std::cout <<"\n";
            }
        }
    }

    if (settings.doListStrings) {
        Strings::StringFinder analyzer;
        analyzer.settings().minLength = 1;
        analyzer.settings().maxLength = 8192;
        analyzer.settings().keepingOnlyLongest = true;
        analyzer.discardingCodePoints(false);
        analyzer.insertCommonEncoders(ByteOrder::ORDER_LSB);
        analyzer.find(partitioner.memoryMap()->any());
        BOOST_FOREACH (const Strings::EncodedString &string, analyzer.strings()) {
            std::cout <<string.where() <<" " <<string.encoder()->length() <<"-character " <<string.encoder()->name() <<"\n";
            std::cout <<"  \"" <<StringUtility::cEscape(string.narrow()) <<"\"\n";
        }
    }
    
    if (settings.doListContainer && interp) {
        std::set<SgAsmGenericFile*> emittedFiles;
        BOOST_FOREACH (SgAsmGenericHeader *fileHeader, interp->get_headers()->get_headers()) {
            SgAsmGenericFile *container = SageInterface::getEnclosingNode<SgAsmGenericFile>(fileHeader);
            if (emittedFiles.insert(container).second) {
                container->dump(stdout);
                int i=0;
                BOOST_FOREACH (SgAsmGenericSection *section, container->get_sections()) {
                    printf("Section [%d]:\n", i++);
                    section->dump(stdout, "  ", -1);
                }
            }
        }
    }
    

    // Build the AST and unparse it.
    if (settings.doListAsm) {
        if (settings.usingOldUnparser) {
            SgAsmBlock *gblock = buildAst(engine, partitioner);
            AsmUnparser unparser;
            unparser.set_registers(partitioner.instructionProvider().registerDictionary());
            unparser.add_control_flow_graph(ControlFlow().build_block_cfg_from_ast<ControlFlow::BlockGraph>(gblock));
            unparser.staticDataDisassembler.init(engine.disassembler());
            unparser.unparse(std::cout, gblock);
        } else {
            BinaryAnalysis::Unparser::Base::Ptr unparser = partitioner.unparser();
            unparser->settings() = settings.unparserSettings;
            unparser->unparse(std::cout, partitioner);
        }
    }

    
    // Test what affect shared instructions have on the AST by counting how many times each instruction appears in the AST.  If
    // instruction copying is enabled, they should all occur once; if not, then there may be instructions that occur multiple
    // times (but always having the same parent pointer).
    if (0) {
        struct InsnCounter: AstSimpleProcessing {
            typedef Sawyer::Container::Map<SgAsmInstruction*, size_t> InsnCount;
            InsnCount insnCount;
            void visit(SgNode *node) {
                if (SgAsmInstruction *insn = isSgAsmInstruction(node))
                    ++insnCount.insertMaybe(insn, 0);
            }
        } insnCounter;
        insnCounter.traverse(buildAst(engine, partitioner), preorder);
        BOOST_FOREACH (const InsnCounter::InsnCount::Node &node, insnCounter.insnCount.nodes())
            std::cout <<node.value() <<"\t" <<unparseInstructionWithAddress(node.key()) <<"\n";
    }

#if 0 // [Robb P. Matzke 2015-08-06]: example of calling convention analysis
    {
        const CallingConvention::Dictionary &dictionary = partitioner.instructionProvider().callingConventions();
        const CallingConvention::Definition *dfltCc = dictionary.empty() ? NULL : &dictionary.front();
        partitioner.allFunctionCallingConvention(dfltCc);
        BOOST_FOREACH (const P2::Function::Ptr &function, partitioner.functions()) {
            std::cerr <<"calling conventions for " <<function->printableName() <<":\n";
            const CallingConvention::Analysis &ccAnalysis = function->callingConventionAnalysis();
            if (ccAnalysis.hasResults()) {
                if (!ccAnalysis.didConverge())
                    std::cerr <<"  warning: non-convergent analysis\n";
                CallingConvention::Dictionary matches = ccAnalysis.match(dictionary);
                if (matches.empty()) {
                    std::cerr <<"  no maches; analysis reports " <<ccAnalysis <<"\n";
                } else {
                    BOOST_FOREACH (const CallingConvention::Definition &cc, matches)
                        std::cerr <<"  " <<cc.comment() <<"\n";
                }
            } else {
                std::cerr <<"  no analysis results\n";
            }
        }
    }
#endif
    
#if 0 // DEBUGGING [Robb P. Matzke 2014-08-23]
    // This should free all symbolic expressions except for perhaps a few held by something we don't know about.
    partitioner.clear();
    SymbolicExpr::Node::poolAllocator().showInfo(std::cerr);
    std::cerr <<"all done; entering busy loop\n";
    while (1);                                          // makes us easy to find in process listings
#endif
}
