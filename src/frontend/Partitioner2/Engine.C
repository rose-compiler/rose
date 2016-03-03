#include "sage3basic.h"
#include "rosePublicConfig.h"

#include "BinaryDebugger.h"
#include "BinaryLoader.h"
#include "Diagnostics.h"
#include "DisassemblerM68k.h"
#include "DisassemblerX86.h"
#include "SRecord.h"
#include <Partitioner2/Engine.h>
#include <Partitioner2/Modules.h>
#include <Partitioner2/ModulesElf.h>
#include <Partitioner2/ModulesM68k.h>
#include <Partitioner2/ModulesPe.h>
#include <Partitioner2/ModulesX86.h>
#include <Partitioner2/Semantics.h>
#include <Partitioner2/Utility.h>
#include <Sawyer/GraphTraversal.h>
#include <Sawyer/Stopwatch.h>

#ifdef ROSE_HAVE_LIBYAML
#include <yaml-cpp/yaml.h>
#endif

using namespace rose::Diagnostics;

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Utility functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
Engine::init() {
    Diagnostics::initialize();
}

void
Engine::reset() {
    interp_ = NULL;
    binaryLoader_ = NULL;
    disassembler_ = NULL;
    map_.clear();
    basicBlockWorkList_ = BasicBlockWorkList::instance(this);
}

// Returns true if the specified vertex has at least one E_CALL_RETURN edge
static bool
hasCallReturnEdges(const ControlFlowGraph::ConstVertexIterator &vertex) {
    BOOST_FOREACH (const ControlFlowGraph::Edge &edge, vertex->outEdges()) {
        if (edge.value().type() == E_CALL_RETURN)
            return true;
    }
    return false;
}

// True if any callee may-return is positive; false if all callees are negative; indeterminate if any are indeterminate
static boost::logic::tribool
hasAnyCalleeReturn(const Partitioner &partitioner, const ControlFlowGraph::ConstVertexIterator &caller) {
    bool hasIndeterminateCallee = false;
    for (ControlFlowGraph::ConstEdgeIterator edge=caller->outEdges().begin(); edge != caller->outEdges().end(); ++edge) {
        if (edge->value().type() == E_FUNCTION_CALL) {
            bool mayReturn = false;
            if (!partitioner.basicBlockOptionalMayReturn(edge->target()).assignTo(mayReturn)) {
                hasIndeterminateCallee = true;
            } else if (mayReturn) {
                return true;
            }
        }
    }
    if (hasIndeterminateCallee)
        return boost::logic::indeterminate;
    return false;
}
    
