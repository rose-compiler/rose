#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Partitioner2/Engine.h>

// Concrete engine pre-registered as factories
#include <Rose/BinaryAnalysis/Partitioner2/EngineBinary.h>
#include <Rose/BinaryAnalysis/Partitioner2/EngineJvm.h>

#include <Rose/BinaryAnalysis/BinaryLoader.h>
#include <Rose/BinaryAnalysis/Disassembler.h>
#include <Rose/BinaryAnalysis/Partitioner2/Configuration.h>
#include <Rose/BinaryAnalysis/Partitioner2/DataBlock.h>
#include <Rose/BinaryAnalysis/Partitioner2/Modules.h>
#include <Rose/BinaryAnalysis/Partitioner2/ModulesElf.h>
#include <Rose/BinaryAnalysis/Partitioner2/ModulesPe.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/CommandLine.h>

#include <Sawyer/FileSystem.h>

using namespace Rose::Diagnostics;

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

static SAWYER_THREAD_TRAITS::Mutex registryMutex;
static std::vector<Engine::EnginePtr> registry;
static boost::once_flag registryInitFlag = BOOST_ONCE_INIT;

static void
initRegistryHelper() {
    SAWYER_THREAD_TRAITS::LockGuard lock(registryMutex);
    registry.push_back(EngineBinary::factory());
    registry.push_back(EngineJvm::factory());
}

static void
initRegistry() {
    boost::call_once(&initRegistryHelper, registryInitFlag);
}

Engine::Engine(const std::string &name)
    : name_{name}, settings_{Settings()}, interp_{nullptr}, progress_{Progress::instance()} {
}

Engine::Engine(const Settings &settings)
    : name_{}, settings_{settings}, interp_{nullptr}, progress_{Progress::instance()} {
    init();
}

Engine::~Engine() {}

Engine::EnginePtr
Engine::forge() {
  return forge(std::vector<std::string>{});
}

Engine::EnginePtr
Engine::forge(const std::vector<std::string> &specimen) {
    initRegistry();
    SAWYER_THREAD_TRAITS::LockGuard lock(registryMutex);

    for (auto factory = registry.rbegin(); factory != registry.rend(); ++factory) {
        if ((*factory)->matchFactory(specimen)) {
            return (*factory)->instanceFromFactory();
        }
    }
    return {};
}

Engine::EnginePtr
Engine::forge(int argc, char *argv[], Sawyer::CommandLine::Parser& p) {
    initRegistry();
    SAWYER_THREAD_TRAITS::LockGuard lock(registryMutex);

    for (auto f = registry.rbegin(); f != registry.rend(); ++f) {
        Engine::EnginePtr factory = *f;
        Settings settings = factory->settings();

        // Obtain potential specimen from command line arguments using a copy of the parser
        auto parser = p;
        parser.with(factory->engineSwitches());
        parser.with(factory->loaderSwitches());
        parser.with(factory->disassemblerSwitches());
        parser.with(factory->partitionerSwitches());
        std::vector<std::string> specimen = parser.parse(argc, argv).unreachedArgs();

        if (factory->matchFactory(specimen)) {
            auto engine = factory->instanceFromFactory();
            engine->specimen(specimen);

            // good comment could go here
            p.with(engine->engineSwitches());
            p.with(engine->loaderSwitches());
            p.with(engine->disassemblerSwitches());
            p.with(engine->partitionerSwitches());

            return engine;
        }
    }
    return {};
}

