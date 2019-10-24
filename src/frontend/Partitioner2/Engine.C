#include <sage3basic.h>
#include <rosePublicConfig.h>

#include <AsmUnparser_compat.h>
#include <BinaryDebugger.h>
#include <BinaryLoader.h>
#include <BinarySerialIo.h>
#include <CommandLine.h>
#include <Diagnostics.h>
#include <DisassemblerM68k.h>
#include <DisassemblerPowerpc.h>
#include <DisassemblerX86.h>
#include <SRecord.h>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <Partitioner2/Engine.h>
#include <Partitioner2/Modules.h>
#include <Partitioner2/ModulesElf.h>
#include <Partitioner2/ModulesLinux.h>
#include <Partitioner2/ModulesM68k.h>
#include <Partitioner2/ModulesPe.h>
#include <Partitioner2/ModulesPowerpc.h>
#include <Partitioner2/ModulesX86.h>
#include <Partitioner2/Semantics.h>
#include <Partitioner2/Utility.h>
#include <Sawyer/FileSystem.h>
#include <Sawyer/GraphAlgorithm.h>
#include <Sawyer/GraphTraversal.h>
#include <Sawyer/Stopwatch.h>

#ifdef ROSE_HAVE_LIBYAML
#include <yaml-cpp/yaml.h>
#endif

using namespace Rose::Diagnostics;

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Utility functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
Engine::init() {
    ASSERT_require(map_ == NULL);
    Rose::initialize(NULL);
    functionMatcherThunks_ = ThunkPredicates::functionMatcherThunks();
    functionSplittingThunks_ = ThunkPredicates::allThunks();
#if ROSE_PARTITIONER_EXPENSIVE_CHECKS == 1
    static bool emitted = false;
    if (!emitted) {
        emitted = true;
        mlog[WARN] <<"ROSE_PARTITIONER_EXPENSIVE_CHECKS is enabled\n";
    }
#endif
}

