#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

#include <Rose/CommandLine.h>
#include <Rose/BinaryAnalysis/Partitioner2/JvmEngine.h>
#include <Rose/BinaryAnalysis/DisassemblerJvm.h>
#include <Rose/BinaryAnalysis/ByteCode/Jvm.h>

using namespace Rose::Diagnostics;
using std::cout;
using std::endl;

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Utility functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
JvmEngine::init() {
    Rose::initialize(nullptr);
}

void
JvmEngine::reset() {
    disassembler_ = nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Top-level, do everything functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SgAsmBlock*
JvmEngine::frontend(int argc, char *argv[], const std::string &purpose, const std::string &description) {
    std::vector<std::string> args;
    for (int i=1; i<argc; ++i)
        args.push_back(argv[i]);
    return frontend(args, purpose, description);
}

SgAsmBlock*
JvmEngine::frontend(const std::vector<std::string> &args, const std::string &purpose, const std::string &description) {
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
JvmEngine::disassemblerSwitches() {
    return disassemblerSwitches(settings_.disassembler);
}

// class method
Sawyer::CommandLine::SwitchGroup
JvmEngine::disassemblerSwitches(DisassemblerSettings &settings) {
    using namespace Sawyer::CommandLine;
    SwitchGroup sg("Disassembler switches");
    sg.name("disassemble");
    sg.doc("These switches affect the disassembler proper, which is the software responsible for decoding machine "
           "instruction bytes into ROSE internal representations.  The disassembler only decodes instructions at "
           "given addresses and is not responsible for determining what addresses of the virtual address space are decoded.");

    Rose::CommandLine::insertBooleanSwitch(sg, "disassemble", settings.doDisassemble,
                                           "Perform the disassemble, partition, and post-analysis steps. Otherwise only parse "
                                           "the container (if any) and save the raw, un-disassembled bytes.");

    return sg;
}

Sawyer::CommandLine::SwitchGroup
JvmEngine::partitionerSwitches() {
    return partitionerSwitches(settings_.partitioner);
}

// class method
Sawyer::CommandLine::SwitchGroup
JvmEngine::partitionerSwitches(PartitionerSettings &settings) {
    using namespace Sawyer::CommandLine;
    SwitchGroup sg("Partitioner switches");
    sg.name("partition");
    sg.doc("The partitioner is the part of ROSE that drives a disassembler. While the disassembler knows how to decode "
           "a machine instruction to an internal representation, the partitioner knows where to decode. These switches have "
           "no effect if the input is a ROSE Binary Analysis (RBA) file, since the partitioner steps in such an input "
           "have already been completed.");

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
JvmEngine::engineSwitches() {
    return engineSwitches(settings_.engine);
}

// class method
Sawyer::CommandLine::SwitchGroup
JvmEngine::engineSwitches(EngineSettings &settings) {
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
JvmEngine::astConstructionSwitches() {
    return astConstructionSwitches(settings_.astConstruction);
}

// class method
Sawyer::CommandLine::SwitchGroup
JvmEngine::astConstructionSwitches(AstConstructionSettings &settings) {
    using namespace Sawyer::CommandLine;
    SwitchGroup sg("AST construction switches");
    sg.name("ast");
    sg.doc("These switches control how an abstract syntax tree (AST) is generated from partitioner results.");

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
JvmEngine::specimenNameDocumentation() {
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
JvmEngine::commandLineParser(const std::string &purpose, const std::string &description) {
    using namespace Sawyer::CommandLine;
    cout << "JvmEngine::parseCommandLine:2: creating parser\n";
    Parser parser =
        CommandLine::createEmptyParser(purpose.empty() ? std::string("analyze binary specimen") : purpose, description);
    parser.groupNameSeparator("-");                     // ROSE defaults to ":", which is sort of ugly
    parser.doc("Synopsis", "@prop{programName} [@v{switches}] @v{specimen_names}");
    parser.doc("Specimens", specimenNameDocumentation());
    parser.with(engineSwitches());
    parser.with(disassemblerSwitches());
    parser.with(partitionerSwitches());
    parser.with(astConstructionSwitches());

    return parser;
}

Sawyer::CommandLine::ParserResult
JvmEngine::parseCommandLine(int argc, char *argv[], const std::string &purpose, const std::string &description) {
  try {
    std::vector<std::string> args;
    for (int i=1; i<argc; ++i) {
      args.push_back(argv[i]);
    }
    return parseCommandLine(args, purpose, description);
  } catch (const std::runtime_error &e) {
    if (settings().engine.exitOnError) {
      mlog[FATAL] << e.what() << "\n";
      exit(1);
    } else {
      throw;
    }
  }
}

Sawyer::CommandLine::ParserResult
JvmEngine::parseCommandLine(const std::vector<std::string> &args, const std::string &purpose, const std::string &description) {
#if 0 // until I figure out the command line options
  return commandLineParser(purpose, description).parse(args).apply();
#endif

  using namespace ModulesJvm;
  std::string fileName{args[0]};

  if (isJavaClassFile(fileName)) {
#ifdef DEBUG_ON
    // assumes args[0] is the file name for now
    cout << "JvmEngine::parseCommandLine for file " << fileName << endl;
    cout << "  Purpose: " << purpose << ": Description: " << description << "\n";
#endif
  }
  else {
    return Sawyer::CommandLine::ParserResult{};
  }

  auto gf = new SgAsmGenericFile{};
  gf->parse(fileName); /* this loads file into memory, does no reading of file */
  auto header = new SgAsmJvmFileHeader(gf);

  // Check AST
  ROSE_ASSERT(header == gf->get_header(SgAsmGenericFile::FAMILY_JVM));
  ROSE_ASSERT(header->get_parent() == gf);

  header->parse();
#ifdef DEBUG_ON
  cout << "\n --- JVM file header ---\n";
  header->dump(stdout, "    jfh:", 0);

  cout << "\n---------- JVM Analysis -----------------\n\n";
#endif

  Rose::BinaryAnalysis::ByteCode::JvmClass* jvmClass = new ByteCode::JvmClass(header);
#ifdef DEBUG_ON
  cout << "class '" << jvmClass->name() << "'" << endl;
  cout << "----------------\n";
  cout << "   super: " << jvmClass->super_name() << "\n\n";

  cout << "constant pool\n";
  cout << "-----------\n";
  jvmClass->constant_pool()->dump(stdout, "", 1);
  cout << "-----------\n\n";

  if (jvmClass->interfaces().size() > 0) {
    cout << "interfaces\n";
    cout << "-----------\n";
    for (auto interface : jvmClass->interfaces()) {
      cout << "   interface: " << interface->name() << endl;
    }
    cout << "-----------\n\n";
  }

  if (jvmClass->fields().size() > 0) {
    cout << "fields\n";
    cout << "-----------\n";
    for (auto field : jvmClass->fields()) {
      cout << "   field: " << field->name() << endl;
    }
    cout << "-----------\n\n";
  }

  if (jvmClass->attributes().size() > 0) {
    cout << "attributes\n";
    cout << "-----------\n";
    for (auto attribute : jvmClass->attributes()) {
      cout << "   attribute: " << attribute->name() << endl;
    }
    cout << "-----------\n\n";
  }
#endif

  for (auto method : jvmClass->methods()) {
#ifdef DEBUG_ON
    cout << "method '" << method->name() << endl;
    cout << "-----------\n";
#endif

    Disassembler* disassembler = obtainDisassembler();
    ASSERT_not_null(disassembler);

    method->decode(disassembler);
#ifdef DEBUG_ON
    for (auto insn : method->instructions()->get_instructions()) {
      cout << "   : " << insn->get_anyKind() << ": " << insn->get_mnemonic() << ": '"
           << insn->description() << "' " << " size:" << insn->get_size()
           << " va:" << insn->get_address();
      if (insn->terminatesBasicBlock()) cout << " :terminates";
      cout << endl;
    }
    cout << "-----------\n\n";
#endif
  }

  // Bonus
#ifdef DEBUG_ON
  cout << "--- strings ---\n";
  for (auto str : jvmClass->strings()) {
    cout << "   " << str << endl;
  }
  cout << "-----------\n\n";
#endif

  // Run the partitioner
  jvmClass->partition();

  // Dump diagnostics from the partition
#ifdef DEBUG_ON
  for (auto method : jvmClass->methods()) {
    cout << "\nmethod: " << method->name() << endl;
    for (auto block : method->blocks()) {
      cout << "--------------block------------\n";
      for (auto insn : block->instructions()) {
        auto va = insn->get_address();
        cout << "... insn: " << insn << " va:" << va << " :" << insn->get_mnemonic()
             << " nOperands:" << insn->nOperands()
             << " terminates:" << insn->terminatesBasicBlock() << endl;
        for (auto op : insn->get_operandList()->get_operands()) {
          if (op->asUnsigned()) {
            cout << "      unsigned operand:" << *(op->asUnsigned()) << endl;
          }
          else if (op->asSigned()) {
            cout << "       signed operand:" << *(op->asSigned()) << endl;
          }
        }
      }
      // Explore block methods
      cout << "      :nInstructions:" << block->nInstructions() << endl;
      cout << "      :address:" << block->address() << endl;
      cout << "      :fallthroughVa:" << block->fallthroughVa() << endl;
      cout << "      :isEmpty:" << block->isEmpty() << endl;
      cout << "      :nDataBlocks:" << block->nDataBlocks() << endl;
      if (block->isFunctionCall().isCached()) cout << "      :isFunctionCall:" << block->isFunctionCall().get() << endl;
      if (block->isFunctionReturn().isCached()) cout << "      :isFunctionReturn:" << block->isFunctionReturn().get() << endl;
      if (block->successors().isCached()) cout << "      :#successors:" << block->successors().get().size() << endl;
    }
  }
#endif

  // Create graphviz DOT file
  jvmClass->digraph();

  return Sawyer::CommandLine::ParserResult{};
}

void
JvmEngine::checkSettings() {
    if (!disassembler_ && !settings_.disassembler.isaName.empty())
        disassembler_ = Disassembler::lookup(settings_.disassembler.isaName);
}

SgAsmInterpretation*
JvmEngine::parseContainers(const std::string &fileName) {
  Sawyer::Message::Stream where(mlog[WHERE]);
  SAWYER_MESG(where) << "implement parsing containers\n";
  return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Disassembler creation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Disassembler*
JvmEngine::obtainDisassembler(Disassembler *hint) {
  if (disassembler_ == nullptr) {
    disassembler_ = new DisassemblerJvm();
  }
  return disassembler_;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Partitioner high-level functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Partitioner
JvmEngine::createPartitioner() {
  return Partitioner{};
}

void
JvmEngine::runPartitioner(Partitioner &partitioner) {
    Sawyer::Message::Stream info(mlog[INFO]);
    Sawyer::Stopwatch timer;
    info <<"disassembling and partitioning";
    info <<"; took " <<timer <<"\n";
}

Partitioner
JvmEngine::partition(const std::vector<std::string> &fileNames) {
    try {
        obtainDisassembler();
        Partitioner partitioner = createPartitioner();
        runPartitioner(partitioner);
        return boost::move(partitioner);
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
JvmEngine::partition(const std::string &fileName) {
    return partition(std::vector<std::string>(1, fileName));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Partitioner mid-level operations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
JvmEngine::labelAddresses(Partitioner &p, const Configuration &c) {
  Sawyer::Message::Stream where{mlog[WHERE]};
  SAWYER_MESG(where) << "JvmEngine::labelAddresses: no implementation\n";
}

void
JvmEngine::discoverBasicBlocks(Partitioner&) {
  Sawyer::Message::Stream where{mlog[WHERE]};
  SAWYER_MESG(where) << "JvmEngine::discoverBasicBlocks: no implementation\n";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Build AST
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SgAsmBlock*
JvmEngine::buildAst(const std::vector<std::string> &fileNames) {
    try {
        // TODO
        // Partitioner partitioner = partition(fileNames);
        return nullptr;
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
JvmEngine::buildAst(const std::string &fileName) {
    return buildAst(std::vector<std::string>(1, fileName));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      JVM Module
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Disassembly and partitioning utility functions for JVM. */
namespace ModulesJvm {

/** True if named file is a Java class file.
 *
 *  Class files usually have names with a ".class" extension, although this function actually tries to open the file and parse
 *  the file header to make that determination. */
bool isJavaClassFile(const boost::filesystem::path &file) {
  if (!boost::filesystem::exists(file)) {
    return false; // file doesn't exist
  }

  MemoryMap::Ptr map = MemoryMap::instance();
  if (0 == map->insertFile(file.string(), 0)) {
    return false; // file cannot be mmap'd
  }

  uint8_t magic[4];
  if (4 == map->at(0).limit(4).read(magic).size()) {
    if (magic[0] == 0xCA && magic[1] == 0xFE && magic[2] == 0xBA && magic[3] == 0xBE) {
      // 0xCAFEBABE
      return true;
    }
  }
  return false;
}  

} // namespace


} // namespace
} // namespace
} // namespace

#endif