bool
Engine::isFactory() const {
    return name_.length() > 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Utility functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
Engine::init() {
    ASSERT_require(map_ == nullptr);
    Rose::initialize(nullptr);
    basicBlockWorkList_ = BasicBlockWorkList::instance(this, settings().partitioner.functionReturnAnalysisMaxSorts);
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
    interpretation(nullptr);
    disassembler_ = Disassembler::Base::Ptr();
    map_ = MemoryMap::Ptr();
    basicBlockWorkList_ = BasicBlockWorkList::instance(this, settings().partitioner.functionReturnAnalysisMaxSorts);
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Command-line parsing
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Sawyer::CommandLine::SwitchGroup
Engine::loaderSwitches() {
    return loaderSwitches(settings().loader);
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
              .doc("This switch is for backward compatibility. It is equivalent to adding \"meta:0:perm+x\" to the end "
                   "of the command line.\n\n"
                   "Adds execute permission to the entire memory map, aside from regions excluded by @s{remove-zeros}. "
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

    sg.insert(Switch("env-erase-name")
              .argument("variable", anyParser(settings.envEraseNames))
              .whichValue(SAVE_ALL)
              .doc("Remove the specified variable from the environment when processing specimens with the \"run:\" schema. This "
                   "switch may appear multiple times to remove multiple variables. The default is to not erase any variables. "
                   "See also, @s{env-erase-pattern}."));

    sg.insert(Switch("env-erase-pattern")
              .argument("regular-expression", anyParser(settings.envErasePatterns))
              .whichValue(SAVE_ALL)
              .doc("Remove variables whose names match the specified regular expression when processing specimens with the "
                   "\"run:\" schema. You must specify \"^\" and/or \"$\" if you want the regular expression anchored to the "
                   "beginning and/or end of names. This switch may appear multiple times to supply multiple regular expressions. "
                   "See also, @s{env-erase-name}."));

    sg.insert(Switch("env-insert")
              .argument("name=value", anyParser(settings.envInsert))
              .whichValue(SAVE_ALL)
              .doc("Add the specified variable and value to the environment when processing specimens with the \"run:\" schema. "
                   "Insertions occur after all environment variable erasures. This switch may appear multiple times to specify "
                   "multiple environment variables."));

    return sg;
}

Sawyer::CommandLine::SwitchGroup
Engine::disassemblerSwitches() {
    return disassemblerSwitches(settings().disassembler);
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

    Rose::CommandLine::insertBooleanSwitch(sg, "disassemble", settings.doDisassemble,
                                           "Perform the disassemble, partition, and post-analysis steps. Otherwise only parse "
                                           "the container (if any) and save the raw, un-disassembled bytes.");

    sg.insert(Switch("isa")
              .argument("architecture", anyParser(settings.isaName))
              .doc("Name of instruction set architecture.  If no name is specified then the architecture is obtained from "
                   "the binary container (ELF, PE). The following ISA names are supported: " +
                   StringUtility::joinEnglish(Disassembler::isaNames()) + "."));

    return sg;
}

Sawyer::CommandLine::SwitchGroup
Engine::partitionerSwitches() {
    return partitionerSwitches(settings().partitioner);
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

    sg.insert(Switch("function-at")
              .argument("addresses", listParser(nonNegativeIntegerParser(settings.functionStartingVas)))
              .whichValue(SAVE_ALL)
              .explosiveLists(true)
              .doc("List of addresses where recursive disassembly should start in addition to addresses discovered by "
                   "other methods. A function entry point will be insterted at each address listed by this switch. "
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

    sg.insert(Switch("ignore-unknown")
              .intrinsicValue(true, settings.base.ignoringUnknownInsns)
              .doc("If set, then any machine instructions that cannot be decoded due to ROSE having an incomplete disassembler "
                   "are treated as if they were no-ops for the purpose of extending a basic block. Although they will still show "
                   "up as \"unknown\" in the assembly listing, they will not cause a basic block to be terminated. If this "
                   "feature is disabled (@s{no-ignore-unknown}) then such instruvctions terminate a basic block. The default "
                   "is that unknown instructions " +
                   std::string(settings.base.ignoringUnknownInsns ? "are ignored." : "terminate basic blocks.")));
    sg.insert(Switch("no-ignore-unknown")
              .key("ignore-unknown")
              .intrinsicValue(false, settings.base.ignoringUnknownInsns)
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
              .doc("A \"ghost edge\" is a control flow graph (CFG) edge that would be present if the CFG-building analysis "
                   "looked only at individual instructions, but would be absent when the analysis looks at coarser units "
                   "of code.  For instance, consider the following x86 assembly code:"

                   "@numbered{mov eax, 0}"              // 1
                   "@numbered{cmp eax, 0}"              // 2
                   "@numbered{jne 5}"                   // 3
                   "@numbered{nop}"                     // 4
                   "@numbered{hlt}"                     // 5

                   "If the analysis looks only at instruction 3, then it appears to have two CFG successors: instructions "
                   "4 and 5. But if the analysis looks at the first three instructions collectively it will ascertain that "
                   "instruction 3 has an opaque predicate, that the only valid CFG successor is instruction 4, and that the "
                   "edge from 3 to 5 is a \"ghost\". In fact, if there are no other incoming edges to these instructions, "
                   "then instructions 1 through 4 will form a basic block with the (unconditional) branch in the interior. "
                   "The ability to look at larger units of code than single instructions is enabled with the @s{use-semantics} "
                   "switch.\n\n"

                   "This @s{follow-ghost-edges} switch causes the ghost edges to be added back into the CFG as real edges, which "
                   "might force a basic block to end. For instance, in this example, turning on @s{follow-ghost-edges} will "
                   "force the first basic block to end with the \"jne\" instruction. The @s{no-follow-ghost-edges} switch turns "
                   "this feature off. By default, this feature is " +
                   std::string(settings.followingGhostEdges?"enabled":"disabled") + "."));
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
              .doc("If ghost edges are being discovered (see @s{follow-ghost-edges} for the definition of \"ghost "
                   "edge\") and are not being inserted into the global control flow graph (controlled by "
                   "@s{follow-ghost-edges}) then the target address of the ghost edge might not be used as a code "
                   "address during the instruction discovery phase. This switch, @s{find-dead-code}, will cause the "
                   "target addresses of ghost edges to be used to discover more instructions even though the ghost "
                   "edges don't appear in the control flow graph. The @s{no-find-dead-code} switch turns this off. "
                   "The default is that this feature is " +
                   std::string(settings.findingDeadCode?"enabled":"disabled") + "."));
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
              .argument("addresses", addressIntervalParser(settings.namingConstants), "all")
              .doc("Scans the instructions and gives labels to constants that refer to entities that have that address "
                   "and also have a name.  For instance, if a constant refers to the beginning of a file section then "
                   "the constant will be labeled so it has the same name as the section. The argument for this switch is the "
                   "range of integer values that can be labeled, defaulting to all addresses. The @s{no-name-constants} switch "
                   "turns this feature off. The default is to " +
                   (settings.namingConstants ?
                    "try to label constants within " + StringUtility::addrToString(settings.namingConstants) + "." :
                    "not perform this labeling.")));
    sg.insert(Switch("no-name-constants")
              .key("name-constants")
              .intrinsicValue(AddressInterval(), settings.namingConstants)
              .hidden(true));

    sg.insert(Switch("name-strings")
              .argument("addresses", addressIntervalParser(settings.namingStrings), "all")
              .doc("Scans the instructions and gives labels to constants that refer to the beginning of string literals. "
                   "The label is usually the first few characters of the string. The argument for this switch is the "
                   "range of integer values that should be tested as string pointers, defaulting to all addresses. The "
                   "@s{no-name-strings} switch turns this feature off. The default is to " +
                   (settings.namingStrings.isEmpty() ? "not perform this labeling." :
                    ("try to label constants within " + StringUtility::addrToString(settings.namingStrings) + "."))));
    sg.insert(Switch("no-name-strings")
              .key("name-strings")
              .intrinsicValue(AddressInterval(), settings.namingStrings)
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
    return engineSwitches(settings().engine);
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
                   "recursively searched for files whose names that end with \".json\" or \".yaml\".  Each file is parsed and "
                   "used to configure the partitioner. This switch may appear more than once and/or a comma-separated list of "
                   "names can be specified.\n\n" + Configuration::fileFormatDoc()));
    return sg;
}

Sawyer::CommandLine::SwitchGroup
Engine::astConstructionSwitches() {
    return astConstructionSwitches(settings().astConstruction);
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
            "those wrong addresses and will be dangling when those addresses are removed from the map.}"

            "@named{aslr}{Turns on address space layout randomization. ASLR is an exploitation counter measure on Linux "
            "systems, but ROSE tools normally disable it in order to have reproducible results during analysis. The \"aslr\" "
            "option causes ROSE to re-enable address space randomization. Although \"noaslr\" is the default, it can also be "
            "explicitly specified.}"

            "}"                                         // end of @bullet

            "@bullet{If the file name begins with the string \"srec:\" then it is treated as Motorola S-Record format. "
            "Mapping attributes are stored after the first column and before the second; the file name appears after the "
            "second colon.  The only mapping attributes supported at this time are permissions, specified as an equal "
            "sign ('=') followed by zero or more of the letters \"r\", \"w\", and \"x\" to signify read, write, and "
            "execute permissions. If no letters are present after the equal sign, then the memory has no permissions; "
            "if the equal sign itself is also missing then the segments are given read, write, and execute permission.}"

            "@bullet{If the name begins with the string \"vxcore:\" then it is treated as a special VxWorks core dump "
            "in a format defined by ROSE. The complete specification has the syntax \"vxcore:[@v{memory_attributes}]"
            ":[@v{file_attributes}]:@v{file_name}\". The parts in square brackets are optional. The only memory attribute "
            "recognized at this time is an equal sign (\"=\") followed by zero of more of the letters \"r\" (read), "
            "\"w\" (write), and \"x\" (execute) to specify the mapping permissions. The default mapping permission if "
            "no equal sign is specified is read, write, and execute.  The only file attribute recognized at this time is "
            "\"version=@v{v}\" where @v{v} is a version number, and ROSE currently supports only version 1.}"

            "@bullet{If the name begins with the string \"meta:\" then it adjusts meta information about the memory "
            "map, such as permissions. " + MemoryMap::adjustMapDocumentation() + "}"

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
    if (!disassembler_ && !settings().disassembler.isaName.empty())
        disassembler_ = Disassembler::lookup(settings().disassembler.isaName);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Binary container parsing
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool
Engine::isRbaFile(const std::string &name) {
    return boost::ends_with(name, ".rba");
}

SgAsmInterpretation*
Engine::parseContainers(const std::string &fileName) {
    return parseContainers(std::vector<std::string>(1, fileName));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Memory map creation (loading)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool
Engine::areSpecimensLoaded() const {
    return map_!=nullptr && !map_->isEmpty();
}

void
Engine::adjustMemoryMap() {
    ASSERT_not_null(map_);
    if (settings().loader.memoryIsExecutable)
        map_->any().changeAccess(MemoryMap::EXECUTABLE, 0);
    Modules::deExecuteZeros(map_/*in,out*/, settings().loader.deExecuteZerosThreshold,
                            settings().loader.deExecuteZerosLeaveAtFront, settings().loader.deExecuteZerosLeaveAtBack);

    switch (settings().loader.memoryDataAdjustment) {
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
Engine::memoryMap() const {
    return map_;
}

void
Engine::memoryMap(const MemoryMap::Ptr &m) {
    map_ = m;
}

MemoryMap::Ptr
Engine::loadSpecimens(const std::string &fileName) {
    return loadSpecimens(std::vector<std::string>(1, fileName));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Disassembler creation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Disassembler::Base::Ptr
Engine::obtainDisassembler() {
    return obtainDisassembler(Disassembler::Base::Ptr());
}

Disassembler::Base::Ptr
Engine::obtainDisassembler(const Disassembler::Base::Ptr &hint) {
    if (!disassembler_ && !settings().disassembler.isaName.empty())
        disassembler_ = Disassembler::lookup(settings().disassembler.isaName);

    if (!disassembler_ && interpretation())
        disassembler_ = Disassembler::lookup(interpretation());

    if (!disassembler_ && hint)
        disassembler_ = hint;

    if (!disassembler_ && settings().disassembler.doDisassemble)
        throw std::runtime_error("no disassembler found and none specified");

    return disassembler_;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Partitioner high-level functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
Engine::checkCreatePartitionerPrerequisites() const {
    if (nullptr == disassembler_ && settings().disassembler.doDisassemble)
        throw std::runtime_error("Engine::createBarePartitioner needs a prior disassembler");
    if (!map_ || map_->isEmpty())
        mlog[WARN] <<"Engine::createBarePartitioner: using an empty memory map\n";
}

Partitioner::Ptr
Engine::createBarePartitioner() {
    Sawyer::Message::Stream info(mlog[MARCH]);

    checkCreatePartitionerPrerequisites();
    auto partitioner = Partitioner::instance(disassembler(), memoryMap());
    if (partitioner->memoryMap() && partitioner->memoryMap()->byteOrder() == ByteOrder::ORDER_UNSPECIFIED && disassembler())
        partitioner->memoryMap()->byteOrder(disassembler()->byteOrder());
    partitioner->settings(settings().partitioner.base);
    partitioner->progress(progress());

    // Load configuration files
    if (!settings().engine.configurationNames.empty()) {
        Sawyer::Stopwatch timer;
        info <<"loading configuration files";
        for (const std::string &configName: settings().engine.configurationNames)
            partitioner->configuration().loadFromFile(configName);
        info <<"; took " <<timer <<"\n";
    }

    // Build the may-return blacklist and/or whitelist.  This could be made specific to the type of interpretation being
    // processed, but there's so few functions that we'll just plop them all into the lists.
//TODO: Not for JVM?
    ModulesPe::buildMayReturnLists(partitioner);
    ModulesElf::buildMayReturnLists(partitioner);

    // Make sure the basicBlockWorkList gets updated when the partitioner's CFG is adjusted.
    ASSERT_not_null(basicBlockWorkList());
    partitioner->cfgAdjustmentCallbacks().prepend(basicBlockWorkList());

    // Make sure the stream of constants found in instruction ASTs is updated whenever the CFG is adjusted.
    if (settings().partitioner.findingCodeFunctionPointers) {
        codeFunctionPointers(CodeConstants::instance());
        partitioner->cfgAdjustmentCallbacks().prepend(codeFunctionPointers());
    }

    // Perform some finalization whenever a basic block is created.  For instance, this figures out whether we should add an
    // extra indeterminate edge for indirect jump instructions that go through initialized but writable memory.
    partitioner->basicBlockCallbacks().append(BasicBlockFinalizer::instance());

    // If the may-return analysis is run and cannot decide whether a function may return, should we assume that it may or
    // cannot return?  The engine decides whether to actually invoke the analysis -- this just sets what to do if it's
    // invoked.
    switch (settings().partitioner.functionReturnAnalysis) {
        case MAYRETURN_ALWAYS_YES:
        case MAYRETURN_DEFAULT_YES:
            partitioner->assumeFunctionsReturn(true);
            break;
        case MAYRETURN_ALWAYS_NO:
        case MAYRETURN_DEFAULT_NO:
            partitioner->assumeFunctionsReturn(false);
    }

    // Should the partitioner favor list-based or map-based containers for semantic memory states?
    partitioner->semanticMemoryParadigm(settings().partitioner.semanticMemoryParadigm);

    // Miscellaneous settings
    if (!settings().partitioner.ipRewrites.empty()) {
        std::vector<Modules::IpRewriter::AddressPair> rewrites;
        for (size_t i = 0; i+1 < settings().partitioner.ipRewrites.size(); i += 2)
            rewrites.push_back(std::make_pair(settings().partitioner.ipRewrites[i+0], settings().partitioner.ipRewrites[i+1]));
        partitioner->basicBlockCallbacks().append(Modules::IpRewriter::instance(rewrites));
    }
    if (settings().partitioner.followingGhostEdges)
        partitioner->basicBlockCallbacks().append(Modules::AddGhostSuccessors::instance());
    if (!settings().partitioner.discontiguousBlocks)
        partitioner->basicBlockCallbacks().append(Modules::PreventDiscontiguousBlocks::instance());
    if (settings().partitioner.maxBasicBlockSize > 0)
        partitioner->basicBlockCallbacks().append(Modules::BasicBlockSizeLimiter::instance(settings().partitioner.maxBasicBlockSize));

    // PEScrambler descrambler
    if (settings().partitioner.peScramblerDispatcherVa) {
        ModulesPe::PeDescrambler::Ptr cb = ModulesPe::PeDescrambler::instance(settings().partitioner.peScramblerDispatcherVa);
        cb->nameKeyAddresses(partitioner);              // give names to certain PEScrambler things
        partitioner->basicBlockCallbacks().append(cb);
        partitioner->attachFunction(Function::instance(settings().partitioner.peScramblerDispatcherVa,
                                                       partitioner->addressName(settings().partitioner.peScramblerDispatcherVa),
                                                       SgAsmFunction::FUNC_PESCRAMBLER_DISPATCH));
    }

    return partitioner;
}

void
Engine::runPartitioner(const Partitioner::Ptr &partitioner) {
    ASSERT_not_null(partitioner);
    Sawyer::Message::Stream info(mlog[INFO]);
    Sawyer::Stopwatch timer;
    info <<"disassembling and partitioning";
    runPartitionerInit(partitioner);
    runPartitionerRecursive(partitioner);
    runPartitionerFinal(partitioner);
    info <<"; took " <<timer <<"\n";

    if (settings().partitioner.doingPostAnalysis)
        updateAnalysisResults(partitioner);

    // Make sure solver statistics are accumulated into the class
    if (SmtSolverPtr solver = partitioner->smtSolver())
        solver->resetStatistics();
}

Partitioner::Ptr
Engine::partition(const std::string &fileName) {
    return partition(std::vector<std::string>(1, fileName));
}

void
Engine::savePartitioner(const Partitioner::ConstPtr &partitioner, const boost::filesystem::path &name,
                        SerialIo::Format fmt) {
    Sawyer::Message::Stream info(mlog[INFO]);
    info <<"writing RBA state file";
    Sawyer::Stopwatch timer;
    SerialOutput::Ptr archive = SerialOutput::instance();
    archive->format(fmt);
    archive->open(name);

    archive->savePartitioner(partitioner);

    if (SgProject *project = SageInterface::getProject()) {
        for (SgBinaryComposite *file: SageInterface::querySubTree<SgBinaryComposite>(project))
            archive->saveAst(file);
    }

    info <<"; took " <<timer <<"\n";
}

Partitioner::Ptr
Engine::loadPartitioner(const boost::filesystem::path &name, SerialIo::Format fmt) {
    Sawyer::Message::Stream info(mlog[INFO]);
    info <<"reading RBA state from " <<name;
    Sawyer::Stopwatch timer;
    SerialInput::Ptr archive = SerialInput::instance();
    archive->format(fmt);
    archive->open(name);

    Partitioner::Ptr partitioner = archive->loadPartitioner();

    interpretation(nullptr);
    while (archive->objectType() == SerialIo::AST) {
        SgNode *ast = archive->loadAst();
        if (nullptr == interpretation()) {
            std::vector<SgAsmInterpretation*> interps = SageInterface::querySubTree<SgAsmInterpretation>(ast);
            if (!interps.empty()) {
                interpretation(interps[0]);
            }
        }
    }

    info <<"; took " <<timer << "\n";
    memoryMap(partitioner->memoryMap());
    return partitioner;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Partitioner mid-level operations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
Engine::labelAddresses(const Partitioner::Ptr &partitioner, const Configuration &configuration) {
    ASSERT_not_null(partitioner);
    Modules::labelSymbolAddresses(partitioner, interpretation());

    for (const AddressConfiguration &c: configuration.addresses().values()) {
        if (!c.name().empty())
            partitioner->addressName(c.address(), c.name());
    }
}

std::vector<DataBlock::Ptr>
Engine::makeConfiguredDataBlocks(const Partitioner::Ptr &partitioner, const Configuration &configuration) {
    // FIXME[Robb P. Matzke 2015-05-12]: This just adds labels to addresses right now.
    ASSERT_not_null(partitioner);
    for (const DataBlockConfiguration &dconfig: configuration.dataBlocks().values()) {
        if (!dconfig.name().empty())
            partitioner->addressName(dconfig.address(), dconfig.name());
    }
    return std::vector<DataBlock::Ptr>();
}

std::vector<Function::Ptr>
Engine::makeConfiguredFunctions(const Partitioner::Ptr &partitioner, const Configuration &configuration) {
    ASSERT_not_null(partitioner);
    std::vector<Function::Ptr> retval;
    for (const FunctionConfiguration &fconfig: configuration.functionConfigurationsByAddress().values()) {
        rose_addr_t entryVa = 0;
        if (fconfig.address().assignTo(entryVa)) {
            Function::Ptr function = Function::instance(entryVa, fconfig.name(), SgAsmFunction::FUNC_CONFIGURED);
            function->comment(fconfig.comment());
            insertUnique(retval, partitioner->attachOrMergeFunction(function), sortFunctionsByAddress);
        }
    }
    return retval;
}

void
Engine::updateAnalysisResults(const Partitioner::Ptr &partitioner) {
    ASSERT_not_null(partitioner);
    Sawyer::Message::Stream info(mlog[INFO]);
    Sawyer::Stopwatch timer;
    info <<"post partition analysis";
    std::string separator = ": ";

    if (settings().partitioner.doingPostFunctionNoop) {
        info <<separator <<"func-no-op";
        separator = ", ";
        Modules::nameNoopFunctions(partitioner);
    }

    if (settings().partitioner.doingPostFunctionMayReturn) {
        info <<separator <<"may-return";
        separator = ", ";
        partitioner->allFunctionMayReturn();
    }

    if (settings().partitioner.doingPostFunctionStackDelta) {
        info <<separator <<"stack-delta";
        separator = ", ";
        partitioner->allFunctionStackDelta();
    }

    if (settings().partitioner.doingPostCallingConvention) {
        info <<separator <<"call-conv";
        separator = ", ";
        // Calling convention analysis uses a default convention to break recursion cycles in the CG.
        const CallingConvention::Dictionary &ccDict = partitioner->instructionProvider().callingConventions();
        CallingConvention::Definition::Ptr dfltCcDef;
        if (!ccDict.empty())
            dfltCcDef = ccDict[0];
        partitioner->allFunctionCallingConventionDefinition(dfltCcDef);
    }

    info <<"; total " <<timer <<"\n";
}

// class method called by ROSE's ::frontend to disassemble instructions.
void
Engine::disassembleForRoseFrontend(SgAsmInterpretation *interp) {
    ASSERT_not_null(interp);
    ASSERT_require(interp->get_global_block() == nullptr);

    if (interp->get_map() == nullptr) {
        mlog[WARN] <<"no virtual memory to disassemble for";
        for (SgAsmGenericFile *file: interp->get_files())
            mlog[WARN] <<" \"" <<StringUtility::cEscape(file->get_name()) <<"\"";
        mlog[WARN] <<"\n";
        return;
    }

    //TODO: Warning, forge() returns EngineBinary as default. Base engine instance on interpretation?
    EnginePtr engine = Engine::forge();

    engine->memoryMap(interp->get_map()->shallowCopy()); // copied so we can make local changes
    engine->adjustMemoryMap();
    engine->interpretation(interp);

    if (SgAsmBlock *gblock = engine->buildAst()) {
        interp->set_global_block(gblock);
        interp->set_map(engine->memoryMap());
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Partitioner low-level stuff
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Build AST
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Settings and Properties
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Disassembler::Base::Ptr
Engine::disassembler() const {
    return disassembler_;
}

void
Engine::disassembler(const Disassembler::Base::Ptr &d) {
    disassembler_ = d;
}

Progress::Ptr
Engine::progress() const {
    return progress_;
}

void
Engine::progress(const Progress::Ptr &progress) {
    progress_ = progress;
}

} // namespace
} // namespace
} // namespace

#endif