// Increment the address as far as possible while avoiding overflow.
static rose_addr_t
incrementAddress(rose_addr_t va, rose_addr_t amount, rose_addr_t maxaddr) {
    if (maxaddr - va < amount)
        return maxaddr;
    return va + amount;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Top-level, do everything functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SgAsmBlock*
Engine::frontend(int argc, char *argv[], const std::string &purpose, const std::string &description) {
    std::vector<std::string> args;
    for (int i=1; i<argc; ++i)
        args.push_back(argv[i]);
    return frontend(args, purpose, description);
}

SgAsmBlock*
Engine::frontend(const std::vector<std::string> &args, const std::string &purpose, const std::string &description) {
    std::vector<std::string> specimenNames = parseCommandLine(args, purpose, description).unreachedArgs();
    if (specimenNames.empty())
        throw std::runtime_error("no binary specimen specified; see --help");
    return buildAst(specimenNames);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Command-line parsing
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Sawyer::CommandLine::SwitchGroup
Engine::loaderSwitches() {
    using namespace Sawyer::CommandLine;
    SwitchGroup sg("Loader switches");

    sg.insert(Switch("remove-zeros")
              .argument("size", nonNegativeIntegerParser(settings_.loader.deExecuteZeros), "128")
              .doc("This switch causes execute permission to be removed from sequences of contiguous zero bytes. The "
                   "switch argument is the minimum number of consecutive zeros that will trigger the removal, and "
                   "defaults to 128.  An argument of zero disables the removal.  When this switch is not specified at "
                   "all, this tool assumes a value of " +
                   StringUtility::plural(settings_.loader.deExecuteZeros, "bytes") + "."));

    sg.insert(Switch("executable")
              .intrinsicValue(true, settings_.loader.memoryIsExecutable)
              .doc("Adds execute permission to the entire memory map, aside from regions excluded by @s{remove-zeros}. "
                   "The executable bit determines whether the partitioner is allowed to make instructions at some address, "
                   "so using this switch is an easy way to make the disassembler think that all of memory may contain "
                   "instructions.  The default is to not add executable permission to all of memory."));
    sg.insert(Switch("no-executable")
              .key("executable")
              .intrinsicValue(false, settings_.loader.memoryIsExecutable)
              .hidden(true));

    sg.insert(Switch("data")
              .argument("state", enumParser<MemoryDataAdjustment>(settings_.loader.memoryDataAdjustment)
                        ->with("constant", DATA_IS_CONSTANT)
                        ->with("initialized", DATA_IS_INITIALIZED)
                        ->with("default", DATA_NO_CHANGE))
              .doc("Globally adjusts the memory map to influence how the partitioner treats reads from concrete memory "
                   "addresses.  The values for @v{state} are one of these words:"
                   "@named{constant}{Causes write access to be removed from all memory segments and the partitioner treats "
                   "memory reads as returning a concrete value." +
                   std::string(DATA_IS_CONSTANT==settings_.loader.memoryDataAdjustment?" This is the default.":"") + "}"
                   "@named{initialized}{Causes the initialized bit to be added to all memory segments and the partitioner "
                   "treats reads from such addresses to return a concrete value, plus if the address is writable, "
                   "indeterminate values." +
                   std::string(DATA_IS_INITIALIZED==settings_.loader.memoryDataAdjustment?" This is the default.":"") + "}"
                   "@named{default}{Causes the engine to not change data access bits for memory." +
                   std::string(DATA_NO_CHANGE==settings_.loader.memoryDataAdjustment?" This is the default.":"") + "}"
                   "One of the things influenced by these access flags is indirect jumps, like x86 \"jmp [@v{addr}]\". If "
                   "@v{addr} is constant memory, then the \"jmp\" has a single constant successor; if @v{addr} is "
                   "non-constant but initialized, then the \"jmp\" will have a single constant successor and indeterminate "
                   "successors; otherwise it will have only indeterminate successors."));

    return sg;
}

Sawyer::CommandLine::SwitchGroup
Engine::disassemblerSwitches() {
    using namespace Sawyer::CommandLine;
    SwitchGroup sg("Disassembler switches");

    sg.insert(Switch("isa")
              .argument("architecture", anyParser(settings_.disassembler.isaName))
              .doc("Name of instruction set architecture.  If no name is specified then the architecture is obtained from "
                   "the binary container (ELF, PE). A list of valid architecture names can be obtained by specifying "
                   "\"list\" as the name."));

    return sg;
}

Sawyer::CommandLine::SwitchGroup
Engine::partitionerSwitches() {
    using namespace Sawyer::CommandLine;
    SwitchGroup sg("Partitioner switches");

    sg.insert(Switch("start")
              .argument("addresses", listParser(nonNegativeIntegerParser(settings_.partitioner.startingVas)))
              .whichValue(SAVE_ALL)
              .explosiveLists(true)
              .doc("List of addresses where recursive disassembly should start in addition to addresses discovered by "
                   "other methods. Each address listed by this switch will be considered the entry point of a function. "
                   "This switch may appear multiple times, each of which may have multiple comma-separated addresses."));

    sg.insert(Switch("use-semantics")
              .intrinsicValue(true, settings_.partitioner.usingSemantics)
              .doc("The partitioner can either use quick and naive methods of determining instruction characteristics, or "
                   "it can use slower but more accurate methods, such as symbolic semantics.  This switch enables use of "
                   "the slower symbolic semantics, or the feature can be disabled with @s{no-use-semantics}. The default is " +
                   std::string(settings_.partitioner.usingSemantics?"true":"false") + "."));
    sg.insert(Switch("no-use-semantics")
              .key("use-semantics")
              .intrinsicValue(false, settings_.partitioner.usingSemantics)
              .hidden(true));

    sg.insert(Switch("semantic-memory")
              .argument("type", enumParser<SemanticMemoryParadigm>(settings_.partitioner.semanticMemoryParadigm)
                        ->with("list", LIST_BASED_MEMORY)
                        ->with("map", MAP_BASED_MEMORY))
              .doc("The partitioner can switch between storing semantic memory states in a list versus a map.  The @v{type} "
                   "should be one of these words:"

                   "@named{list}{List-based memory stores memory cells (essentially address+value pairs) in a reverse "
                   "chronological list and uses an SMT solver (when one is configured and enabled) to solve aliasing "
                   "equations.  The number of symbolic expression comparisons (either within ROSE or using an SMT solver) "
                   "is linear with the size of the memory cell list.}"

                   "@named{map}{Map-based memory stores memory cells in a container hashed by address expression. Aliasing "
                   "equations are not solved even when an SMT solver is available. One cell aliases another only if their "
                   "address expressions are identical. This approach is faster but less precise.}"

                   "The default is to use the " +
                   std::string(LIST_BASED_MEMORY == settings_.partitioner.semanticMemoryParadigm ? "list" : "map") +
                   "-based paradigm."));

    sg.insert(Switch("follow-ghost-edges")
              .intrinsicValue(true, settings_.partitioner.followingGhostEdges)
              .doc("When discovering the instructions for a basic block, treat instructions individually rather than "
                   "looking for opaque predicates.  The @s{no-follow-ghost-edges} switch turns this off.  The default "
                   "is " + std::string(settings_.partitioner.followingGhostEdges?"true":"false") + "."));
    sg.insert(Switch("no-follow-ghost-edges")
              .key("follow-ghost-edges")
              .intrinsicValue(false, settings_.partitioner.followingGhostEdges)
              .hidden(true));

    sg.insert(Switch("allow-discontiguous-blocks")
              .intrinsicValue(true, settings_.partitioner.discontiguousBlocks)
              .doc("This setting allows basic blocks to contain instructions that are discontiguous in memory as long as "
                   "the other requirements for a basic block are still met. Discontiguous blocks can be formed when a "
                   "compiler fails to optimize away an opaque predicate for a conditional branch, or when basic blocks "
                   "are scattered in memory by the introduction of unconditional jumps.  The @s{no-allow-discontiguous-blocks} "
                   "switch disables this feature and can slightly improve partitioner performance by avoiding cases where "
                   "an unconditional branch initially creates a larger basic block which is later discovered to be "
                   "multiple blocks.  The default is to " +
                   std::string(settings_.partitioner.discontiguousBlocks?"":"not ") +
                   "allow discontiguous basic blocks."));
    sg.insert(Switch("no-allow-discontiguous-blocks")
              .key("allow-discontiguous-blocks")
              .intrinsicValue(false, settings_.partitioner.discontiguousBlocks)
              .hidden(true));

    sg.insert(Switch("find-function-padding")
              .intrinsicValue(true, settings_.partitioner.findingFunctionPadding)
              .doc("Look for padding such as zero bytes and certain instructions like no-ops that occur prior to the "
                   "lowest address of a function and attach them to the function as static data.  The "
                   "@s{no-find-function-padding} switch turns this off.  The default is to " +
                   std::string(settings_.partitioner.findingFunctionPadding?"":"not ") + "search for padding."));
    sg.insert(Switch("no-find-function-padding")
              .key("find-function-padding")
              .intrinsicValue(false, settings_.partitioner.findingFunctionPadding)
              .hidden(true));

    sg.insert(Switch("find-dead-code")
              .intrinsicValue(true, settings_.partitioner.findingDeadCode)
              .doc("Use ghost edges (non-followed control flow from branches with opaque predicates) to locate addresses "
                   "for unreachable code, then recursively discover basic blocks at those addresses and add them to the "
                   "same function.  The @s{no-find-dead-code} switch turns this off.  The default is " +
                   std::string(settings_.partitioner.findingDeadCode?"true":"false") + "."));
    sg.insert(Switch("no-find-dead-code")
              .key("find-dead-code")
              .intrinsicValue(false, settings_.partitioner.findingDeadCode)
              .hidden(true));

    sg.insert(Switch("find-thunks")
              .intrinsicValue(true, settings_.partitioner.findingThunks)
              .doc("Search for common thunk patterns in areas of executable memory that have not been previously "
                   "discovered to contain other functions.  When this switch is enabled, the function-searching callbacks "
                   "include the patterns to match thunks.  This switch does not cause the thunk's instructions to be "
                   "detached as a separate function from the thunk's target function; that's handled by the "
                   "@s{split-thunks} switch.  The @s{no-find-thunks} switch turns thunk searching off. The default "
                   "is to " + std::string(settings_.partitioner.findingThunks ? "" : "not ") + "search for thunks."));
    sg.insert(Switch("no-find-thunks")
              .key("find-thunks")
              .intrinsicValue(false, settings_.partitioner.findingThunks)
              .hidden(true));

    sg.insert(Switch("split-thunks")
              .intrinsicValue(true, settings_.partitioner.splittingThunks)
              .doc("Look for common thunk patterns at the start of existing functions and split off those thunk "
                   "instructions to their own separate function.  The @s{no-detach-thunks} switch turns this feature "
                   "off.  The default is to " + std::string(settings_.partitioner.splittingThunks?"":"not ") +
                   "split thunks into their own functions."));
    sg.insert(Switch("no-split-thunks")
              .key("split-thunks")
              .intrinsicValue(false, settings_.partitioner.splittingThunks)
              .hidden(true));

    sg.insert(Switch("pe-scrambler")
              .argument("dispatcher_address", nonNegativeIntegerParser(settings_.partitioner.peScramblerDispatcherVa))
              .doc("Simulate the action of the PEScrambler dispatch function in order to rewrite CFG edges.  Any edges "
                   "that go into the specified @v{dispatcher_address} are immediately rewritten so they appear to go "
                   "instead to the function contained in the dispatcher table which normally immediately follows the "
                   "dispatcher function.  The dispatcher function is quite easy to find in a call graph because nearly "
                   "everything calls it -- it will likely have far and away more callers than anything else.  Setting the "
                   "address to zero disables this module (which is the default)."));

    sg.insert(Switch("intra-function-code")
              .intrinsicValue(true, settings_.partitioner.findingIntraFunctionCode)
              .doc("Near the end of processing, if there are regions of unused memory that are immediately preceded and "
                   "followed by the same single function then a basic block is create at the beginning of that region and "
                   "added as a member of the surrounding function.  A function block discover phase follows in order to "
                   "find the instructions for the new basic blocks and to follow their control flow to add additional "
                   "blocks to the functions.  These two steps are repeated until no new code can be created.  This step "
                   "occurs before the @s{intra-function-data} step if both are enabled.  The @s{no-intra-function-code} "
                   "switch turns this off. The default is to " +
                   std::string(settings_.partitioner.findingIntraFunctionCode?"":"not ") +
                   "perform this analysis."));
    sg.insert(Switch("no-intra-function-code")
              .key("intra-function-code")
              .intrinsicValue(false, settings_.partitioner.findingIntraFunctionCode)
              .hidden(true));

    sg.insert(Switch("intra-function-data")
              .intrinsicValue(true, settings_.partitioner.findingIntraFunctionData)
              .doc("Near the end of processing, if there are regions of unused memory that are immediately preceded and "
                   "followed by the same function then add that region of memory to that function as a static data block."
                   "The @s{no-intra-function-data} switch turns this feature off.  The default is " +
                   std::string(settings_.partitioner.findingIntraFunctionData?"true":"false") + "."));
    sg.insert(Switch("no-intra-function-data")
              .key("intra-function-data")
              .intrinsicValue(false, settings_.partitioner.findingIntraFunctionData)
              .hidden(true));

    sg.insert(Switch("data-functions")
              .intrinsicValue(true, settings_.partitioner.findingDataFunctionPointers)
              .doc("Scan non-executable areas of memory to find pointers to functions.  This analysis can be disabled "
                   "with @s{no-data-functions}. The default is to " +
                   std::string(settings_.partitioner.findingDataFunctionPointers?"":"not ") +
                   "perform this analysis."));
    sg.insert(Switch("no-data-functions")
              .key("data-functions")
              .intrinsicValue(false, settings_.partitioner.findingDataFunctionPointers)
              .hidden(true));

    sg.insert(Switch("interrupt-vector")
              .argument("addresses", addressIntervalParser(settings_.partitioner.interruptVector))
              .doc("A table containing addresses of functions invoked for various kinds of interrupts. " +
                   AddressIntervalParser::docString() + " The length and contents of the table is architecture "
                   "specific, and the disassembler will use available information about the architecture to decode the "
                   "table.  If a single address is specified, then the length of the table is architecture dependent, "
                   "otherwise the entire table is read."));

    sg.insert(Switch("name-constants")
              .intrinsicValue(true, settings_.partitioner.namingConstants)
              .doc("Scans the instructions and gives labels to constants that refer to entities that have that address "
                   "and also have a name.  For instance, if a constant refers to the beginning of a file section then "
                   "the constant will be labeled so it has the same name as the section.  The @s{no-name-constants} "
                   "turns this feature off. The default is to " + std::string(settings_.partitioner.namingConstants?"":"not ") +
                   "do this step."));
    sg.insert(Switch("no-name-constants")
              .key("name-constants")
              .intrinsicValue(false, settings_.partitioner.namingConstants)
              .hidden(true));

    sg.insert(Switch("name-strings")
              .intrinsicValue(true, settings_.partitioner.namingStrings)
              .doc("Scans the instructions and gives labels to constants that refer to the beginning of string literals. "
                   "The label is usually the first few characters of the string.  The @s{no-name-strings} turns this "
                   "feature off. The default is to " + std::string(settings_.partitioner.namingStrings?"":"not ") +
                   "do this step."));
    sg.insert(Switch("no-name-strings")
              .key("name-strings")
              .intrinsicValue(false, settings_.partitioner.namingStrings)
              .hidden(true));

    sg.insert(Switch("post-analysis")
              .intrinsicValue(true, settings_.partitioner.doingPostAnalysis)
              .doc("Run all enabled post-partitioning analysis functions.  For instance, calculate stack deltas for each "
                   "instruction, and may-return analysis for each function.  The individual analyses are enabled and "
                   "disabled separately with other s{post-*} switches. Some of these analyses will only work if "
                   "instruction semantics are enabled (see @s{use-semantics}).  The @s{no-post-analysis} switch turns "
                   "this off, although analysis will still be performed where it is needed for partitioning.  The "
                   "default is to " + std::string(settings_.partitioner.doingPostAnalysis?"":"not ") +
                   "perform the post analysis phase."));
    sg.insert(Switch("no-post-analysis")
              .key("post-analysis")
              .intrinsicValue(false, settings_.partitioner.doingPostAnalysis)
              .hidden(true));

    sg.insert(Switch("post-function-noop")
              .intrinsicValue(true, settings_.partitioner.doingPostFunctionNoop)
              .doc("Run a function no-op analysis for each function if post-partitioning analysis is enabled with the "
                   "@s{post-analysis} switch. This analysis tries to determine whether each function is effectively a no-op. "
                   "Functions that are no-ops are given names (if they don't already have one) that's indicative of "
                   "being a no-op. The @s{no-post-function-noop} switch disables this analysis. The default is that "
                   "this analysis is " +
                   std::string(settings_.partitioner.doingPostFunctionNoop?"enable":"disable") + "."));
    sg.insert(Switch("no-post-function-noop")
              .key("post-function-noop")
              .intrinsicValue(false, settings_.partitioner.doingPostFunctionNoop)
              .hidden(true));

    sg.insert(Switch("post-may-return")
              .intrinsicValue(true, settings_.partitioner.doingPostFunctionMayReturn)
              .doc("Run the may-return analysis for each function if post-partitioning analysis is enabled with the "
                   "@s{post-analysis} switch. This analysis tries to quickly determine if a function might return a "
                   "value to the caller.  The @s{no-post-may-return} switch disables this analysis. The default is that "
                   "this analysis is " +
                   std::string(settings_.partitioner.doingPostFunctionMayReturn?"enabled":"disabled") + "."));
    sg.insert(Switch("no-post-may-return")
              .key("post-may-return")
              .intrinsicValue(false, settings_.partitioner.doingPostFunctionMayReturn)
              .hidden(true));

    sg.insert(Switch("post-stack-delta")
              .intrinsicValue(true, settings_.partitioner.doingPostFunctionStackDelta)
              .doc("Run the stack-delta analysis for each function if post-partitioning analysis is enabled with the "
                   "@s{post-analysis} switch.  This is a data-flow analysis that tries to determine whether the function "
                   "has a constant net effect on the stack pointer and what that effect is.  For instance, when a caller "
                   "is reponsible for cleaning up function call arguments on a 32-bit architecture with a downward-growing "
                   "stack then the stack delta is usually +4, representing the fact that the called function popped the "
                   "return address from the stack.  The @s{no-post-stack-delta} switch disables this analysis. The default "
                   "is that this analysis is " +
                   std::string(settings_.partitioner.doingPostFunctionStackDelta?"enabled":"disabled") + "."));
    sg.insert(Switch("no-post-stack-delta")
              .key("post-stack-delta")
              .intrinsicValue(false, settings_.partitioner.doingPostFunctionStackDelta)
              .hidden(true));

    sg.insert(Switch("post-calling-convention")
              .intrinsicValue(true, settings_.partitioner.doingPostCallingConvention)
              .doc("Run the calling-convention analysis for each function. This relatively expensive analysis uses "
                   "use-def, stack-delta, memory variable discovery, and data-flow to determine characteristics of the "
                   "function and then matches it against a dictionary of calling conventions appropriate for the "
                   "architecture.  The @s{no-post-calling-convention} disables this analysis. The default is that this "
                   "analysis is " +
                   std::string(settings_.partitioner.doingPostCallingConvention?"enabled":"disabled") + "."));
    sg.insert(Switch("no-post-calling-convention")
              .key("post-calling-convention")
              .intrinsicValue(false, settings_.partitioner.doingPostCallingConvention)
              .hidden(true));

    sg.insert(Switch("functions-return")
              .argument("how", enumParser<FunctionReturnAnalysis>(settings_.partitioner.functionReturnAnalysis)
                        ->with("always", MAYRETURN_ALWAYS_YES)
                        ->with("never", MAYRETURN_ALWAYS_NO)
                        ->with("yes", MAYRETURN_DEFAULT_YES)
                        ->with("no", MAYRETURN_DEFAULT_NO))
              .doc("Determines how function may-return analysis is performed. This analysis returns true if a call to the "
                   "function has a possibility of returning, false if the call has no possibility of returning, or "
                   "indeterminate if the analysis cannot decide.  The partitioner will attempt to disassemble instructions "
                   "at the fall-through address of a call if the call has a possibility of returning, otherwise that address "
                   "will be disassembled only if it can be reached by some other mechanism.\n\n"

                   "This switch accepts one of these four words:"
                   "@named{always}{Assume that all function calls may return without ever running the may-return analysis.}"
                   "@named{never}{Assume that all functions cannot return to the caller and never run the may-return analysis.}"
                   "@named{yes}{Assume a function returns if the may-return analysis cannot decide. This is the default.}"
                   "@named{no}{Assume a function does not return if the may-return analysis cannot decide.}"));

    return sg;
}

Sawyer::CommandLine::SwitchGroup
Engine::engineSwitches() {
    using namespace Sawyer::CommandLine;
    SwitchGroup sg = CommandlineProcessing::genericSwitches();

    sg.insert(Switch("config")
              .argument("names", listParser(anyParser(settings_.engine.configurationNames), ":"))
              .explosiveLists(true)
              .whichValue(SAVE_ALL)
              .doc("Directories containing configuration files, or configuration files themselves.  A directory is searched "
                   "recursively searched for files whose names end with \".json\" or and each file is parsed and used to "
                   "to configure the partitioner.  The JSON file contents is defined by the Carnegie Mellon University "
                   "Software Engineering Institute. It should have a top-level \"config.exports\" table whose keys are "
                   "function names and whose values are have a \"function.delta\" integer. The delta does not include "
                   "popping the return address from the stack in the final RET instruction.  Function names of the form "
                   "\"lib:func\" are translated to the ROSE format \"func@lib\"."));
    return sg;
}

std::string
Engine::specimenNameDocumentation() {
    return ("The following names are recognized for binary specimens:"

            "@bullet{If the name does not match any of the following patterns then it is assumed to be the name of a "
            "file containing a specimen that is a binary container format such as ELF or PE.}"

            "@bullet{If the name begins with the string \"map:\" then it is treated as a memory map resource string that "
            "adjusts a memory map by inserting part of a file. " + MemoryMap::insertFileDocumentation() + "}"

            "@bullet{If the name begins with the string \"proc:\" then it is treated as a process resource string that "
            "adjusts a memory map by reading the process' memory. " + MemoryMap::insertProcessDocumentation() + "}"

            "@bullet{If the name begins with the string \"run:\" then it is first treated like a normal file by ROSE's "
            "\"fontend\" function, and then during a second pass it will be loaded natively under a debugger, run until "
            "a mapped executable address is reached, and then its memory is copied into ROSE's memory map possibly "
            "overwriting existing parts of the map.  This can be useful when the user wants accurate information about "
            "how that native loader links in shared objects since ROSE's linker doesn't always have identical behavior.}"

            "@bullet{If the file name begins with the string \"srec:\" then it is treated as Motorola S-Record format. "
            "Mapping attributes are stored after the first column and before the second; the file name appears after the "
            "second colon.  The only mapping attributes supported at this time are permissions, specified as an equal "
            "sign ('=') followed by zero or more of the letters \"r\", \"w\", and \"x\" to signify read, write, and "
            "execute permissions. If no letters are present after the equal sign, then the memory has no permissions; "
            "if the equal sign itself is also missing then the segments are given read, write, and execute permission.}"

            "@bullet{If the name ends with \".srec\" and doesn't match the previous list of prefixes then it is assumed "
            "to be a text file containing Motorola S-Records and will be parsed as such and loaded into the memory map "
            "with read, write, and execute permissions.}"

            "When more than one mechanism is used to load a single coherent specimen, the normal names are processed first "
            "by passing them all to ROSE's \"frontend\" function, which results in an initial memory map.  The other names "
            "are then processed in the order they appear, possibly overwriting parts of the map.");
}

Sawyer::CommandLine::Parser
Engine::commandLineParser(const std::string &purpose, const std::string &description) {
    using namespace Sawyer::CommandLine;
    Parser parser;
    parser.purpose(purpose.empty() ? std::string("analyze binary specimen") : purpose);
    parser.version(std::string(ROSE_SCM_VERSION_ID).substr(0, 8), ROSE_CONFIGURE_DATE);
    parser.chapter(1, "ROSE Command-line Tools");
    parser.doc("Synopsis", "@prop{programName} [@v{switches}] @v{specimen_names}");
    if (!description.empty())
        parser.doc("Description", description);
    parser.doc("Specimens", specimenNameDocumentation());
    parser.with(engineSwitches());
    parser.with(loaderSwitches());
    parser.with(disassemblerSwitches());
    parser.with(partitionerSwitches());
    return parser;
}

Sawyer::CommandLine::ParserResult
Engine::parseCommandLine(int argc, char *argv[], const std::string &purpose, const std::string &description) {
    std::vector<std::string> args;
    for (int i=1; i<argc; ++i)
        args.push_back(argv[i]);
    return parseCommandLine(args, purpose, description);
}

Sawyer::CommandLine::ParserResult
Engine::parseCommandLine(const std::vector<std::string> &args, const std::string &purpose, const std::string &description) {
    return commandLineParser(purpose, description).parse(args).apply();
}

void
Engine::checkSettings() {
    if (!disassembler_ && !settings_.disassembler.isaName.empty())
        disassembler_ = Disassembler::lookup(settings_.disassembler.isaName);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Binary container parsing
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool
Engine::isNonContainer(const std::string &name) {
    return (boost::starts_with(name, "map:") ||         // map file directly into MemoryMap
            boost::starts_with(name, "proc:") ||        // map process memory into MemoryMap
            boost::starts_with(name, "run:") ||         // run a process in a debugger, then map into MemoryMap
            boost::starts_with(name, "srec:") ||        // Motorola S-Record format
            boost::ends_with(name, ".srec"));           // Motorola S-Record format
}

bool
Engine::areContainersParsed() const {
    return interp_ != NULL;
}

SgAsmInterpretation*
Engine::parseContainers(const std::string &fileName) {
    return parseContainers(std::vector<std::string>(1, fileName));
}

SgAsmInterpretation*
Engine::parseContainers(const std::vector<std::string> &fileNames) {
    interp_ = NULL;
    map_.clear();
    checkSettings();

    // Prune away things we recognize as not being binary containers.
    std::vector<std::string> frontendNames;
    BOOST_FOREACH (const std::string &fileName, fileNames) {
        if (boost::starts_with(fileName, "run:") && fileName.size()>4) {
            frontendNames.push_back(fileName.substr(4));
        } else if (!isNonContainer(fileName)) {
            frontendNames.push_back(fileName);
        }
    }

    // Process through ROSE's frontend()
    if (!frontendNames.empty()) {
        std::vector<std::string> frontendArgs;
        frontendArgs.push_back("/proc/self/exe");       // I don't think frontend actually uses this
        frontendArgs.push_back("-rose:binary");
        frontendArgs.push_back("-rose:read_executable_file_format_only");
        frontendArgs.insert(frontendArgs.end(), frontendNames.begin(), frontendNames.end());
        SgProject *project = ::frontend(frontendArgs);
        std::vector<SgAsmInterpretation*> interps = SageInterface::querySubTree<SgAsmInterpretation>(project);
        if (interps.empty())
            throw std::runtime_error("a binary specimen container must have at least one SgAsmInterpretation");
        interp_ = interps.back();    // windows PE is always after DOS
        ASSERT_require(areContainersParsed());
    }

    ASSERT_require(!areSpecimensLoaded());
    return interp_;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Memory map creation (loading)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool
Engine::areSpecimensLoaded() const {
    return !map_.isEmpty();
}

BinaryLoader*
Engine::obtainLoader(BinaryLoader *hint) {
    if (!binaryLoader_ && interp_) {
        if ((binaryLoader_ = BinaryLoader::lookup(interp_))) {
            binaryLoader_ = binaryLoader_->clone();
            binaryLoader_->set_perform_remap(true);
            binaryLoader_->set_perform_dynamic_linking(false);
            binaryLoader_->set_perform_relocations(false);
        }
    }

    if (!binaryLoader_ && hint)
        binaryLoader_ = hint;

    if (!binaryLoader_)
        throw std::runtime_error("no binary loader found and none specified");

    return binaryLoader_;
}

void
Engine::loadContainers(const std::vector<std::string> &fileNames) {
    // Load the interpretation if it hasn't been already
    if (interp_ && (!interp_->get_map() || interp_->get_map()->isEmpty())) {
        obtainLoader();
        binaryLoader_->load(interp_);
    }

    // Get a map from the now-loaded interpretation, or use an empty map if the interp isn't mapped
    if (interp_ && interp_->get_map())
        map_ = *interp_->get_map();
}

void
Engine::loadNonContainers(const std::vector<std::string> &fileNames) {
    BOOST_FOREACH (const std::string &fileName, fileNames) {
        if (boost::starts_with(fileName, "map:")) {
            std::string resource = fileName.substr(3);  // remove "map", leaving colon and rest of string
            map_.insertFile(resource);
        } else if (boost::starts_with(fileName, "proc:")) {
            std::string resource = fileName.substr(4);  // remove "proc", leaving colon and the rest of the string
            map_.insertProcess(resource);
        } else if (boost::starts_with(fileName, "run:")) {
            std::string exeName = fileName.substr(4);
            BinaryDebugger debugger(exeName);
            BOOST_FOREACH (const MemoryMap::Node &node, map_.nodes()) {
                if (0 != (node.value().accessibility() & MemoryMap::EXECUTABLE))
                    debugger.setBreakpoint(node.key());
            }
            debugger.runToBreakpoint();
            if (debugger.isTerminated())
                throw std::runtime_error(exeName + " " + debugger.howTerminated() + " without reaching a breakpoint");
            map_.insertProcess(":noattach:" + StringUtility::numberToString(debugger.isAttached()));
            debugger.terminate();
        } else if (boost::starts_with(fileName, "srec:") || boost::ends_with(fileName, ".srec")) {
            std::string resource;                       // name of file to open
            unsigned perms = MemoryMap::READABLE | MemoryMap::WRITABLE | MemoryMap::EXECUTABLE;

            if (boost::starts_with(fileName, "srec:")) {
                // Format is "srec:[=PERMS]:FILENAME" where PERMS are the letters "r", "w", and/or "x"
                std::vector<std::string> parts = StringUtility::split(":", fileName, 3);
                if (parts.size() != 3)
                    throw std::runtime_error("second ':' expected in \"srec\" URI (expected \"srec:[=PERMS]:FILENAME\")");
                resource = parts[2];

                // Permissions, like "=rw". Lack of '=...' means default permissions; nothing after '=' means no permissions
                // (e.g., "srec:=:filename").
                if (!parts[1].empty()) {
                    if ('=' != parts[1][0])
                        throw std::runtime_error("expected \"=PERMS\" in \"srec:\" URI");
                    perms = 0;
                    for (size_t i=1; i<parts[1].size(); ++i) {
                        switch (parts[1][i]) {
                            case 'r': perms |= MemoryMap::READABLE; break;
                            case 'w': perms |= MemoryMap::WRITABLE; break;
                            case 'x': perms |= MemoryMap::EXECUTABLE; break;
                            default:
                                throw std::runtime_error("invalid permission character '" +
                                                         StringUtility::cEscape(parts[1].substr(i, 1)) +
                                                         "' in \"srec:\" URI");
                                break;
                        }
                    }
                }
            } else {
                resource = fileName;
            }
            
            // Parse and load the S-Record file
            if (resource.size()!=strlen(resource.c_str())) {
                throw std::runtime_error("file name contains internal NUL characters: \"" +
                                         StringUtility::cEscape(resource) + "\"");
            }
            std::ifstream input(resource.c_str());
            if (!input.good()) {
                throw std::runtime_error("cannot open Motorola S-Record file: \"" +
                                         StringUtility::cEscape(resource) + "\"");
            }
            std::vector<SRecord> srecs = SRecord::parse(input);
            for (size_t i=0; i<srecs.size(); ++i) {
                if (!srecs[i].error().empty())
                    mlog[ERROR] <<resource <<":" <<(i+1) <<": S-Record: " <<srecs[i].error() <<"\n";
            }
            SRecord::load(srecs, map_, true /*create*/, perms);
        }
    }
}

void
Engine::adjustMemoryMap() {
    if (settings_.loader.memoryIsExecutable)
        map_.any().changeAccess(MemoryMap::EXECUTABLE, 0);
    Modules::deExecuteZeros(map_/*in,out*/, settings_.loader.deExecuteZeros);

    switch (settings_.loader.memoryDataAdjustment) {
        case DATA_IS_CONSTANT:
            map_.any().changeAccess(0, MemoryMap::WRITABLE);
            break;
        case DATA_IS_INITIALIZED:
            map_.any().changeAccess(MemoryMap::INITIALIZED, 0);
            break;
        case DATA_NO_CHANGE:
            break;
    }
}

MemoryMap&
Engine::loadSpecimens(const std::string &fileName) {
    return loadSpecimens(std::vector<std::string>(1, fileName));
}

MemoryMap&
Engine::loadSpecimens(const std::vector<std::string> &fileNames) {
    map_.clear();
    if (!areContainersParsed())
        parseContainers(fileNames);
    loadContainers(fileNames);
    loadNonContainers(fileNames);
    adjustMemoryMap();
    return map_;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Disassembler creation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Disassembler*
Engine::obtainDisassembler(Disassembler *hint) {
    if (!disassembler_ && !settings_.disassembler.isaName.empty() &&
        (disassembler_ = Disassembler::lookup(settings_.disassembler.isaName)))
        disassembler_ = disassembler_->clone();

    if (!disassembler_ && interp_ &&
        (disassembler_ = Disassembler::lookup(interp_)))
        disassembler_ = disassembler_->clone();

    if (!disassembler_ && hint)
        disassembler_ = hint;

    if (!disassembler_)
        throw std::runtime_error("no disassembler found and none specified");

    return disassembler_;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Partitioner high-level functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
Engine::checkCreatePartitionerPrerequisites() const {
    if (NULL==disassembler_)
        throw std::runtime_error("Engine::createBarePartitioner needs a prior disassembler");
    if (map_.isEmpty())
        mlog[WARN] <<"Engine::createBarePartitioner: using an empty memory map\n";
}

Partitioner
Engine::createBarePartitioner() {
    Sawyer::Message::Stream info(mlog[MARCH]);

    checkCreatePartitionerPrerequisites();
    Partitioner p(disassembler_, map_);

    // Load configuration files
    if (!settings_.engine.configurationNames.empty()) {
        Sawyer::Stopwatch timer;
        info <<"loading configuration files";
        BOOST_FOREACH (const std::string &configName, settings_.engine.configurationNames)
            p.configuration().loadFromFile(configName);
        info <<"; took " <<timer <<" seconds\n";
    }

    // Build the may-return blacklist and/or whitelist.  This could be made specific to the type of interpretation being
    // processed, but there's so few functions that we'll just plop them all into the lists.
    ModulesPe::buildMayReturnLists(p);
    ModulesElf::buildMayReturnLists(p);

    // Make sure the basicBlockWorkList_ gets updated when the partitioner's CFG is adjusted.
    ASSERT_not_null(basicBlockWorkList_);
    p.cfgAdjustmentCallbacks().prepend(basicBlockWorkList_);

    // Perform some finalization whenever a basic block is created.  For instance, this figures out whether we should add an
    // extra indeterminate edge for indirect jump instructions that go through initialized but writable memory.
    p.basicBlockCallbacks().append(BasicBlockFinalizer::instance());

    // If the may-return analysis is run and cannot decide whether a function may return, should we assume that it may or
    // cannot return?  The engine decides whether to actually invoke the analysis -- this just sets what to do if it's
    // invoked.
    switch (settings_.partitioner.functionReturnAnalysis) {
        case MAYRETURN_ALWAYS_YES:
        case MAYRETURN_DEFAULT_YES:
            p.assumeFunctionsReturn(true);
            break;
        case MAYRETURN_ALWAYS_NO:
        case MAYRETURN_DEFAULT_NO:
            p.assumeFunctionsReturn(false);
    }

    // Should the partitioner favor list-based or map-based containers for semantic memory states?
    p.semanticMemoryParadigm(settings_.partitioner.semanticMemoryParadigm);
            
    // Miscellaneous settings
    p.enableSymbolicSemantics(settings_.partitioner.usingSemantics);
    if (settings_.partitioner.followingGhostEdges)
        p.basicBlockCallbacks().append(Modules::AddGhostSuccessors::instance());
    if (!settings_.partitioner.discontiguousBlocks)
        p.basicBlockCallbacks().append(Modules::PreventDiscontiguousBlocks::instance());

    // PEScrambler descrambler
    if (settings_.partitioner.peScramblerDispatcherVa) {
        ModulesPe::PeDescrambler::Ptr cb = ModulesPe::PeDescrambler::instance(settings_.partitioner.peScramblerDispatcherVa);
        cb->nameKeyAddresses(p);                        // give names to certain PEScrambler things
        p.basicBlockCallbacks().append(cb);
        p.attachFunction(Function::instance(settings_.partitioner.peScramblerDispatcherVa,
                                            p.addressName(settings_.partitioner.peScramblerDispatcherVa),
                                            SgAsmFunction::FUNC_USERDEF)); 
    }
    
    return p;
}

Partitioner
Engine::createGenericPartitioner() {
    checkCreatePartitionerPrerequisites();
    Partitioner p = createBarePartitioner();
    p.functionPrologueMatchers().push_back(ModulesX86::MatchHotPatchPrologue::instance());
    p.functionPrologueMatchers().push_back(ModulesX86::MatchStandardPrologue::instance());
    p.functionPrologueMatchers().push_back(ModulesX86::MatchAbbreviatedPrologue::instance());
    p.functionPrologueMatchers().push_back(ModulesX86::MatchEnterPrologue::instance());
    if (settings_.partitioner.findingThunks)
        p.functionPrologueMatchers().push_back(ModulesX86::MatchThunk::instance());
    p.functionPrologueMatchers().push_back(ModulesX86::MatchRetPadPush::instance());
    p.functionPrologueMatchers().push_back(ModulesM68k::MatchLink::instance());
    p.basicBlockCallbacks().append(ModulesX86::FunctionReturnDetector::instance());
    p.basicBlockCallbacks().append(ModulesM68k::SwitchSuccessors::instance());
    p.basicBlockCallbacks().append(ModulesX86::SwitchSuccessors::instance());
    return p;
}

Partitioner
Engine::createTunedPartitioner() {
    obtainDisassembler();

    if (dynamic_cast<DisassemblerM68k*>(disassembler_)) {
        checkCreatePartitionerPrerequisites();
        Partitioner p = createBarePartitioner();
        p.functionPrologueMatchers().push_back(ModulesM68k::MatchLink::instance());
        p.basicBlockCallbacks().append(ModulesM68k::SwitchSuccessors::instance());
        return p;
    }
    
    if (dynamic_cast<DisassemblerX86*>(disassembler_)) {
        checkCreatePartitionerPrerequisites();
        Partitioner p = createBarePartitioner();
        p.functionPrologueMatchers().push_back(ModulesX86::MatchHotPatchPrologue::instance());
        p.functionPrologueMatchers().push_back(ModulesX86::MatchStandardPrologue::instance());
        p.functionPrologueMatchers().push_back(ModulesX86::MatchEnterPrologue::instance());
        if (settings_.partitioner.findingThunks)
            p.functionPrologueMatchers().push_back(ModulesX86::MatchThunk::instance());
        p.functionPrologueMatchers().push_back(ModulesX86::MatchRetPadPush::instance());
        p.basicBlockCallbacks().append(ModulesX86::FunctionReturnDetector::instance());
        p.basicBlockCallbacks().append(ModulesX86::SwitchSuccessors::instance());
        return p;
    }

    return createGenericPartitioner();
}

Partitioner
Engine::createPartitionerFromAst(SgAsmInterpretation *interp) {
    ASSERT_not_null(interp);
    interp_ = interp;
    map_.clear();
    loadSpecimens(std::vector<std::string>());
    Partitioner partitioner = createTunedPartitioner();

    // Cache all the instructions so they're available by address in O(log N) time in the future.
    BOOST_FOREACH (SgAsmInstruction *insn, SageInterface::querySubTree<SgAsmInstruction>(interp))
        partitioner.instructionProvider().insert(insn);

    // Create and attach basic blocks
    BOOST_FOREACH (SgAsmNode *node, SageInterface::querySubTree<SgAsmNode>(interp)) {
        SgAsmBlock *blockAst = isSgAsmBlock(node);
        if (!blockAst || !blockAst->has_instructions())
            continue;
        BasicBlock::Ptr bblock = BasicBlock::instance(blockAst->get_address(), &partitioner);
        bblock->comment(blockAst->get_comment());

        // Instructions
        const SgAsmStatementPtrList &stmts = blockAst->get_statementList();
        for (SgAsmStatementPtrList::const_iterator si=stmts.begin(); si!=stmts.end(); ++si) {
            if (SgAsmInstruction *insn = isSgAsmInstruction(*si))
                bblock->append(insn);
        }

        // Successors
        const SgAsmIntegerValuePtrList &successors = blockAst->get_successors();
        BOOST_FOREACH (SgAsmIntegerValueExpression *ival, successors)
            bblock->insertSuccessor(ival->get_absoluteValue(), ival->get_significantBits());
        if (!blockAst->get_successors_complete()) {
            size_t nbits = partitioner.instructionProvider().instructionPointerRegister().get_nbits();
            bblock->insertSuccessor(Semantics::SValue::instance_undefined(nbits));
        }

        partitioner.attachBasicBlock(bblock);
    }

    // Create and attach functions
    BOOST_FOREACH (SgAsmFunction *funcAst, SageInterface::querySubTree<SgAsmFunction>(interp)) {
        if (0!=(funcAst->get_reason() & SgAsmFunction::FUNC_LEFTOVERS))
            continue;                                   // this isn't really a true function
        Function::Ptr function = Function::instance(funcAst->get_entry_va(), funcAst->get_name());
        function->comment(funcAst->get_comment());
        function->reasons(funcAst->get_reason());

        BOOST_FOREACH (SgAsmBlock *blockAst, SageInterface::querySubTree<SgAsmBlock>(funcAst)) {
            if (blockAst->has_instructions())
                function->insertBasicBlock(blockAst->get_address());
        }

        BOOST_FOREACH (SgAsmStaticData *dataAst, SageInterface::querySubTree<SgAsmStaticData>(funcAst)) {
            DataBlock::Ptr dblock = DataBlock::instance(dataAst->get_address(), dataAst->get_size());
            partitioner.attachDataBlock(dblock);
            function->insertDataBlock(dblock);
        }

        partitioner.attachFunction(function);
    }

    return partitioner;
}

Partitioner
Engine::createPartitioner() {
    return createTunedPartitioner();
}

void
Engine::runPartitionerInit(Partitioner &partitioner) {
    labelAddresses(partitioner);
    makeConfiguredDataBlocks(partitioner, partitioner.configuration());
    makeConfiguredFunctions(partitioner, partitioner.configuration());
    makeContainerFunctions(partitioner, interp_);
    makeInterruptVectorFunctions(partitioner, settings_.partitioner.interruptVector);
    makeUserFunctions(partitioner, settings_.partitioner.startingVas);
}

void
Engine::runPartitionerRecursive(Partitioner &partitioner) {
    // Start discovering instructions and forming them into basic blocks and functions
    discoverFunctions(partitioner);

    // Additional work
    if (settings_.partitioner.findingDeadCode)
        attachDeadCodeToFunctions(partitioner);
    if (settings_.partitioner.findingFunctionPadding)
        attachPaddingToFunctions(partitioner);
    if (settings_.partitioner.findingIntraFunctionCode)
        attachAllSurroundedCodeToFunctions(partitioner);
    if (settings_.partitioner.findingIntraFunctionData)
        attachSurroundedDataToFunctions(partitioner);

    // Another pass to attach blocks to functions
    attachBlocksToFunctions(partitioner);
}

void
Engine::runPartitionerFinal(Partitioner &partitioner) {
    if (settings_.partitioner.splittingThunks) {
        // Splitting thunks off the front of a basic block causes the rest of the basic block to be discarded and then
        // rediscovered. This might also create additional blocks due to the fact that opaque predicate analysis runs only on
        // single blocks at a time -- splitting the block may have broken the opaque predicate.
        ModulesX86::splitThunkFunctions(partitioner);
        discoverBasicBlocks(partitioner);
    }

    // Perform a final pass over all functions.
    attachBlocksToFunctions(partitioner);

    if (interp_)
        ModulesPe::nameImportThunks(partitioner, interp_);
    if (settings_.partitioner.namingConstants)
        Modules::nameConstants(partitioner);
    if (settings_.partitioner.namingStrings)
        Modules::nameStrings(partitioner);
}

void
Engine::runPartitioner(Partitioner &partitioner) {
    Sawyer::Message::Stream info(mlog[INFO]);
    Sawyer::Stopwatch timer;
    info <<"disassembling and partitioning";
    runPartitionerInit(partitioner);
    runPartitionerRecursive(partitioner);
    runPartitionerFinal(partitioner);
    info <<"; took " <<timer <<" seconds\n";

    if (settings_.partitioner.doingPostAnalysis)
        updateAnalysisResults(partitioner);
}

Partitioner
Engine::partition(const std::vector<std::string> &fileNames) {
    if (!areSpecimensLoaded())
        loadSpecimens(fileNames);
    obtainDisassembler();
    Partitioner partitioner = createPartitioner();
    runPartitioner(partitioner);
    return partitioner;
}

Partitioner
Engine::partition(const std::string &fileName) {
    return partition(std::vector<std::string>(1, fileName));
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Partitioner mid-level operations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
Engine::labelAddresses(Partitioner &partitioner) {
    Modules::labelSymbolAddresses(partitioner, interp_);
}

std::vector<DataBlock::Ptr>
Engine::makeConfiguredDataBlocks(Partitioner &partitioner, const Configuration &configuration) {
    // FIXME[Robb P. Matzke 2015-05-12]: This just adds labels to addresses right now.
    BOOST_FOREACH (const DataBlockConfig &dconfig, configuration.dataBlocks().values()) {
        if (!dconfig.name().empty())
            partitioner.addressName(dconfig.address(), dconfig.name());
    }
    return std::vector<DataBlock::Ptr>();
}

std::vector<Function::Ptr>
Engine::makeConfiguredFunctions(Partitioner &partitioner, const Configuration &configuration) {
    std::vector<Function::Ptr> retval;
    BOOST_FOREACH (const FunctionConfig &fconfig, configuration.functionConfigsByAddress().values()) {
        rose_addr_t entryVa = 0;
        if (fconfig.address().assignTo(entryVa)) {
            Function::Ptr function = Function::instance(entryVa, fconfig.name(), SgAsmFunction::FUNC_USERDEF);
            function->comment(fconfig.comment());
            insertUnique(retval, partitioner.attachOrMergeFunction(function), sortFunctionsByAddress);
        }
    }
    return retval;
}

std::vector<Function::Ptr>
Engine::makeEntryFunctions(Partitioner &partitioner, SgAsmInterpretation *interp) {
    std::vector<Function::Ptr> retval;
    if (interp) {
        BOOST_FOREACH (SgAsmGenericHeader *fileHeader, interp->get_headers()->get_headers()) {
            BOOST_FOREACH (const rose_rva_t &rva, fileHeader->get_entry_rvas()) {
                rose_addr_t va = rva.get_rva() + fileHeader->get_base_va();
                Function::Ptr function = Function::instance(va, "_start", SgAsmFunction::FUNC_ENTRY_POINT);
                insertUnique(retval, partitioner.attachOrMergeFunction(function), sortFunctionsByAddress);
            }
        }
    }
    return retval;
}

std::vector<Function::Ptr>
Engine::makeErrorHandlingFunctions(Partitioner &partitioner, SgAsmInterpretation *interp) {
    std::vector<Function::Ptr> retval;
    if (interp) {
        BOOST_FOREACH (const Function::Ptr &function, ModulesElf::findErrorHandlingFunctions(interp))
            insertUnique(retval, partitioner.attachOrMergeFunction(function), sortFunctionsByAddress);
    }
    return retval;
}

std::vector<Function::Ptr>
Engine::makeImportFunctions(Partitioner &partitioner, SgAsmInterpretation *interp) {
    std::vector<Function::Ptr> retval;
    if (interp) {
        // Windows PE imports
        ModulesPe::rebaseImportAddressTables(partitioner, ModulesPe::getImportIndex(partitioner, interp));
        BOOST_FOREACH (const Function::Ptr &function, ModulesPe::findImportFunctions(partitioner, interp))
            insertUnique(retval, partitioner.attachOrMergeFunction(function), sortFunctionsByAddress);
    
        // ELF imports
        BOOST_FOREACH (const Function::Ptr &function, ModulesElf::findPltFunctions(partitioner, interp))
            insertUnique(retval, partitioner.attachOrMergeFunction(function), sortFunctionsByAddress);
    }
    return retval;
}

std::vector<Function::Ptr>
Engine::makeExportFunctions(Partitioner &partitioner, SgAsmInterpretation *interp) {
    std::vector<Function::Ptr> retval;
    if (interp) {
        BOOST_FOREACH (const Function::Ptr &function, ModulesPe::findExportFunctions(partitioner, interp))
            insertUnique(retval, partitioner.attachOrMergeFunction(function), sortFunctionsByAddress);
    }
    return retval;
}

std::vector<Function::Ptr>
Engine::makeSymbolFunctions(Partitioner &partitioner, SgAsmInterpretation *interp) {
    std::vector<Function::Ptr> retval;
    if (interp) {
        BOOST_FOREACH (const Function::Ptr &function, Modules::findSymbolFunctions(partitioner, interp))
            insertUnique(retval, partitioner.attachOrMergeFunction(function), sortFunctionsByAddress);
    }
    return retval;
}

std::vector<Function::Ptr>
Engine::makeContainerFunctions(Partitioner &partitioner, SgAsmInterpretation *interp) {
    std::vector<Function::Ptr> retval;

    BOOST_FOREACH (const Function::Ptr &function, makeEntryFunctions(partitioner, interp))
        insertUnique(retval, function, sortFunctionsByAddress);
    BOOST_FOREACH (const Function::Ptr &function, makeErrorHandlingFunctions(partitioner, interp))
        insertUnique(retval, function, sortFunctionsByAddress);
    BOOST_FOREACH (const Function::Ptr &function, makeImportFunctions(partitioner, interp))
        insertUnique(retval, function, sortFunctionsByAddress);
    BOOST_FOREACH (const Function::Ptr &function, makeExportFunctions(partitioner, interp))
        insertUnique(retval, function, sortFunctionsByAddress);
    BOOST_FOREACH (const Function::Ptr &function, makeSymbolFunctions(partitioner, interp))
        insertUnique(retval, function, sortFunctionsByAddress);
    return retval;
}

std::vector<Function::Ptr>
Engine::makeInterruptVectorFunctions(Partitioner &partitioner, const AddressInterval &interruptVector) {
    std::vector<Function::Ptr> functions;
    if (interruptVector.isEmpty())
        return functions;
    Disassembler *disassembler = disassembler_ ? disassembler_ : partitioner.instructionProvider().disassembler();
    if (!disassembler) {
        throw std::runtime_error("cannot decode interrupt vector without architecture information");
    } else if (dynamic_cast<DisassemblerM68k*>(disassembler)) {
        BOOST_FOREACH (const Function::Ptr f, ModulesM68k::findInterruptFunctions(partitioner, interruptVector.least()))
            insertUnique(functions, partitioner.attachOrMergeFunction(f), sortFunctionsByAddress);
    } else if (1 == interruptVector.size()) {
        throw std::runtime_error("cannot determine interrupt vector size for architecture");
    } else {
        size_t ptrSize = partitioner.instructionProvider().instructionPointerRegister().get_nbits();
        ASSERT_require2(ptrSize % 8 == 0, "instruction pointer register size is strange");
        size_t bytesPerPointer = ptrSize / 8;
        size_t nPointers = interruptVector.size() / bytesPerPointer;
        ByteOrder::Endianness byteOrder = partitioner.instructionProvider().defaultByteOrder();

        for (size_t i=0; i<nPointers; ++i) {
            rose_addr_t elmtVa = interruptVector.least() + i*bytesPerPointer;
            uint32_t functionVa;
            if (4 == partitioner.memoryMap().at(elmtVa).limit(4).read((uint8_t*)&functionVa).size()) {
                functionVa = ByteOrder::disk_to_host(byteOrder, functionVa);
                std::string name = "interrupt_" + StringUtility::numberToString(i) + "_handler";
                Function::Ptr function = Function::instance(functionVa, name, SgAsmFunction::FUNC_EXCEPTION_HANDLER);
                if (Sawyer::Optional<Function::Ptr> found = getUnique(functions, function, sortFunctionsByAddress)) {
                    // Multiple vector entries point to the same function, so give it a rather generic name
                    found.get()->name("interrupt_vector_function");
                } else {
                    insertUnique(functions, partitioner.attachOrMergeFunction(function), sortFunctionsByAddress);
                }
            }
        }
    }
    return functions;
}

std::vector<Function::Ptr>
Engine::makeUserFunctions(Partitioner &partitioner, const std::vector<rose_addr_t> &vas) {
    std::vector<Function::Ptr> retval;
    BOOST_FOREACH (rose_addr_t va, vas) {
        Function::Ptr function = Function::instance(va, SgAsmFunction::FUNC_USERDEF);
        insertUnique(retval, partitioner.attachOrMergeFunction(function), sortFunctionsByAddress);
    }
    return retval;
}

void
Engine::discoverBasicBlocks(Partitioner &partitioner) {
    while (makeNextBasicBlock(partitioner)) /*void*/;
}

Function::Ptr
Engine::makeNextDataReferencedFunction(const Partitioner &partitioner, rose_addr_t &readVa /*in,out*/) {
    const rose_addr_t wordSize = partitioner.instructionProvider().instructionPointerRegister().get_nbits() / 8;
    ASSERT_require2(wordSize>0 && wordSize<=8, StringUtility::numberToString(wordSize)+"-byte words not implemented yet");
    const rose_addr_t maxaddr = partitioner.memoryMap().hull().greatest();

    while (readVa < maxaddr &&
           partitioner.memoryMap()
           .atOrAfter(readVa)
           .require(MemoryMap::READABLE).prohibit(MemoryMap::EXECUTABLE|MemoryMap::WRITABLE)
           .next().assignTo(readVa)) {

        // Addresses must be aligned on a word boundary
        if (rose_addr_t misaligned = readVa % wordSize) {
            readVa = incrementAddress(readVa, wordSize-misaligned, maxaddr);
            continue;
        }

        // Convert raw memory to native address
        // FIXME[Robb P. Matzke 2014-12-08]: assuming little endian
        ASSERT_require(wordSize<=8);
        uint8_t raw[8];
        if (partitioner.memoryMap().at(readVa).limit(wordSize)
            .require(MemoryMap::READABLE).prohibit(MemoryMap::EXECUTABLE|MemoryMap::WRITABLE)
            .read(raw).size()!=wordSize) {
            readVa = incrementAddress(readVa, wordSize, maxaddr);
            continue;
        }
        rose_addr_t targetVa = 0;
        for (size_t i=0; i<wordSize; ++i)
            targetVa |= raw[i] << (8*i);

        // Sanity checks
        SgAsmInstruction *insn = partitioner.discoverInstruction(targetVa);
        if (!insn || insn->isUnknown()) {
            readVa = incrementAddress(readVa, wordSize, maxaddr);
            continue;                                   // no instruction
        }
        AddressInterval insnInterval = AddressInterval::baseSize(insn->get_address(), insn->get_size());
        if (!partitioner.instructionsOverlapping(insnInterval).empty()) {
            readVa = incrementAddress(readVa, wordSize, maxaddr);
            continue;                                   // would overlap with existing instruction
        }
        
        // All seems okay, so make a function there
        // FIXME[Robb P. Matzke 2014-12-08]: USERDEF is not the best, most descriptive reason, but it's what we have for now
        mlog[INFO] <<"possible code address " <<StringUtility::addrToString(targetVa)
                   <<" found at read-only address " <<StringUtility::addrToString(readVa) <<"\n";
        readVa = incrementAddress(readVa, wordSize, maxaddr);
        return Function::instance(targetVa, SgAsmFunction::FUNC_USERDEF);
    }
    readVa = maxaddr;
    return Function::Ptr();
}

std::vector<Function::Ptr>
Engine::makeCalledFunctions(Partitioner &partitioner) {
    std::vector<Function::Ptr> retval;
    BOOST_FOREACH (const Function::Ptr &function, partitioner.discoverCalledFunctions())
        insertUnique(retval, partitioner.attachOrMergeFunction(function), sortFunctionsByAddress);
    return retval;
}

std::vector<Function::Ptr>
Engine::makeNextPrologueFunction(Partitioner &partitioner, rose_addr_t startVa) {
    std::vector<Function::Ptr> functions = partitioner.nextFunctionPrologue(startVa);
    BOOST_FOREACH (const Function::Ptr &function, functions)
        partitioner.attachOrMergeFunction(function);
    return functions;
}

void
Engine::discoverFunctions(Partitioner &partitioner) {
    rose_addr_t nextPrologueVa = 0;                     // where to search for function prologues
    rose_addr_t nextReadAddr = 0;                       // where to look for read-only function addresses

    while (1) {
        // Find as many basic blocks as possible by recursively following the CFG as we build it.
        discoverBasicBlocks(partitioner);

        // No pending basic blocks, so look for a function prologue. This creates a pending basic block for the function's
        // entry block, so go back and look for more basic blocks again.
        std::vector<Function::Ptr> newFunctions = makeNextPrologueFunction(partitioner, nextPrologueVa);
        if (!newFunctions.empty()) {
            nextPrologueVa = newFunctions[0]->address();   // avoid "+1" because it may overflow
            continue;
        }

        // Try looking for a function address mentioned in read-only memory
        if (settings_.partitioner.findingDataFunctionPointers) {
            if (Function::Ptr function = makeNextDataReferencedFunction(partitioner, nextReadAddr /*in,out*/)) {
                partitioner.attachFunction(function);
                continue;
            }
        }
        
        // Nothing more to do
        break;
    }

    // Try to attach basic blocks to functions
    makeCalledFunctions(partitioner);
    attachBlocksToFunctions(partitioner);
}

std::set<rose_addr_t>
Engine::attachDeadCodeToFunction(Partitioner &partitioner, const Function::Ptr &function, size_t maxIterations) {
    ASSERT_not_null(function);
    std::set<rose_addr_t> retval;

    for (size_t i=0; i<maxIterations; ++i) {
        // Find ghost edges
        std::set<rose_addr_t> ghosts = partitioner.functionGhostSuccessors(function);
        if (ghosts.empty())
            break;

        // Insert placeholders for all ghost edge targets
        partitioner.detachFunction(function);           // so we can modify its basic block ownership list
        BOOST_FOREACH (rose_addr_t ghost, ghosts) {
            if (retval.find(ghost)==retval.end()) {
                partitioner.insertPlaceholder(ghost);   // ensure a basic block gets created here
                function->insertBasicBlock(ghost);      // the function will own this basic block
                retval.insert(ghost);
            }
        }
        
        // If we're about to do more iterations then we should recursively discover instructions for pending basic blocks. Once
        // we've done that we should traverse the function's CFG to see if some of those new basic blocks are reachable and
        // should also be attached to the function.
        if (i+1 < maxIterations) {
            while (makeNextBasicBlock(partitioner)) /*void*/;
            partitioner.discoverFunctionBasicBlocks(function);
        }
    }

    partitioner.attachFunction(function);               // no-op if still attached
    return retval;
}

DataBlock::Ptr
Engine::attachPaddingToFunction(Partitioner &partitioner, const Function::Ptr &function) {
    ASSERT_not_null(function);
    if (DataBlock::Ptr padding = partitioner.matchFunctionPadding(function)) {
        partitioner.attachFunctionDataBlock(function, padding);
        return padding;
    }
    return DataBlock::Ptr();
}

std::vector<DataBlock::Ptr>
Engine::attachPaddingToFunctions(Partitioner &partitioner) {
    std::vector<DataBlock::Ptr> retval;
    BOOST_FOREACH (const Function::Ptr &function, partitioner.functions()) {
        if (DataBlock::Ptr padding = attachPaddingToFunction(partitioner, function))
            insertUnique(retval, padding, sortDataBlocks);
    }
    return retval;
}

size_t
Engine::attachAllSurroundedCodeToFunctions(Partitioner &partitioner) {
    size_t retval = 0;
    while (size_t n = attachSurroundedCodeToFunctions(partitioner)) {
        retval += n;
        discoverBasicBlocks(partitioner);
        makeCalledFunctions(partitioner);
        attachBlocksToFunctions(partitioner);
    }
    return retval;
}

// Assumes that each unused address interaval that's surrounded by a single function begins coincident with the beginning of an
// as yet undiscovered basic block and adds a basic block placeholder to the surrounding function.  This could be further
// improved by testing to see if the candidate address looks like a valid basic block.
size_t
Engine::attachSurroundedCodeToFunctions(Partitioner &partitioner) {
    size_t nNewBlocks = 0;
    if (partitioner.aum().isEmpty())
        return 0;
    rose_addr_t va = partitioner.aum().hull().least() + 1;
    while (va < partitioner.aum().hull().greatest()) {
        // Find an address interval that's unused and also executable.
        AddressInterval unusedAum = partitioner.aum().nextUnused(va);
        if (!unusedAum || unusedAum.greatest() > partitioner.aum().hull().greatest())
            break;
        AddressInterval interval = partitioner.memoryMap().within(unusedAum).require(MemoryMap::EXECUTABLE).available();
        if (interval == unusedAum) {
            // Is this interval immediately surrounded by a single function?
            typedef std::vector<Function::Ptr> Functions;
            Functions beforeFuncs = partitioner.functionsOverlapping(interval.least()-1);
            Functions afterFuncs = partitioner.functionsOverlapping(interval.greatest()+1);
            Functions enclosingFuncs(beforeFuncs.size());
            Functions::iterator final = std::set_intersection(beforeFuncs.begin(), beforeFuncs.end(),
                                                              afterFuncs.begin(), afterFuncs.end(), enclosingFuncs.begin());
            enclosingFuncs.resize(final-enclosingFuncs.begin());
            if (1 == enclosingFuncs.size()) {
                Function::Ptr function = enclosingFuncs[0];

                // Add the address to the function
                mlog[DEBUG] <<"attachSurroundedCodeToFunctions: basic block " <<StringUtility::addrToString(interval.least())
                            <<" is attached now to function " <<function->printableName() <<"\n";
                partitioner.detachFunction(function);
                function->insertBasicBlock(interval.least());
                partitioner.attachFunction(function);
                ++nNewBlocks;
            }
        }

        // Advance to next unused interval
        if (unusedAum.greatest() > partitioner.aum().hull().greatest())
            break;                                      // prevent possible overflow
        va = unusedAum.greatest() + 1;
    }
    return nNewBlocks;
}

void
Engine::attachBlocksToFunctions(Partitioner &partitioner) {
    std::vector<Function::Ptr> retval;
    BOOST_FOREACH (const Function::Ptr &function, partitioner.functions()) {
        partitioner.detachFunction(function);           // must be detached in order to modify block ownership
        partitioner.discoverFunctionBasicBlocks(function);
        partitioner.attachFunction(function);
    }
}

// Finds dead code and adds it to the function to which it seems to belong.
std::set<rose_addr_t>
Engine::attachDeadCodeToFunctions(Partitioner &partitioner, size_t maxIterations) {
    std::set<rose_addr_t> retval;
    BOOST_FOREACH (const Function::Ptr &function, partitioner.functions()) {
        std::set<rose_addr_t> deadVas = attachDeadCodeToFunction(partitioner, function, maxIterations);
        retval.insert(deadVas.begin(), deadVas.end());
    }
    return retval;
}

std::vector<DataBlock::Ptr>
Engine::attachSurroundedDataToFunctions(Partitioner &partitioner) {
    // Find executable addresses that are not yet used in the CFG/AUM
    AddressIntervalSet executableSpace;
    BOOST_FOREACH (const MemoryMap::Node &node, partitioner.memoryMap().nodes()) {
        if ((node.value().accessibility() & MemoryMap::EXECUTABLE) != 0)
            executableSpace.insert(node.key());
    }
    AddressIntervalSet unused = partitioner.aum().unusedExtent(executableSpace);

    // Iterate over the larged unused address intervals and find their surrounding functions
    std::vector<DataBlock::Ptr> retval;
    BOOST_FOREACH (const AddressInterval &interval, unused.intervals()) {
        if (interval.least()<=executableSpace.least() || interval.greatest()>=executableSpace.greatest())
            continue;
        typedef std::vector<Function::Ptr> Functions;
        Functions beforeFuncs = partitioner.functionsOverlapping(interval.least()-1);
        Functions afterFuncs = partitioner.functionsOverlapping(interval.greatest()+1);

        // What functions are in both sets?
        Functions enclosingFuncs(beforeFuncs.size());
        Functions::iterator final = std::set_intersection(beforeFuncs.begin(), beforeFuncs.end(),
                                                          afterFuncs.begin(), afterFuncs.end(), enclosingFuncs.begin());
        enclosingFuncs.resize(final-enclosingFuncs.begin());

        // Add the data block to all enclosing functions
        if (!enclosingFuncs.empty()) {
            BOOST_FOREACH (const Function::Ptr &function, enclosingFuncs) {
                DataBlock::Ptr dblock = partitioner.attachFunctionDataBlock(function, interval.least(), interval.size());
                insertUnique(retval, dblock, sortDataBlocks);
            }
        }
    }
    return retval;
}

void
Engine::updateAnalysisResults(Partitioner &partitioner) {
    Sawyer::Message::Stream info(mlog[INFO]);
    Sawyer::Stopwatch timer;
    info <<"post partition analysis";
    std::string separator = ": ";

    if (settings_.partitioner.doingPostFunctionNoop) {
        info <<separator <<"func-no-op";
        separator = ", ";
        Modules::nameNoopFunctions(partitioner);
    }

    if (settings_.partitioner.doingPostFunctionMayReturn) {
        info <<separator <<"may-return";
        separator = ", ";
        partitioner.allFunctionMayReturn();
    }

    if (settings_.partitioner.doingPostFunctionStackDelta) {
        info <<separator <<"stack-delta";
        separator = ", ";
        partitioner.allFunctionStackDelta();
    }

    if (settings_.partitioner.doingPostCallingConvention) {
        info <<separator <<"call-conv";
        separator = ", ";
        // Calling convention analysis uses a default convention to break recursion cycles in the CG.
        const CallingConvention::Dictionary &ccDict = partitioner.instructionProvider().callingConventions();
        partitioner.allFunctionCallingConvention(ccDict.empty() ? NULL : &ccDict.front());
    }

    info <<"; total " <<timer <<" seconds\n";
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Partitioner low-level stuff
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Called every time an instructionis added to a basic block.
bool
Engine::BasicBlockFinalizer::operator()(bool chain, const Args &args) {
    if (chain) {
        BasicBlock::Ptr bb = args.bblock;
        ASSERT_not_null(bb);
        ASSERT_require(bb->nInstructions() > 0);

        if (args.bblock->finalState() == NULL)
            return true;
        BaseSemantics::RiscOperatorsPtr ops = args.bblock->dispatcher()->get_operators();

        // Should we add an indeterminate CFG edge from this basic block?  For instance, a "JMP [ADDR]" instruction should get
        // an indeterminate edge if ADDR is a writable region of memory. There are two situations: ADDR is non-writable, in
        // which case RiscOperators::readMemory would have returned a free variable to indicate an indeterminate value, or ADDR
        // is writable but its MemoryMap::INITIALIZED bit is set to indicate it has a valid value already, in which case
        // RiscOperators::readMemory would have returned the value stored there but also marked the value as being
        // INDETERMINATE.  The SymbolicExpr::TreeNode::INDETERMINATE bit in the expression should have been carried along
        // so that things like "MOV EAX, [ADDR]; JMP EAX" will behave the same as "JMP [ADDR]".
        bool addIndeterminateEdge = false;
        size_t addrWidth = 0;
        BOOST_FOREACH (const BasicBlock::Successor &successor, args.partitioner.basicBlockSuccessors(args.bblock)) {
            if (!successor.expr()->is_number()) {       // BB already has an indeterminate successor?
                addIndeterminateEdge = false;
                break;
            } else if (!addIndeterminateEdge &&
                       (successor.expr()->get_expression()->flags() & SymbolicExpr::Node::INDETERMINATE) != 0) {
                addIndeterminateEdge = true;
                addrWidth = successor.expr()->get_width();
            }
        }

        // Add an edge
        if (addIndeterminateEdge) {
            ASSERT_require(addrWidth != 0);
            BaseSemantics::SValuePtr addr = ops->undefined_(addrWidth);
            args.bblock->insertSuccessor(addr);
            SAWYER_MESG(mlog[DEBUG]) <<args.bblock->printableName()
                                     <<": added indeterminate successor for initialized, non-constant memory read\n";
        }
    }
    return chain;
}

// Add basic block to worklist(s)
bool
Engine::BasicBlockWorkList::operator()(bool chain, const AttachedBasicBlock &args) {
    if (chain) {
        ASSERT_not_null(args.partitioner);
        const Partitioner *p = args.partitioner;

        // Basic block that is not yet discovered. We could use the special "undiscovered" CFG vertex, but there is no ordering
        // guarantee for its incoming edges.  We want to process undiscovered vertices in a depth-first manner, which is why we
        // maintain our own list instead.  The reason for depth-first discovery is that some analyses are recursive in nature
        // and we want to try to have children discovered and analyzed before we try to analyze the parent.  For instance,
        // may-return analysis for one vertex probably depends on the may-return analysis of its successors.
        if (args.bblock == NULL) {
            undiscovered_.pushBack(args.startVa);
            return chain;
        }

        // If a new function call is inserted and it has no E_CALL_RETURN edge and at least one of its callees has an
        // indeterminate value for its may-return analysis, then add this block to the list of blocks for which we may need to
        // later add a call-return edge. The engine can be configured to also just assume that all function calls may return
        // (or never return).
        if (p->basicBlockIsFunctionCall(args.bblock)) {
            ControlFlowGraph::ConstVertexIterator placeholder = p->findPlaceholder(args.startVa);
            boost::logic::tribool mayReturn;
            switch (engine_->functionReturnAnalysis()) {
                case MAYRETURN_ALWAYS_YES:
                    mayReturn = true;
                    break;
                case MAYRETURN_ALWAYS_NO:
                    mayReturn = false;
                    break;
                case MAYRETURN_DEFAULT_YES:
                case MAYRETURN_DEFAULT_NO: {
                    ASSERT_require(placeholder != p->cfg().vertices().end());
                    mayReturn = hasAnyCalleeReturn(*p, placeholder);
                    break;
                }
            }
            if (!hasCallReturnEdges(placeholder) && (mayReturn || boost::logic::indeterminate(mayReturn)))
                pendingCallReturn_.pushBack(args.startVa);
        }
    }
    return chain;
}

// Remove basic block from worklist(s). We probably don't really need to erase things since the items are revalidated when
// they're consumed from the list, and avoiding an erase will keep the list in its original order when higher level functions
// make a minor adjustment with a detach-adjust-attach sequence.  On the other hand, perhaps such adjustments SHOULD move the
// block to the top of the stack.
bool
Engine::BasicBlockWorkList::operator()(bool chain, const DetachedBasicBlock &args) {
    if (chain) {
        pendingCallReturn_.erase(args.startVa);
        processedCallReturn_.erase(args.startVa);
        finalCallReturn_.erase(args.startVa);
        undiscovered_.erase(args.startVa);
    }
    return chain;
}

// Move pendingCallReturn items into the finalCallReturn list and (re)sort finalCallReturn items according to the CFG so that
// descendents appear after their ancestors (i.e., descendents will be processed first since we always use popBack).  This is a
// fairly expensive operation: O((V+E) ln N) where V and E are the number of edges in the CFG and N is the number of addresses
// in the combined lists.
void
Engine::BasicBlockWorkList::moveAndSortCallReturn(const Partitioner &partitioner) {
    using namespace Sawyer::Container::Algorithm;       // graph traversals

    if (processedCallReturn().isEmpty())
        return;                                         // nothing to move, and finalCallReturn list was previously sorted

    std::set<rose_addr_t> pending;
    BOOST_FOREACH (rose_addr_t va, finalCallReturn_.items())
        pending.insert(va);
    BOOST_FOREACH (rose_addr_t va, processedCallReturn_.items())
        pending.insert(va);
    finalCallReturn_.clear();
    processedCallReturn_.clear();
    
    std::vector<bool> seen(partitioner.cfg().nVertices(), false);
    while (!pending.empty()) {
        rose_addr_t startVa = *pending.begin();
        ControlFlowGraph::ConstVertexIterator startVertex = partitioner.findPlaceholder(startVa);
        if (startVertex == partitioner.cfg().vertices().end()) {
            pending.erase(pending.begin());             // this worklist item is no longer valid
        } else {
            typedef DepthFirstForwardGraphTraversal<const ControlFlowGraph> Traversal;
            for (Traversal t(partitioner.cfg(), startVertex, ENTER_VERTEX|LEAVE_VERTEX); t; ++t) {
                if (t.event()==ENTER_VERTEX) {
                    // No need to follow this vertex if we processed it in some previous iteration of the "while" loop
                    if (seen[t.vertex()->id()])
                        t.skipChildren();
                    seen[t.vertex()->id()] = true;
                } else if (t.vertex()->value().type() == V_BASIC_BLOCK) {
                    rose_addr_t va = t.vertex()->value().address();
                    std::set<rose_addr_t>::iterator found = pending.find(va);
                    if (found != pending.end()) {
                        finalCallReturn().pushFront(va);
                        pending.erase(found);
                        if (pending.empty())
                            return;
                    }
                }
            }
        }
    }
}

// Return true if a new CFG edge was added.
bool
Engine::makeNextCallReturnEdge(Partitioner &partitioner, boost::logic::tribool assumeReturns) {
    Sawyer::Container::DistinctList<rose_addr_t> &workList = basicBlockWorkList_->pendingCallReturn();
    while (!workList.isEmpty()) {
        rose_addr_t va = workList.popBack();
        ControlFlowGraph::VertexIterator caller = partitioner.findPlaceholder(va);

        // Some sanity checks because it could be possible for this list to be out-of-date if the user monkeyed with the
        // partitioner's CFG adjuster.
        if (caller == partitioner.cfg().vertices().end()) {
            SAWYER_MESG(mlog[WARN]) <<StringUtility::addrToString(va) <<" was present on the basic block worklist "
                                    <<"but not in the CFG\n";
            continue;
        }
        BasicBlock::Ptr bb = caller->value().bblock();
        if (!bb)
            continue;
        if (!partitioner.basicBlockIsFunctionCall(bb))
            continue;
        if (hasCallReturnEdges(caller))
            continue;

        // If the new vertex lacks a call-return edge (tested above) and its callee has positive or indeterminate may-return
        // then we may need to add a call-return edge depending on whether assumeCallReturns is true.
        boost::logic::tribool mayReturn;
        Confidence confidence = PROVED;
        switch (settings_.partitioner.functionReturnAnalysis) {
            case MAYRETURN_ALWAYS_NO:
                mayReturn = false;
                confidence = ASSUMED;
                break;
            case MAYRETURN_ALWAYS_YES:
                mayReturn = true;
                confidence = ASSUMED;
                break;
            case MAYRETURN_DEFAULT_YES:
            case MAYRETURN_DEFAULT_NO:
                mayReturn = hasAnyCalleeReturn(partitioner, caller);
                if (boost::logic::indeterminate(mayReturn)) {
                    mayReturn = assumeReturns;
                    confidence = ASSUMED;
                }
                break;
        }

        if (mayReturn) {
            size_t nBits = partitioner.instructionProvider().instructionPointerRegister().get_nbits();
            partitioner.detachBasicBlock(bb);
            bb->insertSuccessor(bb->fallthroughVa(), nBits, E_CALL_RETURN, confidence);
            partitioner.attachBasicBlock(caller, bb);
            return true;
        } else if (!mayReturn) {
            return false;
        } else {
            // We're not sure yet whether a call-return edge should be inserted, so save vertex for later.
            ASSERT_require(boost::logic::indeterminate(mayReturn));
            basicBlockWorkList_->processedCallReturn().pushBack(va);
        }
    }
    return false;
}

// Discover a basic block's instructions for some placeholder that has no basic block yet.
BasicBlock::Ptr
Engine::makeNextBasicBlockFromPlaceholder(Partitioner &partitioner) {

    // Pick the first (as LIFO) item from the undiscovered worklist. Make sure the item is truly undiscovered
    while (!basicBlockWorkList_->undiscovered().isEmpty()) {
        rose_addr_t va = basicBlockWorkList_->undiscovered().popBack();
        ControlFlowGraph::VertexIterator placeholder = partitioner.findPlaceholder(va);
        if (placeholder == partitioner.cfg().vertices().end()) {
            mlog[WARN] <<"makeNextBasicBlockFromPlaceholder: block " <<StringUtility::addrToString(va)
                       <<" was on the undiscovered worklist but not in the CFG\n";
            continue;
        }
        ASSERT_require(placeholder->value().type() == V_BASIC_BLOCK);
        if (placeholder->value().bblock()) {
            SAWYER_MESG(mlog[DEBUG]) <<"makeNextBasicBlockFromPlacholder: block " <<StringUtility::addrToString(va)
                                     <<" was on the undiscovered worklist but is already discovered\n";
            continue;
        }
        BasicBlock::Ptr bb = partitioner.discoverBasicBlock(placeholder);
        partitioner.attachBasicBlock(placeholder, bb);
        return bb;
    }

    // The user probably monkeyed with basic blocks without notifying this engine, so just consume a block that we don't know
    // about. The order in which such placeholder blocks are discovered is arbitrary.
    if (partitioner.undiscoveredVertex()->nInEdges() > 0) {
        mlog[WARN] <<"partitioner engine undiscovered worklist is empty but CFG undiscovered vertex is not\n";
        ControlFlowGraph::VertexIterator placeholder = partitioner.undiscoveredVertex()->inEdges().begin()->source();
        ASSERT_require(placeholder->value().type() == V_BASIC_BLOCK);
        BasicBlock::Ptr bb = partitioner.discoverBasicBlock(placeholder);
        partitioner.attachBasicBlock(placeholder, bb);
        return bb;
    }

    // Nothing to discover
    return BasicBlock::Ptr();
}

// make a new basic block for an arbitrary placeholder
BasicBlock::Ptr
Engine::makeNextBasicBlock(Partitioner &partitioner) {
    ASSERT_not_null(basicBlockWorkList_);

    while (1) {
        // If there's an undiscovered basic block then discover it.
        if (BasicBlock::Ptr bb = makeNextBasicBlockFromPlaceholder(partitioner))
            return bb;

        // If there's a function call that needs a new call-return edge then add such an edge, but only if we know the callee
        // has a positive may-return analysis. If we don't yet know with certainty whether the call may return or will never
        // return then move the vertex to the processedCallReturn list to save it for later (this happens as part of
        // makeNextCallReturnEdge().
        if (!basicBlockWorkList_->pendingCallReturn().isEmpty()) {
            makeNextCallReturnEdge(partitioner, boost::logic::indeterminate);
            continue;
        }

        // If we've previously tried to add call-return edges and failed then try again but this time assume the block
        // may return or never returns depending on the assumeFunctionsReturn property.  We use the finalCallReturn list, which
        // is always sorted so that descendent blocks are analyzed before their ancestors (according to the CFG as it existed
        // when the sort was performed, and subject to tie breaking for cycles). We only re-sort the finalCallReturn list when
        // we add something to it, and we add things in batches since the sorting is expensive.
        if (!basicBlockWorkList_->processedCallReturn().isEmpty() || !basicBlockWorkList_->finalCallReturn().isEmpty()) {
            ASSERT_require(basicBlockWorkList_->pendingCallReturn().isEmpty());
            if (!basicBlockWorkList_->processedCallReturn().isEmpty())
                basicBlockWorkList_->moveAndSortCallReturn(partitioner);
            if (!basicBlockWorkList_->finalCallReturn().isEmpty()) { // moveAndSortCallReturn might have pruned list
                basicBlockWorkList_->pendingCallReturn().pushBack(basicBlockWorkList_->finalCallReturn().popBack());
                makeNextCallReturnEdge(partitioner, partitioner.assumeFunctionsReturn());
            }
            continue;
        }

        // Nothing to do
        break;
    }

    // Nothing more to do; all lists are empty
    ASSERT_require(basicBlockWorkList_->undiscovered().isEmpty());
    ASSERT_require(basicBlockWorkList_->pendingCallReturn().isEmpty());
    ASSERT_require(basicBlockWorkList_->processedCallReturn().isEmpty());
    ASSERT_require(basicBlockWorkList_->finalCallReturn().isEmpty());
    return BasicBlock::Ptr();
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Build AST
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SgAsmBlock*
Engine::buildAst(const std::vector<std::string> &fileNames) {
    Partitioner partitioner = partition(fileNames);
    return Modules::buildAst(partitioner, interp_);
}

SgAsmBlock*
Engine::buildAst(const std::string &fileName) {
    return buildAst(std::vector<std::string>(1, fileName));
}

} // namespace
} // namespace
} // namespace