void
Engine::reset() {
    interp_ = NULL;
    binaryLoader_ = BinaryLoader::Ptr();
    disassembler_ = NULL;
    map_ = MemoryMap::Ptr();
    basicBlockWorkList_ = BasicBlockWorkList::instance(this, settings_.partitioner.functionReturnAnalysisMaxSorts);
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
    try {
        std::vector<std::string> specimenNames = parseCommandLine(args, purpose, description).unreachedArgs();
        if (specimenNames.empty())
            throw std::runtime_error("no binary specimen specified; see --help");
        return buildAst(specimenNames);
    } catch (const std::runtime_error &e) {
        if (settings().engine.exitOnError) {
            mlog[FATAL] <<e.what() <<"\n";
            exit(1);
        } else {
            throw;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Command-line parsing
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Sawyer::CommandLine::SwitchGroup
Engine::loaderSwitches() {
    return loaderSwitches(settings_.loader);
}

// class method
Sawyer::CommandLine::SwitchGroup
Engine::loaderSwitches(LoaderSettings &settings) {
    using namespace Sawyer::CommandLine;
    SwitchGroup sg("Loader switches");
    sg.name("loader");
    sg.doc("The loader is responsible for mapping a specimen into the address space used for disassembling and analysis. "
           "ROSE uses a virtualized address space (the MemoryMap class) to isolate the address space of a specimen from "
           "the address space of ROSE itself. These switches have no effect if the input is a ROSE Binary Analysis (RBA) "
           "file, since the loader steps in such an input have already been completed.");

    sg.insert(Switch("remove-zeros")
              .argument("size", nonNegativeIntegerParser(settings.deExecuteZerosThreshold), "128")
              .doc("This switch causes execute permission to be removed from sequences of contiguous zero bytes. The "
                   "switch argument is the minimum number of consecutive zeros that will trigger the removal, and "
                   "defaults to 128.  An argument of zero disables the removal.  Each interval of zeros is narrowed "
                   "according to the @s{remove-zeros-narrow} switch before execute permission is removed. When this switch "
                   "is not specified at all, this tool assumes a value of " +
                   StringUtility::plural(settings.deExecuteZerosThreshold, "bytes") + "."));
    sg.insert(Switch("remove-zeros-narrow")
              .argument("@v{begin},@v{end}",
                        listParser(nonNegativeIntegerParser(settings.deExecuteZerosLeaveAtFront))
                        ->nextMember(nonNegativeIntegerParser(settings.deExecuteZerosLeaveAtBack))
                        ->exactly(2))
              .doc("If @s{remove-zeros} is active then each interval of zeros detected by that analysis is narrowed by "
                   "removing @v{begin} and @v{end} bytes from the interval before execute permission is removed. This "
                   "effectively makes the analysis less greedy and less apt to remove zeros from the ends and beginnings "
                   "of adjacent instructions.  The default is to narrow each interval by " +
                   StringUtility::plural(settings.deExecuteZerosLeaveAtFront, "bytes") + " at the beginning and " +
                   StringUtility::plural(settings.deExecuteZerosLeaveAtBack, "bytes") + " at the end."));

    sg.insert(Switch("executable")
              .intrinsicValue(true, settings.memoryIsExecutable)
              .doc("Adds execute permission to the entire memory map, aside from regions excluded by @s{remove-zeros}. "
                   "The executable bit determines whether the partitioner is allowed to make instructions at some address, "
                   "so using this switch is an easy way to make the disassembler think that all of memory may contain "
                   "instructions.  The default is to not add executable permission to all of memory."));
    sg.insert(Switch("no-executable")
              .key("executable")
              .intrinsicValue(false, settings.memoryIsExecutable)
              .hidden(true));

    sg.insert(Switch("data")
              .argument("state", enumParser<MemoryDataAdjustment>(settings.memoryDataAdjustment)
                        ->with("constant", DATA_IS_CONSTANT)
                        ->with("initialized", DATA_IS_INITIALIZED)
                        ->with("no-change", DATA_NO_CHANGE))
              .doc("Globally adjusts the memory map to influence how the partitioner treats reads from concrete memory "
                   "addresses.  The values for @v{state} are one of these words:"

                   "@named{constant}{Causes write access to be removed from all memory segments and the partitioner treats "
                   "memory reads as returning a concrete value." +
                   std::string(DATA_IS_CONSTANT==settings.memoryDataAdjustment?" This is the default.":"") + "}"

                   "@named{initialized}{Causes the initialized bit to be added to all memory segments and the partitioner "
                   "treats reads from such addresses to return a concrete value, plus if the address is writable, "
                   "indeterminate values." +
                   std::string(DATA_IS_INITIALIZED==settings.memoryDataAdjustment?" This is the default.":"") + "}"

                   "@named{no-change}{Causes the engine to not change data access bits for memory." +
                   std::string(DATA_NO_CHANGE==settings.memoryDataAdjustment?" This is the default.":"") + "}"

                   "One of the things influenced by these access flags is indirect jumps, like x86 \"jmp [@v{addr}]\". If "
                   "@v{addr} is constant memory, then the \"jmp\" has a single constant successor; if @v{addr} is "
                   "non-constant but initialized, then the \"jmp\" will have a single constant successor and indeterminate "
                   "successors; otherwise it will have only indeterminate successors."));

    sg.insert(Switch("link-objects")
              .intrinsicValue(true, settings.linkObjectFiles)
              .doc("Object files (\".o\" files) typically don't contain information about how the object is mapped into "
                   "virtual memory, and thus machine instructions are not found. This switch causes the linker to be run "
                   "on all the object files mentioned on the command line and, if successful, the output file is "
                   "analyzed instead of the objects.  The linker command is specified with the @s{linker} switch. The "
                   "@s{no-link-objects} switch disables linking object files. The default is to " +
                   std::string(settings.linkObjectFiles ? "" : "not ") + "perform the link."));
    sg.insert(Switch("no-link-objects")
              .key("link-objects")
              .intrinsicValue(false, settings.linkObjectFiles)
              .hidden(true));

    sg.insert(Switch("link-archives")
              .intrinsicValue(true, settings.linkStaticArchives)
              .doc("Library archives (\".a\" files) contain object files that typically don't contain information about "
                   "how the object is mapped into virtual memory, and thus machine instructions are not found. This switch "
                   "causes the linker to be run on all the library archives mentioned on the command line and, if successful, "
                   "the output file is analyzed instead of the archives.  The linker command is specified with the @s{linker} "
                   "switch. The @s{no-link-archives} switch disables linking archives. The default is to " +
                   std::string(settings.linkStaticArchives ? "" : "not ") + "perform the link."));
    sg.insert(Switch("no-link-archives")
              .key("link-archives")
              .intrinsicValue(false, settings.linkStaticArchives)
              .hidden(true));

    sg.insert(Switch("linker")
              .argument("command", anyParser(settings.linker))
              .doc("Shell command that runs the linker if object files and/or library archives are being linked.  The command "
                   "should include two special variables: \"%o\" is replaced by the name of the output file, and \"%f\" is "
                   "replaced by a space-separated list of input files (object files and/or library archives) in the same order "
                   "that they were specified on the command-line.  Since the substituted files names are properly escaped using "
                   "Bourne shell syntax, the \"%o\" and \"%f\" should not appear in quotes. If the linker fails, the object "
                   "and archive files are processed without linking.  The default link command is \"" +
                   StringUtility::cEscape(settings.linker) + "\"."));

    return sg;
}

Sawyer::CommandLine::SwitchGroup
Engine::disassemblerSwitches() {
    return disassemblerSwitches(settings_.disassembler);
}

// class method
Sawyer::CommandLine::SwitchGroup
Engine::disassemblerSwitches(DisassemblerSettings &settings) {
    using namespace Sawyer::CommandLine;
    SwitchGroup sg("Disassembler switches");
    sg.name("disassemble");
    sg.doc("These switches affect the disassembler proper, which is the software responsible for decoding machine "
           "instruction bytes into ROSE internal representations.  The disassembler only decodes instructions at "
           "given addresses and is not responsible for determining what addresses of the virtual address space are decoded. "
           "These switches have no effect if the input is a ROSE Binary Analysis (RBA) file, since the disassembler steps "
           "in such an input have already been completed.");

    sg.insert(Switch("isa")
              .argument("architecture", anyParser(settings.isaName))
              .doc("Name of instruction set architecture.  If no name is specified then the architecture is obtained from "
                   "the binary container (ELF, PE). The following ISA names are supported: " +
                   StringUtility::joinEnglish(Disassembler::isaNames()) + "."));

    return sg;
}

Sawyer::CommandLine::SwitchGroup
Engine::partitionerSwitches() {
    return partitionerSwitches(settings_.partitioner);
}

// class method
Sawyer::CommandLine::SwitchGroup
Engine::partitionerSwitches(PartitionerSettings &settings) {
    using namespace Sawyer::CommandLine;
    SwitchGroup sg("Partitioner switches");
    sg.name("partition");
    sg.doc("The partitioner is the part of ROSE that drives a disassembler. While the disassembler knows how to decode "
           "a machine instruction to an internal representation, the partitioner knows where to decode. These switches have "
           "no effect if the input is a ROSE Binary Analysis (RBA) file, since the partitioner steps in such an input "
           "have already been completed.");

    sg.insert(Switch("start")
              .argument("addresses", listParser(nonNegativeIntegerParser(settings.startingVas)))
              .whichValue(SAVE_ALL)
              .explosiveLists(true)
              .doc("List of addresses where recursive disassembly should start in addition to addresses discovered by "
                   "other methods. Each address listed by this switch will be considered the entry point of a function. "
                   "This switch may appear multiple times, each of which may have multiple comma-separated addresses."));

    sg.insert(Switch("use-semantics")
              .intrinsicValue(true, settings.base.usingSemantics)
              .doc("The partitioner can either use quick and naive methods of determining instruction characteristics, or "
                   "it can use slower but more accurate methods, such as symbolic semantics.  This switch enables use of "
                   "the slower symbolic semantics, or the feature can be disabled with @s{no-use-semantics}. Furthermore, "
                   "instruction semantics will use an SMT solver if one is specified, which can make the analysis even "
                   "slower. The default is to " + std::string(settings.base.usingSemantics?"":"not ") +
                   "use semantics."));
    sg.insert(Switch("no-use-semantics")
              .key("use-semantics")
              .intrinsicValue(false, settings.base.usingSemantics)
              .hidden(true));

    sg.insert(Switch("semantic-memory")
              .argument("type", enumParser<SemanticMemoryParadigm>(settings.semanticMemoryParadigm)
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
                   std::string(LIST_BASED_MEMORY == settings.semanticMemoryParadigm ? "list" : "map") +
                   "-based paradigm."));

    sg.insert(Switch("follow-ghost-edges")
              .intrinsicValue(true, settings.followingGhostEdges)
              .doc("When discovering the instructions for a basic block, treat instructions individually rather than "
                   "looking for opaque predicates.  The @s{no-follow-ghost-edges} switch turns this off.  The default "
                   "is " + std::string(settings.followingGhostEdges?"true":"false") + "."));
    sg.insert(Switch("no-follow-ghost-edges")
              .key("follow-ghost-edges")
              .intrinsicValue(false, settings.followingGhostEdges)
              .hidden(true));

    sg.insert(Switch("allow-discontiguous-blocks")
              .intrinsicValue(true, settings.discontiguousBlocks)
              .doc("This setting allows basic blocks to contain instructions that are discontiguous in memory as long as "
                   "the other requirements for a basic block are still met. Discontiguous blocks can be formed when a "
                   "compiler fails to optimize away an opaque predicate for a conditional branch, or when basic blocks "
                   "are scattered in memory by the introduction of unconditional jumps.  The @s{no-allow-discontiguous-blocks} "
                   "switch disables this feature and can slightly improve partitioner performance by avoiding cases where "
                   "an unconditional branch initially creates a larger basic block which is later discovered to be "
                   "multiple blocks.  The default is to " +
                   std::string(settings.discontiguousBlocks?"":"not ") +
                   "allow discontiguous basic blocks."));
    sg.insert(Switch("no-allow-discontiguous-blocks")
              .key("allow-discontiguous-blocks")
              .intrinsicValue(false, settings.discontiguousBlocks)
              .hidden(true));

    sg.insert(Switch("max-bblock-size")
              .argument("n", nonNegativeIntegerParser(settings.maxBasicBlockSize))
              .doc("Limit the size of basic blocks to @v{n} instructions. If a basic block would contain more than @v{n} "
                   "instructions then it is split into multiple basic blocks.  Limiting the block size is useful in order "
                   "to prevent long analysis times for intra-basic block semantics, but reduces the amount of information "
                   "available to some analyses. If @v{n} is zero then no limit is enforced.  The default is " +
                   StringUtility::numberToString(settings.maxBasicBlockSize) + "."));

    sg.insert(Switch("ip-rewrite")
              .argument("old", nonNegativeIntegerParser(settings.ipRewrites))
              .argument("new", nonNegativeIntegerParser(settings.ipRewrites))
              .whichValue(SAVE_ALL)
              .doc("Rewrite the global control flow graph so that any edges that would have gone to address @v{old} instead "
                   "go to @v{new}. This switch may appear multiple times."));

    sg.insert(Switch("find-function-padding")
              .intrinsicValue(true, settings.findingFunctionPadding)
              .doc("Cause each built-in and user-defined function padding analysis to run. The purpose of these "
                   "analyzers is to look for padding such as zero bytes and certain instructions like no-ops that occur "
                   "prior to the lowest address of a function and attach them to the function as static data.  The "
                   "@s{no-find-function-padding} switch turns this off.  The default is to " +
                   std::string(settings.findingFunctionPadding?"":"not ") + "search for padding."));
    sg.insert(Switch("no-find-function-padding")
              .key("find-function-padding")
              .intrinsicValue(false, settings.findingFunctionPadding)
              .hidden(true));

    sg.insert(Switch("find-dead-code")
              .intrinsicValue(true, settings.findingDeadCode)
              .doc("Use ghost edges (non-followed control flow from branches with opaque predicates) to locate addresses "
                   "for unreachable code, then recursively discover basic blocks at those addresses and add them to the "
                   "same function.  The @s{no-find-dead-code} switch turns this off.  The default is " +
                   std::string(settings.findingDeadCode?"true":"false") + "."));
    sg.insert(Switch("no-find-dead-code")
              .key("find-dead-code")
              .intrinsicValue(false, settings.findingDeadCode)
              .hidden(true));

    sg.insert(Switch("find-thunks")
              .intrinsicValue(true, settings.findingThunks)
              .doc("Search for common thunk patterns in areas of executable memory that have not been previously "
                   "discovered to contain other functions.  When this switch is enabled, the function-searching callbacks "
                   "include patterns to match thunks.  This switch does not cause the thunk's instructions to be "
                   "detached as a separate function from the thunk's target function; that's handled by the "
                   "@s{split-thunks} switch.  The @s{no-find-thunks} switch turns thunk searching off. The default "
                   "is to " + std::string(settings.findingThunks ? "" : "not ") + "search for thunks."));
    sg.insert(Switch("no-find-thunks")
              .key("find-thunks")
              .intrinsicValue(false, settings.findingThunks)
              .hidden(true));

    sg.insert(Switch("split-thunks")
              .intrinsicValue(true, settings.splittingThunks)
              .doc("Look for common thunk patterns at the start of existing functions and split off those thunk "
                   "instructions to their own separate function.  The @s{no-split-thunks} switch turns this feature "
                   "off.  The default is to " + std::string(settings.splittingThunks?"":"not ") + "split thunks into their own "
                   "functions."));
    sg.insert(Switch("no-split-thunks")
              .key("split-thunks")
              .intrinsicValue(false, settings.splittingThunks)
              .hidden(true));

    sg.insert(Switch("pe-scrambler")
              .argument("dispatcher_address", nonNegativeIntegerParser(settings.peScramblerDispatcherVa))
              .doc("Simulate the action of the PEScrambler dispatch function in order to rewrite CFG edges.  Any edges "
                   "that go into the specified @v{dispatcher_address} are immediately rewritten so they appear to go "
                   "instead to the function contained in the dispatcher table which normally immediately follows the "
                   "dispatcher function.  The dispatcher function is quite easy to find in a call graph because nearly "
                   "everything calls it -- it will likely have far and away more callers than anything else.  Setting the "
                   "address to zero disables this module (which is the default)."));

    sg.insert(Switch("intra-function-code")
              .argument("npasses", nonNegativeIntegerParser(settings.findingIntraFunctionCode), "10")
              .doc("Near the end of processing, a pass is made over the entire address space to find executable memory "
                   "that doesn't yet belong to any known function but is surrounded by a single function. A basic block "
                   "is created for each such region after which a recursive basic block discover phase ensues in order "
                   "find additional blocks reachable by the control flow. This process is repeated up to @v{npasses} "
                   "times, or until no new addresses are found. For backward compatibility, this switch also acts as "
                   "a boolean: @s{intra-function-code} and @s{no-intra-function-code} are equivalent to setting the "
                   "number of passes to ten and zero, respectively. The default is " +
                   StringUtility::plural(settings.findingIntraFunctionCode, "passes") + "."));
    sg.insert(Switch("no-intra-function-code")
              .key("intra-function-code")
              .intrinsicValue((size_t)0, settings.findingIntraFunctionCode)
              .hidden(true));

    sg.insert(Switch("intra-function-data")
              .intrinsicValue(true, settings.findingIntraFunctionData)
              .doc("Near the end of processing, if there are regions of unused memory that are immediately preceded and "
                   "followed by the same function then add that region of memory to that function as a static data block. "
                   "The @s{no-intra-function-data} switch turns this feature off.  The default is to " +
                   std::string(settings.findingIntraFunctionData?"":"not ") + "perform this analysis."));
    sg.insert(Switch("no-intra-function-data")
              .key("intra-function-data")
              .intrinsicValue(false, settings.findingIntraFunctionData)
              .hidden(true));

    sg.insert(Switch("inter-function-calls")
              .intrinsicValue(true, settings.findingInterFunctionCalls)
              .doc("Near the end of processing look for function calls that occur in the executable regions between "
                   "existing functions, and turn the call targets into functions. The analysis uses instruction semantics "
                   "to find the call sites rather than looking for architecture-specific call instructions, and attempts "
                   "to prune away things that are not legitimate calls. The @s{no-inter-function-calls} switch turns this "
                   "feature off. The default is to " + std::string(settings.findingInterFunctionCalls?"":"not ") +
                   "perform this analysis."));
    sg.insert(Switch("no-inter-function-calls")
              .key("inter-function-calls")
              .intrinsicValue(false, settings.findingInterFunctionCalls)
              .hidden(true));

    sg.insert(Switch("called-functions")
              .intrinsicValue(true, settings.findingFunctionCallFunctions)
              .doc("Look for function call instructions or sequences of instructions with similar behavior and assume "
                   "that the target address is the entry point for a function under most circumstances. The "
                   "@s{no-called-functions} switch turns this feature off, which can be useful when analyzing virtual "
                   "memory where targets of call-like sequences have not been initialized (such as in object files). "
                   "The default is to " + std::string(settings.findingFunctionCallFunctions?"":"not ") + "perform this "
                   "analysis."));
    sg.insert(Switch("no-called-functions")
              .key("called-functions")
              .intrinsicValue(false, settings.findingFunctionCallFunctions)
              .hidden(true));

    sg.insert(Switch("entry-functions")
              .intrinsicValue(true, settings.findingEntryFunctions)
              .doc("Create functions at the program entry point(s). The @s{no-entry-functions} switch turns this feature "
                   "off. The default is to " + std::string(settings.findingEntryFunctions?"":"not ") + "perform this analysis."));
    sg.insert(Switch("no-entry-functions")
              .key("entry-functions")
              .intrinsicValue(false, settings.findingEntryFunctions)
              .hidden(true));

    sg.insert(Switch("error-functions")
              .intrinsicValue(true, settings.findingErrorFunctions)
              .doc("Create functions based on error handling information that might be present in the container's tables. "
                   "The @s{no-error-functions} switch turns this feature off. The default is to " +
                   std::string(settings.findingErrorFunctions?"":"not ") + "perform this analysis."));
    sg.insert(Switch("no-error-functions")
              .key("error-functions")
              .intrinsicValue(false, settings.findingErrorFunctions)
              .hidden(true));

    sg.insert(Switch("import-functions")
              .intrinsicValue(true, settings.findingImportFunctions)
              .doc("Create functions based on import information that might be present in the container's tables. The "
                   "@s{no-import-functions} switch turns this feature off. The default is to " +
                   std::string(settings.findingImportFunctions?"":"not ") + "perform this analysis."));
    sg.insert(Switch("no-import-functions")
              .key("import-functions")
              .intrinsicValue(false, settings.findingImportFunctions)
              .hidden(true));

    sg.insert(Switch("export-functions")
              .intrinsicValue(true, settings.findingExportFunctions)
              .doc("Create functions based on export information that might be present in the container's tables. The "
                   "@s{no-export-functions} switch turns this feature off. The default is to " +
                   std::string(settings.findingExportFunctions?"":"not ") + "perform this analysis."));
    sg.insert(Switch("no-export-functions")
              .key("export-functions")
              .intrinsicValue(false, settings.findingExportFunctions)
              .hidden(true));

    sg.insert(Switch("symbol-functions")
              .intrinsicValue(true, settings.findingSymbolFunctions)
              .doc("Create functions based on symbol tables that might be present in the container. The @s{no-symbol-functions} "
                   "switch turns this feature off. The default is to " + std::string(settings.findingSymbolFunctions?"":"not ") +
                   "perform this analysis."));
    sg.insert(Switch("no-symbol-functions")
              .key("symbol-functions")
              .intrinsicValue(false, settings.findingSymbolFunctions)
              .hidden(true));

    sg.insert(Switch("data-functions")
              .intrinsicValue(true, settings.findingDataFunctionPointers)
              .doc("Scan non-executable areas of memory to find pointers to functions.  This analysis can be disabled "
                   "with @s{no-data-functions}. The default is to " + std::string(settings.findingDataFunctionPointers?"":"not ") +
                   "perform this analysis."));
    sg.insert(Switch("no-data-functions")
              .key("data-functions")
              .intrinsicValue(false, settings.findingDataFunctionPointers)
              .hidden(true));

    sg.insert(Switch("code-functions")
              .intrinsicValue(true, settings.findingCodeFunctionPointers)
              .doc("Scan instructions to find pointers to functions. This analysis can be disabled with "
                   "@s{no-code-functions}. The default is to " + std::string(settings.findingCodeFunctionPointers?"":"not ") +
                   "perform this analysis."));
    sg.insert(Switch("no-code-functions")
              .key("code-functions")
              .intrinsicValue(false, settings.findingCodeFunctionPointers)
              .hidden(true));

    sg.insert(Switch("interrupt-vector")
              .argument("addresses", addressIntervalParser(settings.interruptVector))
              .doc("A table containing addresses of functions invoked for various kinds of interrupts. " +
                   AddressIntervalParser::docString() + " The length and contents of the table is architecture "
                   "specific, and the disassembler will use available information about the architecture to decode the "
                   "table.  If a single address is specified, then the length of the table is architecture dependent, "
                   "otherwise the entire table is read."));

    sg.insert(Switch("name-constants")
              .intrinsicValue(true, settings.namingConstants)
              .doc("Scans the instructions and gives labels to constants that refer to entities that have that address "
                   "and also have a name.  For instance, if a constant refers to the beginning of a file section then "
                   "the constant will be labeled so it has the same name as the section.  The @s{no-name-constants} "
                   "turns this feature off. The default is to " + std::string(settings.namingConstants?"":"not ") +
                   "do this step."));
    sg.insert(Switch("no-name-constants")
              .key("name-constants")
              .intrinsicValue(false, settings.namingConstants)
              .hidden(true));

    sg.insert(Switch("name-strings")
              .intrinsicValue(true, settings.namingStrings)
              .doc("Scans the instructions and gives labels to constants that refer to the beginning of string literals. "
                   "The label is usually the first few characters of the string.  The @s{no-name-strings} turns this "
                   "feature off. The default is to " + std::string(settings.namingStrings?"":"not ") + "do this step."));
    sg.insert(Switch("no-name-strings")
              .key("name-strings")
              .intrinsicValue(false, settings.namingStrings)
              .hidden(true));

    sg.insert(Switch("name-syscalls")
              .intrinsicValue(true, settings.namingSyscalls)
              .doc("Scans all instructions and tries to give names to system calls.  The names are assigned as comments "
                   "to the instruction that performs the system call. The system call names are parsed from the Linux header "
                   "files on the system running the analysis (not necessarily where ROSE was compiled); this can be adjusted "
                   "with the @s{syscall-header} switch.  The @s{no-name-syscalls} turns this feature off. The default is to " +
                   std::string(settings.namingSyscalls?"":"not ") + "do this step."));
    sg.insert(Switch("no-name-syscalls")
              .key("name-syscalls")
              .intrinsicValue(false, settings.namingSyscalls)
              .hidden(true));

    sg.insert(Switch("syscall-header")
              .argument("filename", anyParser(settings.syscallHeader))
              .doc("Name of the header file from which to obtain the system call ID-name mapping. The default is to look "
                   "in standard places such as /usr/include/asm/unistd_32.h."));

    sg.insert(Switch("post-analysis")
              .intrinsicValue(true, settings.doingPostAnalysis)
              .doc("Run all enabled post-partitioning analysis functions.  For instance, calculate stack deltas for each "
                   "instruction, and may-return analysis for each function.  The individual analyses are enabled and "
                   "disabled separately with other @s{post-*}{noerror} switches. Some of these analyses will only work if "
                   "instruction semantics are enabled (see @s{use-semantics}).  The @s{no-post-analysis} switch turns "
                   "this off, although analysis will still be performed where it is needed for partitioning.  The "
                   "default is to " + std::string(settings.doingPostAnalysis?"":"not ") + "perform the post analysis phase."));
    sg.insert(Switch("no-post-analysis")
              .key("post-analysis")
              .intrinsicValue(false, settings.doingPostAnalysis)
              .hidden(true));

    sg.insert(Switch("post-function-noop")
              .intrinsicValue(true, settings.doingPostFunctionNoop)
              .doc("Run a function no-op analysis for each function if post-partitioning analysis is enabled with the "
                   "@s{post-analysis} switch. This analysis tries to determine whether each function is effectively a no-op. "
                   "Functions that are no-ops are given names (if they don't already have one) that's indicative of "
                   "being a no-op. The @s{no-post-function-noop} switch disables this analysis. The default is that "
                   "this analysis is " + std::string(settings.doingPostFunctionNoop?"enabled":"disabled") + "."));
    sg.insert(Switch("no-post-function-noop")
              .key("post-function-noop")
              .intrinsicValue(false, settings.doingPostFunctionNoop)
              .hidden(true));

    sg.insert(Switch("post-may-return")
              .intrinsicValue(true, settings.doingPostFunctionMayReturn)
              .doc("Run the may-return analysis for each function if post-partitioning analysis is enabled with the "
                   "@s{post-analysis} switch. This analysis tries to quickly determine if a function might return a "
                   "value to the caller.  The @s{no-post-may-return} switch disables this analysis. The default is that "
                   "this analysis is " + std::string(settings.doingPostFunctionMayReturn?"enabled":"disabled") + "."));
    sg.insert(Switch("no-post-may-return")
              .key("post-may-return")
              .intrinsicValue(false, settings.doingPostFunctionMayReturn)
              .hidden(true));

    sg.insert(Switch("post-stack-delta")
              .intrinsicValue(true, settings.doingPostFunctionStackDelta)
              .doc("Run the stack-delta analysis for each function if post-partitioning analysis is enabled with the "
                   "@s{post-analysis} switch.  This is a data-flow analysis that tries to determine whether the function "
                   "has a constant net effect on the stack pointer and what that effect is.  For instance, when a caller "
                   "is reponsible for cleaning up function call arguments on a 32-bit architecture with a downward-growing "
                   "stack then the stack delta is usually +4, representing the fact that the called function popped the "
                   "return address from the stack.  The @s{no-post-stack-delta} switch disables this analysis. The default "
                   "is that this analysis is " + std::string(settings.doingPostFunctionStackDelta?"enabled":"disabled") + "."));
    sg.insert(Switch("no-post-stack-delta")
              .key("post-stack-delta")
              .intrinsicValue(false, settings.doingPostFunctionStackDelta)
              .hidden(true));

    sg.insert(Switch("post-calling-convention")
              .intrinsicValue(true, settings.doingPostCallingConvention)
              .doc("Run the calling-convention analysis for each function if post-partitioning analysis is enabled with the "
                   "@s{post-analysis} switch. This relatively expensive analysis uses use-def, stack-delta, memory variable "
                   "discovery, and data-flow to determine characteristics of the function and then matches it against a "
                   "dictionary of calling conventions appropriate for the architecture.  The @s{no-post-calling-convention} "
                   "disables this analysis. The default is that this analysis is " +
                   std::string(settings.doingPostCallingConvention?"enabled":"disabled") + "."));
    sg.insert(Switch("no-post-calling-convention")
              .key("post-calling-convention")
              .intrinsicValue(false, settings.doingPostCallingConvention)
              .hidden(true));

    sg.insert(Switch("functions-return")
              .argument("how", enumParser<FunctionReturnAnalysis>(settings.functionReturnAnalysis)
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

    sg.insert(Switch("functions-return-sort")
              .argument("n", nonNegativeIntegerParser(settings.functionReturnAnalysisMaxSorts))
              .doc("If function return analysis is occurring (@s{functions-return}) then functions are sorted according to "
                   "their depth in the global control flow graph, arbitrarily removing cycles. The functions are analyzed "
                   "starting at the leaves in order to minimize forward dependencies. For large specimens, this sorting "
                   "might occur often and is expensive. Therefore, the sorting is limited to the specified number of "
                   "occurrences, after which unsorted lists are used. The default is " +
                   StringUtility::plural(settings.functionReturnAnalysisMaxSorts, "sorting operations") + "."));

    sg.insert(Switch("call-branch")
              .intrinsicValue(true, settings.base.checkingCallBranch)
              .doc("When determining whether a basic block is a function call, also check whether the callee discards "
                   "the return address. If so, then the apparent call is perhaps not a true function call.  The "
                   "@s{no-call-branch} switch disables this analysis. The default is that this analysis is " +
                   std::string(settings.base.checkingCallBranch ? "enabled" : "disabled") + "."));
    sg.insert(Switch("no-call-branch")
              .key("call-branch")
              .intrinsicValue(false, settings.base.checkingCallBranch)
              .hidden(true));

    sg.insert(Switch("demangle-names")
              .intrinsicValue(true, settings.demangleNames)
              .doc("Causes all function names to be run through a demangler process to undo the name mangling that "
                   "normally happens with some compilers.  The @s{no-demangle-names} switch disables this step. The "
                   "default is to " + std::string(settings.demangleNames ? "" : "not ") + "do this step."));
    sg.insert(Switch("no-demangle-names")
              .key("demangle-names")
              .intrinsicValue(false, settings.demangleNames)
              .hidden(true));

    return sg;
}

Sawyer::CommandLine::SwitchGroup
Engine::engineSwitches() {
    return engineSwitches(settings_.engine);
}

// class method
Sawyer::CommandLine::SwitchGroup
Engine::engineSwitches(EngineSettings &settings) {
    using namespace Sawyer::CommandLine;
    SwitchGroup sg = Rose::CommandLine::genericSwitches();
    sg.name("global");

    sg.insert(Switch("config")
              .argument("names", listParser(anyParser(settings.configurationNames), ":"))
              .explosiveLists(true)
              .whichValue(SAVE_ALL)
              .doc("Directories containing configuration files, or configuration files themselves.  A directory is searched "
                   "recursively searched for files whose names end with \".json\" or and each file is parsed and used to "
                   "to configure the partitioner.  The JSON file contents is defined by the Carnegie Mellon University "
                   "Software Engineering Institute. It should have a top-level \"config.exports\" table whose keys are "
                   "function names and whose values are have a \"function.delta\" integer. The delta does not include "
                   "popping the return address from the stack in the final RET instruction.  Function names of the form "
                   "\"lib:func\" are translated to the ROSE format \"func@@lib\"."));
    return sg;
}

Sawyer::CommandLine::SwitchGroup
Engine::astConstructionSwitches() {
    return astConstructionSwitches(settings_.astConstruction);
}

// class method
Sawyer::CommandLine::SwitchGroup
Engine::astConstructionSwitches(AstConstructionSettings &settings) {
    using namespace Sawyer::CommandLine;
    SwitchGroup sg("AST construction switches");
    sg.name("ast");
    sg.doc("These switches control how an abstract syntax tree (AST) is generated from partitioner results.");

    sg.insert(Switch("allow-empty-global-block")
              .intrinsicValue(true, settings.allowEmptyGlobalBlock)
              .doc("Allows creation of an empty AST if the partitioner does not find any functions. The "
                   "@s{no-allow-empty-global-block} switch causes a null AST to be returned instead. The default is to " +
                   std::string(settings.allowEmptyGlobalBlock ? "create an empty " : "not create an ") + "AST."));
    sg.insert(Switch("no-allow-empty-global-block")
              .key("allow-empty-global-block")
              .intrinsicValue(false, settings.allowEmptyGlobalBlock)
              .hidden(true));

    sg.insert(Switch("allow-empty-functions")
              .intrinsicValue(true, settings.allowFunctionWithNoBasicBlocks)
              .doc("Allows creation of an AST that has functions with no instructions. This can happen, for instance, when "
                   "an analysis indicated that a particular virtual address is the start of a function but no memory is "
                   "mapped at that address. This is common for things like functions from shared libraries that have not "
                   "been linked in before the analysis starts.  The @s{no-allow-empty-functions} will instead elide all "
                   "empty functions from the AST. The default is to " +
                   std::string(settings.allowFunctionWithNoBasicBlocks ? "allow " : "elide ") + "empty functions."));
    sg.insert(Switch("no-allow-empty-functions")
              .key("allow-empty-functions")
              .intrinsicValue(false, settings.allowFunctionWithNoBasicBlocks)
              .hidden(true));

    sg.insert(Switch("allow-empty-basic-blocks")
              .intrinsicValue(true, settings.allowEmptyBasicBlocks)
              .doc("Allows creation of an AST that has basic blocks with no instructions. This can happen when an analysis "
                   "indicates that a basic block exists at a particular virtual address but no memory is mapped at that "
                   "address. The @s{no-allow-empty-basic-blocks} will instead elide all empty blocks from the AST. The "
                   "default is to " + std::string(settings.allowEmptyBasicBlocks ? "allow " : "elide ") + "empty blocks."));
    sg.insert(Switch("no-allow-empty-basic-blocks")
              .key("allow-empty-basic-blocks")
              .intrinsicValue(false, settings.allowEmptyBasicBlocks)
              .hidden(true));

    sg.insert(Switch("copy-instructions")
              .intrinsicValue(true, settings.copyAllInstructions)
              .doc("Causes all instructions to be deep-copied from the partitioner's instruction provider into the AST. "
                   "Although this slows down AST construction and increases memory since SageIII nodes are not garbage "
                   "collected, copying instructions ensures that the AST is a tree. Turning off the copying with the "
                   "@s{no-copy-instructions} switch will result in the AST being a lattice if the partitioner has "
                   "determined that two or more functions contain the same basic block, and therefore the same instructions. "
                   "The default is to " + std::string(settings.copyAllInstructions ? "" : "not ") + "copy instructions.\n\n"

                   "Note that within the partitioner data structures it's the basic blocks that are shared when two or "
                   "more functions point to the same block, but within the AST, sharing is at the instruction "
                   "(SgAsmInstruction) level. It was designed this way because it's common to store function-specific "
                   "analysis results as attributes attached to basic blocks (SgAsmBlock nodes) in the AST and sharing at "
                   "the block level would break those programs. Users that store analysis results by attaching them to "
                   "partitioner basic blocks (Partitioner2::BasicBlock) should be aware that those blocks can be shared "
                   "among functions."));
    sg.insert(Switch("no-copy-instructions")
              .key("copy-instructions")
              .intrinsicValue(false, settings.copyAllInstructions)
              .hidden(true));

    return sg;
}

std::string
Engine::specimenNameDocumentation() {
    return ("The following names are recognized for binary specimens:"

            "@bullet{If the name does not match any of the following patterns then it is assumed to be the name of a "
            "file containing a specimen that is a binary container format such as ELF or PE.}"

            "@bullet{If the name begins with the string \"map:\" then it is treated as a memory map resource string that "
            "adjusts a memory map by inserting part of a file. " + MemoryMap::insertFileDocumentation() + "}"

            "@bullet{If the name begins with the string \"data:\" then its data portion is parsed as a byte sequence "
            "which is then inserted into the memory map. " + MemoryMap::insertDataDocumentation() + "}"

            "@bullet{If the name begins with the string \"proc:\" then it is treated as a process resource string that "
            "adjusts a memory map by reading the process' memory. " + MemoryMap::insertProcessDocumentation() + "}"

            "@bullet{If the name begins with the string \"run:\" then it is first treated like a normal file by ROSE's "
            "\"fontend\" function, and then during a second pass it will be loaded natively under a debugger, run until "
            "a mapped executable address is reached, and then its memory is copied into ROSE's memory map possibly "
            "overwriting existing parts of the map.  This can be useful when the user wants accurate information about "
            "how that native loader links in shared objects since ROSE's linker doesn't always have identical behavior. "
            "The syntax of this form is \"run:@v{options}:@v{filename}\" where @v{options} is a comma-separated list of "
            "options that control the finer details. The following options are recognized:"

            "@named{replace}{This option causes the memory map to be entirely replaced with the process map rather than "
            "the default behavior of the process map augmenting the map created by the ROSE loader.  This can be useful "
            "if ROSE's internal loader resulted in wrong addresses, although symbols will then probably also be pointing to "
            "those wrong addresses and will be dangling when those addresses are removed from the map.}}"

            "@bullet{If the file name begins with the string \"srec:\" then it is treated as Motorola S-Record format. "
            "Mapping attributes are stored after the first column and before the second; the file name appears after the "
            "second colon.  The only mapping attributes supported at this time are permissions, specified as an equal "
            "sign ('=') followed by zero or more of the letters \"r\", \"w\", and \"x\" to signify read, write, and "
            "execute permissions. If no letters are present after the equal sign, then the memory has no permissions; "
            "if the equal sign itself is also missing then the segments are given read, write, and execute permission.}"

            "@bullet{If the name ends with \".srec\" and doesn't match the previous list of prefixes then it is assumed "
            "to be a text file containing Motorola S-Records and will be parsed as such and loaded into the memory map "
            "with read, write, and execute permissions.}"

            "@bullet{If the name ends with \".rba\" and doesn't match the previous list of prefixes then it is assumed "
            "to be a ROSE Binary Analysis file that contains a serialized partitioner and an optional AST. Use of an "
            "RBA file to describe a specimen precludes the use of any other inputs for that specimen. Furthermore, since "
            "an RBA file represents a fully parsed and disassembled specimen, the command-line switches that control "
            "the parsing and disassembly are ignored.}"

            "When more than one mechanism is used to load a single coherent specimen, the normal names are processed first "
            "by passing them all to ROSE's \"frontend\" function, which results in an initial memory map.  The other names "
            "are then processed in the order they appear, possibly overwriting parts of the map. RBA files have special "
            "handling described above.");
}

Sawyer::CommandLine::Parser
Engine::commandLineParser(const std::string &purpose, const std::string &description) {
    using namespace Sawyer::CommandLine;
    Parser parser =
        CommandLine::createEmptyParser(purpose.empty() ? std::string("analyze binary specimen") : purpose, description);
    parser.groupNameSeparator("-");                     // ROSE defaults to ":", which is sort of ugly
    parser.doc("Synopsis", "@prop{programName} [@v{switches}] @v{specimen_names}");
    parser.doc("Specimens", specimenNameDocumentation());
    parser.with(engineSwitches());
    parser.with(loaderSwitches());
    parser.with(disassemblerSwitches());
    parser.with(partitionerSwitches());
    parser.with(astConstructionSwitches());
    return parser;
}

Sawyer::CommandLine::ParserResult
Engine::parseCommandLine(int argc, char *argv[], const std::string &purpose, const std::string &description) {
    try {
        std::vector<std::string> args;
        for (int i=1; i<argc; ++i)
            args.push_back(argv[i]);
        return parseCommandLine(args, purpose, description);
    } catch (const std::runtime_error &e) {
        if (settings().engine.exitOnError) {
            mlog[FATAL] <<e.what() <<"\n";
            exit(1);
        } else {
            throw;
        }
    }
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
Engine::isRbaFile(const std::string &name) {
    return boost::ends_with(name, ".rba");
}

bool
Engine::isNonContainer(const std::string &name) {
    return (boost::starts_with(name, "map:")  ||        // map file directly into MemoryMap
            boost::starts_with(name, "data:") ||        // map data directly into MemoryMap
            boost::starts_with(name, "proc:") ||        // map process memory into MemoryMap
            boost::starts_with(name, "run:")  ||        // run a process in a debugger, then map into MemoryMap
            boost::starts_with(name, "srec:") ||        // Motorola S-Record format
            boost::ends_with(name, ".srec")   ||        // Motorola S-Record format
            isRbaFile(name));                           // ROSE Binary Analysis file
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
    try {
        interp_ = NULL;
        map_ = MemoryMap::Ptr();
        checkSettings();

        // Prune away things we recognize as not being binary containers.
        std::vector<boost::filesystem::path> containerFiles;
        BOOST_FOREACH (const std::string &fileName, fileNames) {
            if (boost::starts_with(fileName, "run:") && fileName.size()>4) {
                static size_t colon1 = 3;
                size_t colon2 = fileName.find(':', colon1+1);
                if (colon2 == std::string::npos) {
                    // [Robb Matzke 2017-07-24]: deprecated: use two colons for consistency with other schemas
                    containerFiles.push_back(fileName.substr(colon1+1));
                } else {
                    containerFiles.push_back(fileName.substr(colon2+1));
                }
            } else if (!isNonContainer(fileName)) {
                containerFiles.push_back(fileName);
            }
        }

        // Try to link all the .o and .a files
        bool linkerFailed = false;
        Sawyer::FileSystem::TemporaryFile linkerOutput;
        if (!settings_.loader.linker.empty()) {
            std::vector<boost::filesystem::path> filesToLink, nonLinkedFiles;
            BOOST_FOREACH (const boost::filesystem::path &file, containerFiles) {
                if (settings_.loader.linkObjectFiles && ModulesElf::isObjectFile(file)) {
                    filesToLink.push_back(file);
                } else if (settings_.loader.linkStaticArchives && ModulesElf::isStaticArchive(file)) {
                    filesToLink.push_back(file);
                } else {
                    nonLinkedFiles.push_back(file);
                }
            }
            if (!filesToLink.empty()) {
                linkerOutput.stream().close();          // will be written by linker command
                if (ModulesElf::tryLink(settings_.loader.linker, linkerOutput.name().native(), filesToLink, mlog[WARN])) {
                    containerFiles = nonLinkedFiles;
                    containerFiles.push_back(linkerOutput.name().native());
                } else {
                    mlog[ERROR] <<"linking objects and/or archives failed; falling back to internal (incomplete) linker\n";
                    filesToLink.clear();
                    linkerFailed = true;
                }
            }
        }

        // If we have *.a files that couldn't be linked, extract all their members and replace the archive file with the list
        // of object files.
        Sawyer::FileSystem::TemporaryDirectory tempDir;
        if (linkerFailed) {
            std::vector<boost::filesystem::path> expandedList;
            BOOST_FOREACH (const boost::filesystem::path &file, containerFiles) {
                if (ModulesElf::isStaticArchive(file)) {
                    std::vector<boost::filesystem::path> objects = ModulesElf::extractStaticArchive(tempDir.name(), file);
                    if (objects.empty())
                        mlog[WARN] <<"empty static archive \"" <<StringUtility::cEscape(file.native()) <<"\"\n";
                    BOOST_FOREACH (const boost::filesystem::path &objectFile, objects)
                        expandedList.push_back(objectFile.native());
                } else {
                    expandedList.push_back(file);
                }
            }
            containerFiles = expandedList;
        }

        // Process through ROSE's frontend()
        if (!containerFiles.empty()) {
#if 0 // [Robb Matzke 2019-01-29]: old method calling ::frontend
            std::vector<std::string> frontendArgs;
            frontendArgs.push_back("/proc/self/exe");       // I don't think frontend actually uses this
            frontendArgs.push_back("-rose:binary");
            frontendArgs.push_back("-rose:read_executable_file_format_only");
            BOOST_FOREACH (const boost::filesystem::path &file, containerFiles)
                frontendArgs.push_back(file.native());
            SgProject *project = ::frontend(frontendArgs);
#else // [Robb Matzke 2019-01-29]: new method calling Engine::roseFrontendReplacement
            SgProject *project = roseFrontendReplacement(containerFiles);
#endif
            ASSERT_not_null(project);                       // an exception should have been thrown

            std::vector<SgAsmInterpretation*> interps = SageInterface::querySubTree<SgAsmInterpretation>(project);
            if (interps.empty())
                throw std::runtime_error("a binary specimen container must have at least one SgAsmInterpretation");
            interp_ = interps.back();    // windows PE is always after DOS
            ASSERT_require(areContainersParsed());
        }

        ASSERT_require(!areSpecimensLoaded());
        return interp_;
    } catch (const std::runtime_error &e) {
        if (settings().engine.exitOnError) {
            mlog[FATAL] <<e.what() <<"\n";
            exit(1);
        } else {
            throw;
        }
    }
}

// Replacement for ::frontend, which is a complete mess, in order create a project containing multiple files. Nothing
// special happens to any of the input file names--that should have already been done by this point. All the fileNames
// are expected to be names of existing container (ELF, PE, etc) files that will result in an AST (non-container files
// typically are loaded into virtual memory and have no AST since they have very little structure).
SgProject *
Engine::roseFrontendReplacement(const std::vector<boost::filesystem::path> &fileNames) {
    ASSERT_forbid(fileNames.empty());

    // Create the SgAsmGenericFiles (not a type of SgFile), one per fileName, and add them to a SgAsmGenericFileList node. Each
    // SgAsmGenericFile has one or more file headers (e.g., ELF files have one, PE files have two).
    SgAsmGenericFileList *fileList = new SgAsmGenericFileList;
    BOOST_FOREACH (const boost::filesystem::path &fileName, fileNames) {
        SAWYER_MESG(mlog[TRACE]) <<"parsing " <<fileName <<"\n";
        SgAsmGenericFile *file = SgAsmExecutableFileFormat::parseBinaryFormat(fileName.native().c_str());
        ASSERT_not_null(file);
#ifdef ROSE_HAVE_LIBDWARF
        readDwarf(file);
#endif
        fileList->get_files().push_back(file); file->set_parent(fileList);
    }
    SAWYER_MESG(mlog[DEBUG]) <<"parsed " <<StringUtility::plural(fileList->get_files().size(), "container files") <<"\n";

    // The SgBinaryComposite (type of SgFile) points to the list of SgAsmGenericFile nodes created above.
    // FIXME[Robb Matzke 2019-01-29]: The defaults set here should be set in the SgBinaryComposite constructor instead.
    // FIXME[Robb Matzke 2019-01-29]: A SgBinaryComposite represents many files, not just one, so some of these settings
    //                                don't make much sense.
    SgBinaryComposite *binaryComposite = new SgBinaryComposite;
    binaryComposite->initialization(); // SgFile::initialization
    binaryComposite->set_skipfinalCompileStep(true);
    binaryComposite->set_genericFileList(fileList); fileList->set_parent(binaryComposite);
    binaryComposite->set_sourceFileUsesBinaryFileExtension(true);
    binaryComposite->set_outputLanguage(SgFile::e_Binary_language);
    binaryComposite->set_inputLanguage(SgFile::e_Binary_language);
    binaryComposite->set_binary_only(true);
    binaryComposite->set_requires_C_preprocessor(false);
    //binaryComposite->set_isObjectFile(???) -- makes no sense since the composite can be multiple files of different types
    binaryComposite->set_sourceFileNameWithPath(boost::filesystem::absolute(fileNames[0]).native()); // best we can do
    binaryComposite->set_sourceFileNameWithoutPath(fileNames[0].filename().native());                // best we can do
    binaryComposite->initializeSourcePosition(fileNames[0].native());                                // best we can do
    binaryComposite->set_originalCommandLineArgumentList(std::vector<std::string>(1, fileNames[0].native())); // best we can do
    ASSERT_not_null(binaryComposite->get_file_info());

    // Create one or more SgAsmInterpretation nodes. If all the SgAsmGenericFile objects are ELF files, then there's one
    // SgAsmInterpretation that points to them all. If all the SgAsmGenericFile objects are PE files, then there's two
    // SgAsmInterpretation nodes: one for all the DOS parts of the files, and one for all the PE parts of the files.
    std::vector<std::pair<SgAsmExecutableFileFormat::ExecFamily, SgAsmInterpretation*> > interpretations;
    BOOST_FOREACH (SgAsmGenericFile *file, fileList->get_files()) {
        SgAsmGenericHeaderList *headerList = file->get_headers();
        ASSERT_not_null(headerList);
        BOOST_FOREACH (SgAsmGenericHeader *header, headerList->get_headers()) {
            SgAsmGenericFormat *format = header->get_exec_format();
            ASSERT_not_null(format);

            // Find or create the interpretation that holds this family of headers.
            SgAsmInterpretation *interpretation = NULL;
            for (size_t i=0; i<interpretations.size() && !interpretation; ++i) {
                if (interpretations[i].first == format->get_family())
                    interpretation = interpretations[i].second;
            }
            if (!interpretation) {
                interpretation = new SgAsmInterpretation;
                interpretations.push_back(std::make_pair(format->get_family(), interpretation));
            }

            // Add the header to the interpretation. This isn't an AST parent/child link, so don't set the parent ptr.
            SgAsmGenericHeaderList *interpHeaders = interpretation->get_headers();
            ASSERT_not_null(interpHeaders);
            interpHeaders->get_headers().push_back(header);
        }
    }
    SAWYER_MESG(mlog[DEBUG]) <<"created " <<StringUtility::plural(interpretations.size(), "interpretation nodes") <<"\n";

    // Put all the interpretations in a list
    SgAsmInterpretationList *interpList = new SgAsmInterpretationList;
    for (size_t i=0; i<interpretations.size(); ++i) {
        SgAsmInterpretation *interpretation = interpretations[i].second;
        interpList->get_interpretations().push_back(interpretation); interpretation->set_parent(interpList);
    }
    ASSERT_require(interpList->get_interpretations().size() == interpretations.size());

    // Add the interpretation list to the SgBinaryComposite node
    binaryComposite->set_interpretations(interpList); interpList->set_parent(binaryComposite);

    // The project
    SgProject *project = new SgProject;
    project->get_fileList().push_back(binaryComposite);
    binaryComposite->set_parent(project); // FIXME[Robb Matzke 2019-01-29]: is this the correct parent?

    return project;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Memory map creation (loading)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool
Engine::areSpecimensLoaded() const {
    return map_!=NULL && !map_->isEmpty();
}

BinaryLoader::Ptr
Engine::obtainLoader(const BinaryLoader::Ptr &hint) {
    if (!binaryLoader_ && interp_) {
        if ((binaryLoader_ = BinaryLoader::lookup(interp_))) {
            binaryLoader_ = binaryLoader_->clone();
            binaryLoader_->performingRemap(true);
            binaryLoader_->performingDynamicLinking(false);
            binaryLoader_->performingRelocations(false);
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
    map_ = MemoryMap::instance();
    if (interp_ && interp_->get_map())
        *map_ = *interp_->get_map();
}

void
Engine::loadNonContainers(const std::vector<std::string> &fileNames) {
    ASSERT_not_null(map_);
    BOOST_FOREACH (const std::string &fileName, fileNames) {
        if (boost::starts_with(fileName, "map:")) {
            std::string resource = fileName.substr(3);  // remove "map", leaving colon and rest of string
            map_->insertFile(resource);
        } else if (boost::starts_with(fileName, "data:")) {
            std::string resource = fileName.substr(4);  // remove "data:", leaving colon and the rest of the string
            map_->insertData(resource);
        } else if (boost::starts_with(fileName, "proc:")) {
            std::string resource = fileName.substr(4);  // remove "proc", leaving colon and the rest of the string
            map_->insertProcess(resource);
        } else if (boost::starts_with(fileName, "run:")) {
            // Parse the options between the two colons in "run:OPTIONS:EXECUTABLE"
            static const size_t colon1 = 3;             // index of first colon in fileName
            const size_t colon2 = fileName.find(':', colon1+1); // index of second colon in FileName
            std::string exeName;
            bool doReplace = false;
            if (colon2 == std::string::npos) {
                // [Robb Matzke 2017-07-24]: deprecated. ROSE used to accept "run:/name/of/executable" which is a
                // different syntax than what all the other methods accept (the others all have two colons).
                exeName = fileName.substr(colon1+1);
            } else {
                std::string optionsStr = fileName.substr(colon1+1, colon2-(colon1+1));
                exeName = fileName.substr(colon2+1);
                std::vector<std::string> options;
                boost::split(options, optionsStr, boost::is_any_of(","));
                BOOST_FOREACH (const std::string &option, options) {
                    if (option.empty()) {
                    } else if ("replace" == option) {
                        doReplace = true;
                    } else {
                        throw std::runtime_error("option \"" + StringUtility::cEscape(option) + "\" not recognized"
                                                 " in resource \"" + StringUtility::cEscape(fileName) + "\"");
                    }
                }
            }

            Debugger::Specimen subordinate(exeName);
            subordinate.flags()
                .set(Debugger::CLOSE_FILES)
                .set(Debugger::REDIRECT_INPUT)
                .set(Debugger::REDIRECT_OUTPUT)
                .set(Debugger::REDIRECT_ERROR);
            Debugger::Ptr debugger = Debugger::instance(subordinate);

            // Set breakpoints for all executable addresses in the memory map created by the Linux kernel. Since we're doing
            // this before the first instruction executes, no shared libraries have been loaded yet. However, the dynamic
            // linker itself is present as are the vdso and vsyscall segments.  We don't want to set breakpoints in anything
            // that the dynamic linker might call because the whole purpose of the "run:" URL is to get an accurate memory map
            // of the process after shared libraries are loaded. We assume that the kernel has loaded the executable at the
            // lowest address.
            MemoryMap::Ptr procMap = MemoryMap::instance();
            procMap->insertProcess(debugger->isAttached(), MemoryMap::Attach::NO);
            procMap->require(MemoryMap::EXECUTABLE).keep();
            if (procMap->isEmpty())
                throw std::runtime_error(exeName + " has no executable addresses");
            std::string name = procMap->segments().begin()->name(); // lowest segment is always part of the main executable
            BOOST_FOREACH (const MemoryMap::Node &node, procMap->nodes()) {
                if (node.value().name() == name)        // usually just one match; names are like "proc:123(/bin/ls)"
                    debugger->setBreakpoint(node.key());
            }

            debugger->runToBreakpoint();
            if (debugger->isTerminated())
                throw std::runtime_error(exeName + " " + debugger->howTerminated() + " without reaching a breakpoint");
            if (doReplace)
                map_->clear();
            map_->insertProcess(debugger->isAttached(), MemoryMap::Attach::NO);
            debugger->terminate();
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
    ASSERT_not_null(map_);
    if (settings_.loader.memoryIsExecutable)
        map_->any().changeAccess(MemoryMap::EXECUTABLE, 0);
    Modules::deExecuteZeros(map_/*in,out*/, settings_.loader.deExecuteZerosThreshold,
                            settings_.loader.deExecuteZerosLeaveAtFront, settings_.loader.deExecuteZerosLeaveAtBack);

    switch (settings_.loader.memoryDataAdjustment) {
        case DATA_IS_CONSTANT:
            map_->any().changeAccess(0, MemoryMap::WRITABLE);
            break;
        case DATA_IS_INITIALIZED:
            map_->any().changeAccess(MemoryMap::INITIALIZED, 0);
            break;
        case DATA_NO_CHANGE:
            break;
    }
}

MemoryMap::Ptr
Engine::loadSpecimens(const std::string &fileName) {
    return loadSpecimens(std::vector<std::string>(1, fileName));
}

MemoryMap::Ptr
Engine::loadSpecimens(const std::vector<std::string> &fileNames) {
    try {
        if (!areContainersParsed())
            parseContainers(fileNames);
        if (!map_)
            map_ = MemoryMap::instance();
        loadContainers(fileNames);
        loadNonContainers(fileNames);
        adjustMemoryMap();
        map_->shrinkUnshare();
        return map_;
    } catch (const std::runtime_error &e) {
        if (settings().engine.exitOnError) {
            mlog[FATAL] <<e.what() <<"\n";
            exit(1);
        } else {
            throw;
        }
    }
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
    if (!map_ || map_->isEmpty())
        mlog[WARN] <<"Engine::createBarePartitioner: using an empty memory map\n";
}

Partitioner
Engine::createBarePartitioner() {
    Sawyer::Message::Stream info(mlog[MARCH]);

    checkCreatePartitionerPrerequisites();
    Partitioner p(disassembler_, map_);
    if (p.memoryMap() && p.memoryMap()->byteOrder() == ByteOrder::ORDER_UNSPECIFIED)
        p.memoryMap()->byteOrder(disassembler_->byteOrder());
    p.settings(settings_.partitioner.base);
    p.progress(progress_);

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

    // Make sure the stream of constants found in instruction ASTs is updated whenever the CFG is adjusted.
    if (settings_.partitioner.findingCodeFunctionPointers) {
        codeFunctionPointers_ = CodeConstants::instance();
        p.cfgAdjustmentCallbacks().prepend(codeFunctionPointers_);
    }

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
    if (!settings_.partitioner.ipRewrites.empty()) {
        std::vector<Modules::IpRewriter::AddressPair> rewrites;
        for (size_t i=0; i+1 < settings_.partitioner.ipRewrites.size(); i += 2)
            rewrites.push_back(std::make_pair(settings_.partitioner.ipRewrites[i+0], settings_.partitioner.ipRewrites[i+1]));
        p.basicBlockCallbacks().append(Modules::IpRewriter::instance(rewrites));
    }
    if (settings_.partitioner.followingGhostEdges)
        p.basicBlockCallbacks().append(Modules::AddGhostSuccessors::instance());
    if (!settings_.partitioner.discontiguousBlocks)
        p.basicBlockCallbacks().append(Modules::PreventDiscontiguousBlocks::instance());
    if (settings_.partitioner.maxBasicBlockSize > 0)
        p.basicBlockCallbacks().append(Modules::BasicBlockSizeLimiter::instance(settings_.partitioner.maxBasicBlockSize));

    // PEScrambler descrambler
    if (settings_.partitioner.peScramblerDispatcherVa) {
        ModulesPe::PeDescrambler::Ptr cb = ModulesPe::PeDescrambler::instance(settings_.partitioner.peScramblerDispatcherVa);
        cb->nameKeyAddresses(p);                        // give names to certain PEScrambler things
        p.basicBlockCallbacks().append(cb);
        p.attachFunction(Function::instance(settings_.partitioner.peScramblerDispatcherVa,
                                            p.addressName(settings_.partitioner.peScramblerDispatcherVa),
                                            SgAsmFunction::FUNC_PESCRAMBLER_DISPATCH));
    }

    return boost::move(p);
}

Partitioner
Engine::createGenericPartitioner() {
    checkCreatePartitionerPrerequisites();
    Partitioner p = createBarePartitioner();
    p.functionPrologueMatchers().push_back(ModulesX86::MatchHotPatchPrologue::instance());
    p.functionPrologueMatchers().push_back(ModulesX86::MatchStandardPrologue::instance());
    p.functionPrologueMatchers().push_back(ModulesX86::MatchAbbreviatedPrologue::instance());
    p.functionPrologueMatchers().push_back(ModulesX86::MatchEnterPrologue::instance());
    p.functionPrologueMatchers().push_back(ModulesPowerpc::MatchStwuPrologue::instance());
    if (settings_.partitioner.findingThunks)
        p.functionPrologueMatchers().push_back(Modules::MatchThunk::instance(functionMatcherThunks()));
    p.functionPrologueMatchers().push_back(ModulesX86::MatchRetPadPush::instance());
    p.functionPrologueMatchers().push_back(ModulesM68k::MatchLink::instance());
    p.basicBlockCallbacks().append(ModulesX86::FunctionReturnDetector::instance());
    p.basicBlockCallbacks().append(ModulesM68k::SwitchSuccessors::instance());
    p.basicBlockCallbacks().append(ModulesX86::SwitchSuccessors::instance());
    p.basicBlockCallbacks().append(libcStartMain_ = ModulesLinux::LibcStartMain::instance());
    return boost::move(p);
}

Partitioner
Engine::createTunedPartitioner() {
    obtainDisassembler();

    if (dynamic_cast<DisassemblerM68k*>(disassembler_)) {
        checkCreatePartitionerPrerequisites();
        Partitioner p = createBarePartitioner();
        p.functionPrologueMatchers().push_back(ModulesM68k::MatchLink::instance());
        p.basicBlockCallbacks().append(ModulesM68k::SwitchSuccessors::instance());
        return boost::move(p);
    }

    if (dynamic_cast<DisassemblerX86*>(disassembler_)) {
        checkCreatePartitionerPrerequisites();
        Partitioner p = createBarePartitioner();
        p.functionPrologueMatchers().push_back(ModulesX86::MatchHotPatchPrologue::instance());
        p.functionPrologueMatchers().push_back(ModulesX86::MatchStandardPrologue::instance());
        p.functionPrologueMatchers().push_back(ModulesX86::MatchEnterPrologue::instance());
        if (settings_.partitioner.findingThunks)
            p.functionPrologueMatchers().push_back(Modules::MatchThunk::instance(functionMatcherThunks_));
        p.functionPrologueMatchers().push_back(ModulesX86::MatchRetPadPush::instance());
        p.basicBlockCallbacks().append(ModulesX86::FunctionReturnDetector::instance());
        p.basicBlockCallbacks().append(ModulesX86::SwitchSuccessors::instance());
        p.basicBlockCallbacks().append(ModulesLinux::SyscallSuccessors::instance(p, settings_.partitioner.syscallHeader));
        p.basicBlockCallbacks().append(libcStartMain_ = ModulesLinux::LibcStartMain::instance());
        return boost::move(p);
    }

    if (dynamic_cast<DisassemblerPowerpc*>(disassembler_)) {
        checkCreatePartitionerPrerequisites();
        Partitioner p = createBarePartitioner();
        p.functionPrologueMatchers().push_back(ModulesPowerpc::MatchStwuPrologue::instance());
        return boost::move(p);
    }

    return createGenericPartitioner();
}

Partitioner
Engine::createPartitionerFromAst(SgAsmInterpretation *interp) {
    ASSERT_not_null(interp);
    interp_ = interp;
    map_ = MemoryMap::Ptr();
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
        BasicBlock::Ptr bblock = BasicBlock::instance(blockAst->get_address(), partitioner);
        bblock->comment(blockAst->get_comment());

        // Instructions
        const SgAsmStatementPtrList &stmts = blockAst->get_statementList();
        for (SgAsmStatementPtrList::const_iterator si=stmts.begin(); si!=stmts.end(); ++si) {
            if (SgAsmInstruction *insn = isSgAsmInstruction(*si))
                bblock->append(partitioner, insn);
        }

        // Successors
        const SgAsmIntegerValuePtrList &successors = blockAst->get_successors();
        BOOST_FOREACH (SgAsmIntegerValueExpression *ival, successors)
            bblock->insertSuccessor(ival->get_absoluteValue(), ival->get_significantBits());
        if (!blockAst->get_successors_complete()) {
            size_t nbits = partitioner.instructionProvider().instructionPointerRegister().nBits();
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
        function->reasonComment(funcAst->get_reasonComment());

        BOOST_FOREACH (SgAsmBlock *blockAst, SageInterface::querySubTree<SgAsmBlock>(funcAst)) {
            if (blockAst->has_instructions())
                function->insertBasicBlock(blockAst->get_address());
        }

        BOOST_FOREACH (SgAsmStaticData *dataAst, SageInterface::querySubTree<SgAsmStaticData>(funcAst)) {
            DataBlock::Ptr dblock = DataBlock::instanceBytes(dataAst->get_address(), dataAst->get_size());
            partitioner.attachDataBlock(dblock);
            function->insertDataBlock(dblock);
        }

        partitioner.attachFunction(function);
    }

    return boost::move(partitioner);
}

Partitioner
Engine::createPartitioner() {
    return createTunedPartitioner();
}

void
Engine::runPartitionerInit(Partitioner &partitioner) {
    Sawyer::Message::Stream where(mlog[WHERE]);

    SAWYER_MESG(where) <<"labeling addresses\n";
    labelAddresses(partitioner);

    SAWYER_MESG(where) <<"marking configured basic blocks\n";
    makeConfiguredDataBlocks(partitioner, partitioner.configuration());

    SAWYER_MESG(where) <<"marking configured functions\n";
    makeConfiguredFunctions(partitioner, partitioner.configuration());

    SAWYER_MESG(where) <<"marking ELF/PE container functions\n";
    makeContainerFunctions(partitioner, interp_);

    SAWYER_MESG(where) <<"marking interrupt functions\n";
    makeInterruptVectorFunctions(partitioner, settings_.partitioner.interruptVector);

    SAWYER_MESG(where) <<"marking user-defined functions\n";
    makeUserFunctions(partitioner, settings_.partitioner.startingVas);
}

void
Engine::runPartitionerRecursive(Partitioner &partitioner) {
    Sawyer::Message::Stream where(mlog[WHERE]);

    // Start discovering instructions and forming them into basic blocks and functions
    SAWYER_MESG(where) <<"discovering and populating functions\n";
    discoverFunctions(partitioner);

    // Try to attach basic blocks to functions
    SAWYER_MESG(where) <<"marking function call targets\n";
    if (settings_.partitioner.findingFunctionCallFunctions) {
        SAWYER_MESG(where) <<"finding called functions\n";
        makeCalledFunctions(partitioner);
    }

    SAWYER_MESG(where) <<"discovering basic blocks for marked functions\n";
    attachBlocksToFunctions(partitioner);

    // Additional work
    if (settings_.partitioner.findingDeadCode) {
        SAWYER_MESG(where) <<"attaching dead code to functions\n";
        attachDeadCodeToFunctions(partitioner);
    }
    if (settings_.partitioner.findingFunctionPadding) {
        SAWYER_MESG(where) <<"attaching function padding\n";
        attachPaddingToFunctions(partitioner);
    }
    if (settings_.partitioner.findingIntraFunctionCode > 0) {
        // WHERE message is emitted in the call
        attachAllSurroundedCodeToFunctions(partitioner);
    }
    if (settings_.partitioner.findingIntraFunctionData) {
        SAWYER_MESG(where) <<"searching for inter-function code\n";
        attachSurroundedDataToFunctions(partitioner);
    }

    // Another pass to attach blocks to functions
    SAWYER_MESG(where) <<"discovering basic blocks for marked functions\n";
    attachBlocksToFunctions(partitioner);
}

void
Engine::runPartitionerFinal(Partitioner &partitioner) {
    Sawyer::Message::Stream where(mlog[WHERE]);

    if (settings_.partitioner.splittingThunks) {
        // Splitting thunks off the front of a basic block causes the rest of the basic block to be discarded and then
        // rediscovered. This might also create additional blocks due to the fact that opaque predicate analysis runs only on
        // single blocks at a time -- splitting the block may have broken the opaque predicate.
        SAWYER_MESG(where) <<"splitting thunks from functions\n";
        splitThunkFunctions(partitioner, functionSplittingThunks_);
        discoverBasicBlocks(partitioner);
    }

    // Perform a final pass over all functions.
    SAWYER_MESG(where) <<"discovering basic blocks for marked functions\n";
    attachBlocksToFunctions(partitioner);

    if (interp_) {
        SAWYER_MESG(where) <<"naming imports\n";
        ModulesPe::nameImportThunks(partitioner, interp_);
        ModulesPowerpc::nameImportThunks(partitioner, interp_);
    }
    if (settings_.partitioner.namingConstants) {
        SAWYER_MESG(where) <<"naming constants\n";
        Modules::nameConstants(partitioner);
    }
    if (settings_.partitioner.namingStrings) {
        SAWYER_MESG(where) <<"naming strings\n";
        Modules::nameStrings(partitioner);
    }
    if (settings_.partitioner.namingSyscalls) {
        SAWYER_MESG(where) <<"naming system calls\n";
        ModulesLinux::nameSystemCalls(partitioner, settings_.partitioner.syscallHeader);
    }
    if (settings_.partitioner.demangleNames) {
        SAWYER_MESG(where) <<"demangling names\n";
        Modules::demangleFunctionNames(partitioner);
    }

    if (libcStartMain_)
        libcStartMain_->nameMainFunction(partitioner);
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

    // Make sure solver statistics are accumulated into the class
    if (SmtSolverPtr solver = partitioner.smtSolver())
        solver->resetStatistics();
}

Partitioner
Engine::partition(const std::vector<std::string> &fileNames) {
    try {
        BOOST_FOREACH (const std::string &fileName, fileNames) {
            if (isRbaFile(fileName) && fileNames.size() != 1)
                throw Exception("specifying an RBA file excludes all other inputs");
        }
        if (fileNames.size() == 1 && isRbaFile(fileNames[0])) {
            return loadPartitioner(fileNames[0]);
        } else {
            if (!areSpecimensLoaded())
                loadSpecimens(fileNames);
            obtainDisassembler();
            Partitioner partitioner = createPartitioner();
            runPartitioner(partitioner);
            return boost::move(partitioner);
        }
    } catch (const std::runtime_error &e) {
        if (settings().engine.exitOnError) {
            mlog[FATAL] <<e.what() <<"\n";
            exit(1);
        } else {
            throw;
        }
    }
}

Partitioner
Engine::partition(const std::string &fileName) {
    return partition(std::vector<std::string>(1, fileName));
}

void
Engine::savePartitioner(const Partitioner &partitioner, const boost::filesystem::path &name,
                        SerialIo::Format fmt) {
    Sawyer::Message::Stream info(mlog[INFO]);
    info <<"writing RBA state file";
    Sawyer::Stopwatch timer;
    SerialOutput::Ptr archive = SerialOutput::instance();
    archive->format(fmt);
    archive->open(name);

    archive->savePartitioner(partitioner);

    if (SgProject *project = SageInterface::getProject()) {
        BOOST_FOREACH (SgBinaryComposite *file, SageInterface::querySubTree<SgBinaryComposite>(project))
            archive->saveAst(file);
    }

    info <<"; took " <<timer <<" seconds\n";
}

Partitioner
Engine::loadPartitioner(const boost::filesystem::path &name, SerialIo::Format fmt) {
    Sawyer::Message::Stream info(mlog[INFO]);
    info <<"reading RBA state file";
    Sawyer::Stopwatch timer;
    SerialInput::Ptr archive = SerialInput::instance();
    archive->format(fmt);
    archive->open(name);

    Partitioner partitioner = archive->loadPartitioner();

    interp_ = NULL;
    while (archive->objectType() == SerialIo::AST) {
        SgNode *ast = archive->loadAst();
        if (NULL == interp_) {
            std::vector<SgAsmInterpretation*> interps = SageInterface::querySubTree<SgAsmInterpretation>(ast);
            if (!interps.empty())
                interp_ = interps[0];
        }
    }

    info <<"; took " <<timer << " seconds\n";
    map_ = partitioner.memoryMap();
    return boost::move(partitioner);
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
            Function::Ptr function = Function::instance(entryVa, fconfig.name(), SgAsmFunction::FUNC_CONFIGURED);
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
                ASSERT_require2(function->address() == va, function->printableName());
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
        BOOST_FOREACH (const Function::Ptr &function, ModulesPe::findImportFunctions(partitioner, interp)) {
            rose_addr_t va = function->address();
            insertUnique(retval, partitioner.attachOrMergeFunction(function), sortFunctionsByAddress);
            ASSERT_always_require2(function->address() == va, function->printableName());
        }

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
    Sawyer::Message::Stream where(mlog[WHERE]);

    if (settings_.partitioner.findingEntryFunctions) {
        SAWYER_MESG(where) <<"making entry point functions\n";
        BOOST_FOREACH (const Function::Ptr &function, makeEntryFunctions(partitioner, interp))
            insertUnique(retval, function, sortFunctionsByAddress);
    }
    if (settings_.partitioner.findingErrorFunctions) {
        SAWYER_MESG(where) <<"making error-handling functions\n";
        BOOST_FOREACH (const Function::Ptr &function, makeErrorHandlingFunctions(partitioner, interp))
            insertUnique(retval, function, sortFunctionsByAddress);
    }
    if (settings_.partitioner.findingImportFunctions) {
        SAWYER_MESG(where) <<"making import functions\n";
        BOOST_FOREACH (const Function::Ptr &function, makeImportFunctions(partitioner, interp))
            insertUnique(retval, function, sortFunctionsByAddress);
    }
    if (settings_.partitioner.findingExportFunctions) {
        SAWYER_MESG(where) <<"making export functions\n";
        BOOST_FOREACH (const Function::Ptr &function, makeExportFunctions(partitioner, interp))
            insertUnique(retval, function, sortFunctionsByAddress);
    }
    if (settings_.partitioner.findingSymbolFunctions) {
        SAWYER_MESG(where) <<"making symbol table functions\n";
        BOOST_FOREACH (const Function::Ptr &function, makeSymbolFunctions(partitioner, interp))
            insertUnique(retval, function, sortFunctionsByAddress);
    }
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
        size_t ptrSize = partitioner.instructionProvider().instructionPointerRegister().nBits();
        ASSERT_require2(ptrSize % 8 == 0, "instruction pointer register size is strange");
        size_t bytesPerPointer = ptrSize / 8;
        size_t nPointers = interruptVector.size() / bytesPerPointer;
        ByteOrder::Endianness byteOrder = partitioner.instructionProvider().defaultByteOrder();

        for (size_t i=0; i<nPointers; ++i) {
            rose_addr_t elmtVa = interruptVector.least() + i*bytesPerPointer;
            uint32_t functionVa;
            if (4 == partitioner.memoryMap()->at(elmtVa).limit(4).read((uint8_t*)&functionVa).size()) {
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
        Function::Ptr function = Function::instance(va, SgAsmFunction::FUNC_CMDLINE);
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
    const rose_addr_t wordSize = partitioner.instructionProvider().instructionPointerRegister().nBits() / 8;
    ASSERT_require2(wordSize>0 && wordSize<=8, StringUtility::numberToString(wordSize)+"-byte words not implemented yet");
    const rose_addr_t maxaddr = partitioner.memoryMap()->hull().greatest();

    while (readVa < maxaddr &&
           partitioner.memoryMap()->atOrAfter(readVa)
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
        if (partitioner.memoryMap()->at(readVa).limit(wordSize)
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
        Function::Ptr function = Function::instance(targetVa, SgAsmFunction::FUNC_SCAN_RO_DATA);
        function->reasonComment("at ro-data address " + StringUtility::addrToString(readVa));
        return function;
    }
    readVa = maxaddr;
    return Function::Ptr();
}

Function::Ptr
Engine::makeNextCodeReferencedFunction(const Partitioner &partitioner) {
    // As basic blocks are inserted into the CFG their instructions go into a set to be examined by this function. Once this
    // function examines them, it moves them to an already-examined set.
    rose_addr_t constant = 0;
    while (codeFunctionPointers_ && codeFunctionPointers_->nextConstant(partitioner).assignTo(constant)) {
        rose_addr_t srcVa = codeFunctionPointers_->inProgress();
        SgAsmInstruction *srcInsn = partitioner.instructionProvider()[srcVa];
        ASSERT_not_null(srcInsn);

        SgAsmInstruction *targetInsn = partitioner.discoverInstruction(constant);
        if (!targetInsn || targetInsn->isUnknown())
            continue;                                   // no instruction

        AddressInterval insnInterval = AddressInterval::baseSize(targetInsn->get_address(), targetInsn->get_size());
        if (!partitioner.instructionsOverlapping(insnInterval).empty())
            continue;                                   // would overlap with existing instruction

        // All seems okay, so make a function there
        // FIXME[Robb P Matzke 2017-04-13]: USERDEF is not the best, most descriptive reason, but it's what we have for now
        mlog[INFO] <<"possible code address " <<StringUtility::addrToString(constant) <<"\n";
        Function::Ptr function = Function::instance(constant, SgAsmFunction::FUNC_INSN_RO_DATA);

        function->reasonComment("from " + srcInsn->toString() + ", ro-data address " + StringUtility::addrToString(constant));
        return function;
    }
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

std::vector<Function::Ptr>
Engine::makeFunctionFromInterFunctionCalls(Partitioner &partitioner, rose_addr_t &startVa /*in,out*/) {
    static const rose_addr_t MAX_ADDR(-1);
    static const std::vector<Function::Ptr> NO_FUNCTIONS;
    static const char *me = "makeFunctionFromInterFunctionCalls: ";
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    SAWYER_MESG(debug) <<me <<"(startVa = " <<StringUtility::addrToString(startVa) <<")\n";

    // Avoid creating large basic blocks since this can drastically slow down instruction semantics. Large basic blocks are a
    // real possibility here because we're likely to be interpreting data areas as code. We're not creating any permanent basic
    // blocks in this analysis, so limiting the size here has no effect on which blocks are ultimately added to the control
    // flow graph.  The smaller the limit, the more likely that a multi-instruction call will get split into two blocks and not
    // detected. Multi-instruction calls are an obfuscation technique.
    Sawyer::TemporaryCallback<BasicBlockCallback::Ptr>
        tcb(partitioner.basicBlockCallbacks(), Modules::BasicBlockSizeLimiter::instance(20)); // arbitrary

    while (AddressInterval unusedVas = partitioner.aum().nextUnused(startVa)) {

        // The unused interval must have executable addresses, otherwise skip to the next unused interval.
        AddressInterval unusedExecutableVas = map_->within(unusedVas).require(MemoryMap::EXECUTABLE).available();
        if (unusedExecutableVas.isEmpty()) {
            if (unusedVas.greatest() == MAX_ADDR) {
                startVa = MAX_ADDR;
                return NO_FUNCTIONS;
            } else {
                startVa = unusedVas.greatest() + 1;
                continue;
            }
        }
        startVa = unusedExecutableVas.least();
        SAWYER_MESG(debug) <<me <<"examining interval " <<StringUtility::addrToString(unusedExecutableVas.least())
                           <<".." <<StringUtility::addrToString(unusedExecutableVas.greatest()) <<"\n";

        while (startVa <= unusedExecutableVas.greatest()) {
            // Discover the basic block. It's possible that the partitioner already knows about this block location but just
            // hasn't tried looking for its instructions yet.  I don't think this happens within the stock engine because it
            // tries to recursively discover all basic blocks before it starts scanning things that might be data. But users
            // might call this before they've processed all the outstanding placeholders.  Consider the following hypothetical
            // user's partitioner state
            //          B1: push ebp            ; this block's insns are discovered
            //              mov ebp, esp
            //              test eax, eax
            //              je B3
            //          B2: inc eax             ; this block's insns are discovered
            //          B3: ???                 ; this block is known, but no instructions discovered yet
            //              ???                 ; arbitrary number of blocks
            //          Bn: push ebp            ; this block's insns are discovered
            //              mov ebp, esp
            // To this analysis, the region (B3+1)..Bn is ambiguous. It could be code or data or some of both. Ideally, we
            // should have used the recursive disassembly to process this area already. We'll just pretend it's all unknown and
            // process it with a linear sweep like normal. This is probably fine for normal code since linear and recursive are
            // mostly the same and we'll eventually discover the correct blocks anyway when we finally do the recursive (this
            // analysis doesn't actually create blocks in this region -- it only looks for call sites).
            BasicBlock::Ptr bb;
            if (partitioner.placeholderExists(startVa)) {
#if 0 // DEBUGGING [Robb P Matzke 2016-06-30]
                if (mlog[WARN]) {
                    mlog[WARN] <<me <<"va " <<StringUtility::addrToString(startVa) <<" has ambiguous disposition:\n";
                    static bool emitted = false;
                    if (!emitted) {
                        mlog[WARN] <<"  it is a basic block (with undiscovered instructions),\n"
                                   <<"  and it is absent from the address usage map\n"
                                   <<"  this analysis should be called after all pending instructions are discovered\n";
                        mlog[WARN] <<"  interval " <<StringUtility::addrToString(startVa)
                                   <<".." <<StringUtility::addrToString(unusedExecutableVas.greatest())
                                   <<" treated as unknown\n";
                        mlog[WARN] <<"  the CFG contains "
                                   <<StringUtility::plural(partitioner.undiscoveredVertex()->nInEdges(), "blocks")
                                   <<" lacking instructions\n";
                        emitted = true;
                    }
                }
#endif
            } else {
                bb = partitioner.discoverBasicBlock(startVa);
            }

            // Increment the startVa to be the next unused address. We can't just use the fall-through address of the basic
            // block we just discovered because it might not be contiguous in memory.  Watch out for overflow since it's
            // possible that startVa is already the last address in the address space (in which case we leave startVa as is and
            // return).
            if (bb == NULL || bb->nInstructions() == 0) {
                if (startVa == MAX_ADDR)
                    return NO_FUNCTIONS;
                ++startVa;
                continue;
            }
            if (debug) {
                debug <<me <<bb->printableName() <<"\n";
                BOOST_FOREACH (SgAsmInstruction *insn, bb->instructions())
                    debug <<me <<"  " <<unparseInstructionWithAddress(insn) <<"\n";
            }
            AddressIntervalSet bbVas = bb->insnAddresses();
            if (!bbVas.leastNonExistent(bb->address()).assignTo(startVa)) // address of first hole, or following address
                startVa = MAX_ADDR;                                       // bb ends at max address

            // Basic block sanity checks because we're not sure that we're actually disassembling real code. The basic block
            // should not overlap with anything (basic block, data block, function) already attached to the CFG.
            if (partitioner.aum().anyExists(bbVas)) {
                SAWYER_MESG(debug) <<me <<"candidate basic block overlaps with another; skipping\n";
                continue;
            }

            if (!partitioner.basicBlockIsFunctionCall(bb, Precision::LOW)) {
                SAWYER_MESG(debug) <<me <<"candidate basic block is not a function call; skipping\n";
                continue;
            }

            // Look at the basic block successors to find those which appear to be function calls. Note that the edge types are
            // probably all E_NORMAL at this point rather than E_FUNCTION_CALL, and the call-return edges are not yet present.
            std::set<rose_addr_t> candidateFunctionVas; // entry addresses for potential new functions
            BasicBlock::Successors successors = partitioner.basicBlockSuccessors(bb, Precision::LOW);
            BOOST_FOREACH (const BasicBlock::Successor &succ, successors) {
                if (succ.expr()->is_number() && succ.expr()->get_width() <= 64) {
                    rose_addr_t targetVa = succ.expr()->get_number();
                    if (targetVa == bb->fallthroughVa()) {
                        SAWYER_MESG(debug) <<me <<"successor " <<StringUtility::addrToString(targetVa) <<" is fall-through\n";
                    } else if (partitioner.functionExists(targetVa)) {
                        // We already know about this function, so move on -- nothing to see here.
                        SAWYER_MESG(debug) <<me <<"successor " <<StringUtility::addrToString(targetVa) <<" already exists\n";
                    } else if (partitioner.aum().exists(targetVa)) {
                        // Target is inside some basic block, data block, or function but not a function entry. The basic block
                        // we just discovered is probably bogus, therefore ignore everything about it.
                        candidateFunctionVas.clear();
                        SAWYER_MESG(debug) <<me <<"successor " <<StringUtility::addrToString(targetVa) <<" has a conflict\n";
                        break;
                    } else if (!map_->at(targetVa).require(MemoryMap::EXECUTABLE).exists()) {
                        // Target is in an unmapped area or is not executable. The basic block we just discovered is probably
                        // bogus, therefore ignore everything about it.
                        SAWYER_MESG(debug) <<me <<"successor " <<StringUtility::addrToString(targetVa) <<" not executable\n";
                        candidateFunctionVas.clear();
                        break;
                    } else {
                        // Looks good.
                        candidateFunctionVas.insert(targetVa);
                    }
                }
            }

            // Create new functions containing only the entry blocks. We'll discover the rest of their blocks later by
            // recursively following their control flow.
            if (!candidateFunctionVas.empty()) {
                std::vector<Function::Ptr> newFunctions;
                BOOST_FOREACH (rose_addr_t functionVa, candidateFunctionVas) {
                    Function::Ptr newFunction = Function::instance(functionVa, SgAsmFunction::FUNC_CALL_INSN);
                    newFunction->reasonComment("from " + bb->instructions().back()->toString());
                    newFunctions.push_back(partitioner.attachOrMergeFunction(newFunction));
                    SAWYER_MESG(debug) <<me <<"created " <<newFunction->printableName() <<" from " <<bb->printableName() <<"\n";
                }
                return newFunctions;
            }

            // Avoid overflow or infinite loop
            if (startVa == MAX_ADDR)
                return NO_FUNCTIONS;
        }
    }
    return NO_FUNCTIONS;
}

void
Engine::discoverFunctions(Partitioner &partitioner) {
    rose_addr_t nextPrologueVa = 0;                     // where to search for function prologues
    rose_addr_t nextInterFunctionCallVa = 0;            // where to search for inter-function call instructions
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

        // Scan inter-function code areas to find basic blocks that look reasonable and process them with instruction semantics
        // to find calls to functions that we don't know about yet.
        if (settings_.partitioner.findingInterFunctionCalls) {
            newFunctions = makeFunctionFromInterFunctionCalls(partitioner, nextInterFunctionCallVa /*in,out*/);
            if (!newFunctions.empty())
                continue;
        }

        // Try looking at literal constants inside existing instructions to find possible pointers to new functions
        if (settings_.partitioner.findingCodeFunctionPointers) {
            if (Function::Ptr function = makeNextCodeReferencedFunction(partitioner)) {
                partitioner.attachFunction(function);
                continue;
            }
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
        partitioner.attachDataBlockToFunction(padding, function);
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
    Sawyer::Message::Stream where(mlog[WHERE]);
    size_t retval = 0;
    for (size_t i = 0; i < settings_.partitioner.findingIntraFunctionCode; ++i) {
        SAWYER_MESG(where) <<"searching for intra-function code (pass " <<(i+1) <<")\n";
        size_t n = attachSurroundedCodeToFunctions(partitioner);
        if (0 == n)
            break;
        retval += n;
        discoverBasicBlocks(partitioner);
        if (settings_.partitioner.findingFunctionCallFunctions)
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
        AddressInterval interval = partitioner.memoryMap()->within(unusedAum).require(MemoryMap::EXECUTABLE).available();
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
                if (function->insertBasicBlock(interval.least()))
                    ++nNewBlocks;
                partitioner.attachFunction(function);
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
    BOOST_FOREACH (const MemoryMap::Node &node, partitioner.memoryMap()->nodes()) {
        if ((node.value().accessibility() & MemoryMap::EXECUTABLE) != 0)
            executableSpace.insert(node.key());
    }
    AddressIntervalSet unused = partitioner.aum().unusedExtent(executableSpace);

    // Iterate over the large unused address intervals and find their surrounding functions
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
            DataBlock::Ptr dblock = DataBlock::instanceBytes(interval.least(), interval.size());
            dblock->comment("data encapsulated by function");
            BOOST_FOREACH (const Function::Ptr &function, enclosingFuncs) {
                dblock = partitioner.attachDataBlockToFunction(dblock, function);
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
        CallingConvention::Definition::Ptr dfltCcDef;
        if (!ccDict.empty())
            dfltCcDef = ccDict[0];
        partitioner.allFunctionCallingConventionDefinition(dfltCcDef);
    }

    info <<"; total " <<timer <<" seconds\n";
}

// class method called by ROSE's ::frontend to disassemble instructions.
void
Engine::disassembleForRoseFrontend(SgAsmInterpretation *interp) {
    ASSERT_not_null(interp);
    ASSERT_require(interp->get_global_block() == NULL);

    if (interp->get_map() == NULL) {
        mlog[WARN] <<"no virtual memory to disassemble for";
        BOOST_FOREACH (SgAsmGenericFile *file, interp->get_files())
            mlog[WARN] <<" \"" <<StringUtility::cEscape(file->get_name()) <<"\"";
        mlog[WARN] <<"\n";
        return;
    }

    Engine engine;
    engine.memoryMap(interp->get_map()->shallowCopy()); // copied so we can make local changes
    engine.adjustMemoryMap();
    engine.interpretation(interp);

    if (SgAsmBlock *gblock = engine.buildAst()) {
        interp->set_global_block(gblock);
        interp->set_map(engine.memoryMap());
    }
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

        fixFunctionReturnEdge(args);
        fixFunctionCallEdges(args);
        addPossibleIndeterminateEdge(args);
    }
    return chain;
}

// If the block is a function return (e.g., ends with an x86 RET instruction) to an indeterminate location, then that successor
// type should be E_FUNCTION_RETURN instead of E_NORMAL.
void
Engine::BasicBlockFinalizer::fixFunctionReturnEdge(const Args &args) {
    if (args.partitioner.basicBlockIsFunctionReturn(args.bblock)) {
        bool hadCorrectEdge = false, edgeModified = false;
        BasicBlock::Successors successors = args.partitioner.basicBlockSuccessors(args.bblock);
        for (size_t i = 0; i < successors.size(); ++i) {
            if (!successors[i].expr()->is_number() ||
                (successors[i].expr()->get_expression()->flags() & SymbolicExpr::Node::INDETERMINATE) != 0) {
                if (successors[i].type() == E_FUNCTION_RETURN) {
                    hadCorrectEdge = true;
                    break;
                } else if (successors[i].type() == E_NORMAL && !edgeModified) {
                    successors[i].type(E_FUNCTION_RETURN);
                    edgeModified = true;
                }
            }
        }
        if (!hadCorrectEdge && edgeModified) {
            args.bblock->clearSuccessors();
            args.bblock->successors(successors);
            SAWYER_MESG(mlog[DEBUG]) <<args.bblock->printableName() <<": fixed function return edge type\n";
        }
    }
}

// If the block is a function call (e.g., ends with an x86 CALL instruction) then change all E_NORMAL edges to E_FUNCTION_CALL
// edges.
void
Engine::BasicBlockFinalizer::fixFunctionCallEdges(const Args &args) {
    if (args.partitioner.basicBlockIsFunctionCall(args.bblock)) {
        BasicBlock::Successors successors = args.partitioner.basicBlockSuccessors(args.bblock);
        bool changed = false;
        BOOST_FOREACH (BasicBlock::Successor &successor, successors) {
            if (successor.type() == E_NORMAL) {
                successor.type(E_FUNCTION_CALL);
                changed = true;
            }
        }
        if (changed) {
            args.bblock->clearSuccessors();
            args.bblock->successors(successors);
            SAWYER_MESG(mlog[DEBUG]) <<args.bblock->printableName() <<": fixed function call edge(s) type\n";
        }
    }
}

// Should we add an indeterminate CFG edge from this basic block?  For instance, a "JMP [ADDR]" instruction should get an
// indeterminate edge if ADDR is a writable region of memory. There are two situations: ADDR is non-writable, in which case
// RiscOperators::peekMemory would have returned a free variable to indicate an indeterminate value, or ADDR is writable but
// its MemoryMap::INITIALIZED bit is set to indicate it has a valid value already, in which case RiscOperators::peekMemory
// would have returned the value stored there but also marked the value as being INDETERMINATE.  The
// SymbolicExpr::TreeNode::INDETERMINATE bit in the expression should have been carried along so that things like "MOV EAX,
// [ADDR]; JMP EAX" will behave the same as "JMP [ADDR]".
void
Engine::BasicBlockFinalizer::addPossibleIndeterminateEdge(const Args &args) {
    BasicBlockSemantics sem = args.bblock->semantics();
    if (sem.finalState() == NULL)
        return;
    ASSERT_not_null(sem.operators);

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
        BaseSemantics::SValuePtr addr = sem.operators->undefined_(addrWidth);
        EdgeType type = args.partitioner.basicBlockIsFunctionReturn(args.bblock) ? E_FUNCTION_RETURN : E_NORMAL;
        args.bblock->insertSuccessor(addr, type);
        SAWYER_MESG(mlog[DEBUG]) <<args.bblock->printableName()
                                 <<": added indeterminate successor for initialized, non-constant memory read\n";
    }
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

typedef std::pair<rose_addr_t, size_t> AddressOrder;

static bool
isSecondZero(const AddressOrder &a) {
    return 0 == a.second;
}

static bool
sortBySecond(const AddressOrder &a, const AddressOrder &b) {
    return a.second < b.second;
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

    if (maxSorts_ == 0) {
        BOOST_FOREACH (rose_addr_t va, processedCallReturn_.items())
            finalCallReturn().pushBack(va);
        processedCallReturn_.clear();

    } else {
        if (0 == --maxSorts_)
            mlog[WARN] <<"may-return sort limit reached; reverting to unsorted analysis\n";

        // Get the list of virtual addresses that need to be processed
        std::vector<AddressOrder> pending;
        pending.reserve(finalCallReturn_.size() + processedCallReturn_.size());
        BOOST_FOREACH (rose_addr_t va, finalCallReturn_.items())
            pending.push_back(AddressOrder(va, (size_t)0));
        BOOST_FOREACH (rose_addr_t va, processedCallReturn_.items())
            pending.push_back(AddressOrder(va, (size_t)0));
        finalCallReturn_.clear();
        processedCallReturn_.clear();

        // Find the CFG vertex for each pending address and insert its "order" value. Blocks that are leaves (after arbitrarily
        // breaking cycles) have lower numbers than blocks higher up in the global CFG.
        std::vector<size_t> order = graphDependentOrder(partitioner.cfg());
        BOOST_FOREACH (AddressOrder &pair, pending) {
            ControlFlowGraph::ConstVertexIterator vertex = partitioner.findPlaceholder(pair.first);
            if (vertex != partitioner.cfg().vertices().end() && vertex->value().type() == V_BASIC_BLOCK) {
                pair.second = order[vertex->id()];
            }
        }

        // Sort the pending addresses based on their calculated "order", skipping those that aren't CFG basic blocks, and save
        // the result.
        pending.erase(std::remove_if(pending.begin(), pending.end(), isSecondZero), pending.end());
        std::sort(pending.begin(), pending.end(), sortBySecond);
        BOOST_FOREACH (const AddressOrder &pair, pending)
            finalCallReturn().pushBack(pair.first);
    }
}

// Add new basic block's instructions to list of instruction addresses to process
bool
Engine::CodeConstants::operator()(bool chain, const AttachedBasicBlock &attached) {
    if (chain && attached.bblock) {
        BOOST_FOREACH (SgAsmInstruction *insn, attached.bblock->instructions()) {
            if (wasExamined_.find(insn->get_address()) == wasExamined_.end())
                toBeExamined_.insert(insn->get_address());
        }
    }
    return chain;
}

// Remove basic block's instructions from list of instructions to process
bool
Engine::CodeConstants::operator()(bool chain, const DetachedBasicBlock &detached) {
    if (chain && detached.bblock) {
        BOOST_FOREACH (SgAsmInstruction *insn, detached.bblock->instructions()) {
            toBeExamined_.erase(insn->get_address());
            if (insn->get_address() == inProgress_)
                constants_.clear();
        }
    }
    return chain;
}

// Return the next constant from the next instruction
Sawyer::Optional<rose_addr_t>
Engine::CodeConstants::nextConstant(const Partitioner &partitioner) {
    if (!constants_.empty()) {
        rose_addr_t constant = constants_.back();
        constants_.pop_back();
        return constant;
    }

    while (!toBeExamined_.empty()) {
        inProgress_ = *toBeExamined_.begin();
        toBeExamined_.erase(inProgress_);
        if (SgAsmInstruction *insn = partitioner.instructionExists(inProgress_).insn()) {

            struct T1: AstSimpleProcessing {
                std::set<rose_addr_t> constants;
                virtual void visit(SgNode *node) ROSE_OVERRIDE {
                    if (SgAsmIntegerValueExpression *ival = isSgAsmIntegerValueExpression(node)) {
                        if (ival->get_significantBits() <= 64)
                            constants.insert(ival->get_absoluteValue());
                    }
                }
            } t1;
            t1.traverse(insn, preorder);
            constants_ = std::vector<rose_addr_t>(t1.constants.begin(), t1.constants.end());
        }

        if (!constants_.empty()) {
            rose_addr_t constant = constants_.back();
            constants_.pop_back();
            return constant;
        }
    }

    return Sawyer::Nothing();
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
            size_t nBits = partitioner.instructionProvider().instructionPointerRegister().nBits();
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

        // We've added call-return edges everwhere possible, but may have delayed adding them to blocks where the analysis was
        // indeterminate. If so, sort all those blocks approximately by their height in the global CFG and run may-return
        // analysis on each one
        if (!basicBlockWorkList_->processedCallReturn().isEmpty() || !basicBlockWorkList_->finalCallReturn().isEmpty()) {
            ASSERT_require(basicBlockWorkList_->pendingCallReturn().isEmpty());
            if (!basicBlockWorkList_->processedCallReturn().isEmpty())
                basicBlockWorkList_->moveAndSortCallReturn(partitioner);
            while (!basicBlockWorkList_->finalCallReturn().isEmpty()) {
                basicBlockWorkList_->pendingCallReturn().pushBack(basicBlockWorkList_->finalCallReturn().popFront());
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
    try {
        Partitioner partitioner = partition(fileNames);
        return Modules::buildAst(partitioner, interp_, settings_.astConstruction);
    } catch (const std::runtime_error &e) {
        if (settings().engine.exitOnError) {
            mlog[FATAL] <<e.what() <<"\n";
            exit(1);
        } else {
            throw;
        }
    }
}

SgAsmBlock*
Engine::buildAst(const std::string &fileName) {
    return buildAst(std::vector<std::string>(1, fileName));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Python API support
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef ROSE_ENABLE_PYTHON_API

template<class T>
std::vector<T>
pythonListToVector(boost::python::list &list) {
    std::vector<T> retval;
    for (int i = 0; i < len(list); ++i)
        retval.push_back(boost::python::extract<T>(list[i]));
    return retval;
}

Partitioner
Engine::pythonParseVector(boost::python::list &pyArgs, const std::string &purpose, const std::string &description) {
    reset();
    std::vector<std::string> args = pythonListToVector<std::string>(pyArgs);
    std::vector<std::string> specimenNames = parseCommandLine(args, purpose, description).unreachedArgs();
    return partition(specimenNames);
}

Partitioner
Engine::pythonParseSingle(const std::string &specimen, const std::string &purpose, const std::string &description) {
    return partition(std::vector<std::string>(1, specimen));
}

#endif

} // namespace
} // namespace
} // namespace
