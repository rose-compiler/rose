#ifndef ROSE_BinaryAnalysis_H
#define ROSE_BinaryAnalysis_H

#include <Rose/BinaryAnalysis/AbstractLocation.h>
#include <Rose/BinaryAnalysis/Address.h>
#include <Rose/BinaryAnalysis/AddressInterval.h>
#include <Rose/BinaryAnalysis/AddressIntervalSet.h>
#include <Rose/BinaryAnalysis/AddressSet.h>
#include <Rose/BinaryAnalysis/Alignment.h>
#include <Rose/BinaryAnalysis/Architecture.h>
#include <Rose/BinaryAnalysis/AsmFunctionIndex.h>
#include <Rose/BinaryAnalysis/AstHasher.h>
#include <Rose/BinaryAnalysis/BasicTypes.h>
#include <Rose/BinaryAnalysis/BestMapAddress.h>
#include <Rose/BinaryAnalysis/BinaryLoaderElf.h>
#include <Rose/BinaryAnalysis/BinaryLoaderElfObj.h>
#include <Rose/BinaryAnalysis/BinaryLoader.h>
#include <Rose/BinaryAnalysis/BinaryLoaderPe.h>
#include <Rose/BinaryAnalysis/ByteCode.h>
#include <Rose/BinaryAnalysis/ByteOrder.h>
#include <Rose/BinaryAnalysis/CallingConvention.h>
#include <Rose/BinaryAnalysis/CodeInserter.h>
#include <Rose/BinaryAnalysis/Concolic.h>
#include <Rose/BinaryAnalysis/ConcreteLocation.h>
#include <Rose/BinaryAnalysis/ControlFlow.h>
#include <Rose/BinaryAnalysis/DataFlow.h>
#include <Rose/BinaryAnalysis/Debugger.h>
#include <Rose/BinaryAnalysis/Demangler.h>
#include <Rose/BinaryAnalysis/Disassembler.h>
#include <Rose/BinaryAnalysis/Dwarf.h>
#include <Rose/BinaryAnalysis/FeasiblePath.h>
#include <Rose/BinaryAnalysis/FunctionCall.h>
#include <Rose/BinaryAnalysis/FunctionSimilarity.h>
#include <Rose/BinaryAnalysis/Hexdump.h>
#include <Rose/BinaryAnalysis/HotPatch.h>
#include <Rose/BinaryAnalysis/InstructionEnumsAarch32.h>
#include <Rose/BinaryAnalysis/InstructionEnumsAarch64.h>
#include <Rose/BinaryAnalysis/InstructionEnumsCil.h>
#include <Rose/BinaryAnalysis/InstructionEnumsJvm.h>
#include <Rose/BinaryAnalysis/InstructionEnumsM68k.h>
#include <Rose/BinaryAnalysis/InstructionEnumsMips.h>
#include <Rose/BinaryAnalysis/InstructionEnumsPowerpc.h>
#include <Rose/BinaryAnalysis/InstructionEnumsX86.h>
#include <Rose/BinaryAnalysis/InstructionMap.h>
#include <Rose/BinaryAnalysis/InstructionProvider.h>
#include <Rose/BinaryAnalysis/InstructionSemantics.h>
#include <Rose/BinaryAnalysis/LibraryIdentification.h>
#include <Rose/BinaryAnalysis/MagicNumber.h>
#include <Rose/BinaryAnalysis/Matrix.h>
#include <Rose/BinaryAnalysis/MemoryMap.h>
#include <Rose/BinaryAnalysis/ModelChecker.h>
#include <Rose/BinaryAnalysis/NoOperation.h>
#include <Rose/BinaryAnalysis/Partitioner2.h>
#include <Rose/BinaryAnalysis/PointerDetection.h>
#include <Rose/BinaryAnalysis/Reachability.h>
#include <Rose/BinaryAnalysis/RegisterDescriptor.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>
#include <Rose/BinaryAnalysis/RegisterNames.h>
#include <Rose/BinaryAnalysis/RegisterParts.h>
#include <Rose/BinaryAnalysis/RelativeVirtualAddress.h>
#include <Rose/BinaryAnalysis/ReturnValueUsed.h>
#include <Rose/BinaryAnalysis/SerialIo.h>
#include <Rose/BinaryAnalysis/SmtCommandLine.h>
#include <Rose/BinaryAnalysis/SmtlibSolver.h>
#include <Rose/BinaryAnalysis/SmtSolver.h>
#include <Rose/BinaryAnalysis/SourceLocations.h>
#include <Rose/BinaryAnalysis/SRecord.h>
#include <Rose/BinaryAnalysis/StackDelta.h>
#include <Rose/BinaryAnalysis/String.h>
#include <Rose/BinaryAnalysis/SymbolicExpression.h>
#include <Rose/BinaryAnalysis/SymbolicExpressionParser.h>
#include <Rose/BinaryAnalysis/SystemCall.h>
#include <Rose/BinaryAnalysis/TaintedFlow.h>
#include <Rose/BinaryAnalysis/ToSource.h>
#include <Rose/BinaryAnalysis/Unparser.h>
#include <Rose/BinaryAnalysis/Utility.h>
#include <Rose/BinaryAnalysis/Variables.h>
#include <Rose/BinaryAnalysis/Z3Solver.h>

namespace Rose {

/** Binary analysis.
 *
 *  This namespace contains ROSE binary analysis features, such as the ability to parse file formats like ELF and PE; to disassemble
 *  machine instructions and byte code and organize them into basic blocks and functions; to analyze code in architecture
 *  independent ways, to generate pseudo-assembly listings, and more.
 *
 *  @section rose_binaryanalysis_background Background
 *
 *  ROSE was originally designed as a source-to-source compiler for analyzing and transforming C source code in an HPC environment.
 *  Later, more source languages like Fortran, C++, C#, Java, Cuda, OpenCL, PHP, Python, Ada, and Jovial were added. At some point
 *  along the line, it was observed that analyzing and transforming binary executables could use much of the same mechanisms as
 *  source code: parsing of container formats like ELF and PE, representing instructions and static data as an abstract syntax tree;
 *  describing the runtime semantics of instructions; analyzing an intermediate representation (IR) of a specimen; transforming the
 *  IR; and unparsing the IR as a new specimen or as assembly language.
 *
 *  Of course there are also many differences between binary code and source code, but ROSE is situated in the unique position of
 *  being able to process both. On the one hand, it can operate on a binary specimen for which no source code is available, and on
 *  the other hand it can perform analysis and transformations where both source and binary are available.
 *
 *  @section rose_binaryanalysis_configuration Configuration
 *
 *  In order to support binary analysis, ROSE needs to be configured for binary analysis during build time. See documentation
 *  concerning ROSE's various build systems for details.
 *
 *  A number of optional supporting libraries can be configured, and the binary analysis capabilities in ROSE will be adjusted based
 *  on what software is available at build time. The binary analysis tools distributed with ROSE have a `--version-long` switch that
 *  will display the features that are enabled. For example:
 *
 *  ```console
 *  $ bat-ana --version-long
 *    ROSE 0.11.145.11 (configured Wed Dec 13 10:00:00 2023 -0800)
 *      --- logic assertions:           disabled
 *      --- boost library:              1.80.0
 *      --- readline library:           unused
 *      --- C/C++ analysis:             disabled
 *      --- Fortran analysis:           disabled
 *      --- binary analysis:            enabled
 *      ---   object serialization:     enabled
 *      ---   ARM AArch32 (A32/T32):    enabled
 *      ---   ARM AArch64 (A64):        enabled
 *      ---   MIPS (be and le):         enabled
 *      ---   Motorola m68k (coldfire): enabled
 *      ---   PowerPC (be and le):      enabled
 *      ---   Intel x86 (i386):         enabled
 *      ---   Intel x86-64 (amd64):     enabled
 *      ---   concolic testing:         enabled
 *      ---   Linux PTRACE debugging    enabled
 *      ---   GDB debugger interface    enabled
 *      ---   fast lib identification   enabled
 *      ---   model checking            enabled
 *      ---   capstone library:         4.0.2
 *      ---   dlib library:             19.19.0
 *      ---   gcrypt library:           1.8.5
 *      ---   magic numbers library:    unused
 *      ---   pqxx library:             unused
 *      ---   sqlite library:           3.31.1
 *      ---   yaml-cpp library:         unused
 *      ---   z3 library:               4.12.2.0
 *      --- Ada analysis:               disabled
 *      --- C# analysis:                disabled
 *      --- CUDA analysis:              disabled
 *      --- Java analysis:              disabled
 *      --- Jovial analysis:            disabled
 *      --- Matlab analysis:            disabled
 *      --- OpenCL analysis:            disabled
 *      --- PHP analysis:               disabled
 *      --- Python analysis:            disabled
 *  ```
 *
 *
 *  @section rose_binaryanalysis_example Example tool
 *
 *  The binary analysis capabilities can be demonstrated by writing a simple example program that will read a specimen and create
 *  a human-readable pseudo-disassembly. This example will also demonstrate:
 *
 *  @li The diagnostic output subsystem
 *  @li Command-line parsing
 *  @li Generation of version-specific documentation
 *  @li The ability to parse a variety of container
 *  @li The ability to decode a variety of instruction sets
 *  @li Configuring an unparser to generate output
 *
 *  @subsection rose_binaryanalysis_example_headers Header files
 *
 *  ROSE header files follow these design principles:
 *
 *  @li The directory path of the header file mirrors the namespace hierarchy of the ROSE library. For instance, the header file for
 *  @ref Rose::BinaryAnalysis::Partitioner2::BasicBlock has the path "Rose/BinaryAnalysis/Partitioner2".
 *
 *  @li The name of the header file matches the name of the entity it primarily declares. For instance, the header file for the @ref
 *  Rose::BinaryAnalysis::Partitioner2::BasicBlock "BasicBlock" class in the @ref Rose::BinaryAnalysis::Partitioner2 namespace is
 *  named "BasicBlock.h".
 *
 *  @li Header files corresponding to a C++ namespace recursively include headers for everything within that namespace. For
 *  instance, including the \"Rose/BinaryAnalysis/Partitioner2.h\" header will recursively include header files for @ref
 *  Rose::BinaryAnalysis::Partitioner2::BasicBlock, @ref Rose::BinaryAnalysis::Partitioner2::Function, @ref
 *  Rose::BinaryAnalysis::Partitioner2::Engine, etc.
 *
 *  @li Header files named "BasicTypes.h" contain declarations most commonly needed as prerequisites for other header files, such
 *  as forward declarations for class names, definitions of shared pointer types, common enums, etc. As such, they can be compiled
 *  quickly.
 *
 *  @li Header files can be included in any order, and any number of times.
 *
 *  @note ROSE has not always followed these principles and therefore has legacy header files that violate them. None of these
 *  headers have paths that start with "Rose", and most of them are included with no path at all (or perhaps just lower-case "rose",
 *  depending on the build system and how ROSE was installed). One of these legacy headers is "rose.h" which must be pre-included by
 *  every program that uses ROSE.
 *
 *  To keep this example simple, we will include the bare minimum of header files. We must first include "rose.h" for legacy
 *  reasons, and then we include a namespace header that will declare everything we need for binary analysis. Including such an
 *  all-encompassing header is not the most efficient if you're building a large tool with many compilation units, but it suffices
 *  for this example; most large tools will want to include only those things they use.
 *
 *  @snippet{trimleft} disassembler.C rose include
 *
 *  @subsection rose_binaryanalysis_example_namespaces Namespaces
 *
 *  It is customary to have C++ directives to help reduce the amount of typing for deeply nested ROSE data structures. This example
 *  will try to strike a balance between minimizing typing and using qualified names to help document the code for newcomers.
 *
 *  Namely, we will bring @ref Rose::BinaryAnalysis into scope for the entire compilation unit as well as @ref
 *  Sawyer::Message::Common. The latter provides (among other things) the diagnostic message severity levels like @c FATAL, @c
 *  ERROR, @c WARN, @c INFO, and others. In addition, we abbreviate "Partitioner2" to just "P2".
 *
 *  @snippet{trimleft} disassembler.C namespaces
 *
 *  @subsection rose_binaryanalysis_example_diagnostics Diagnostics
 *
 *  ROSE has a @ref Rose::Diagnostics "subsystem" for emitting diagnostic messages and progress bars based on @ref Sawyer::Message.
 *  In short, line-oriented text messages are sent to @ref Sawyer::Message::Stream "streams" (subclass of @c std::ostream) of
 *  various severity levels ranging in severity from debug messages to fatal messages.  Streams for the severity levels are
 *  organized into @ref Sawyer::Message::Facility "facilities", one facility per ROSE component. For instance, the various @ref
 *  Rose::BinaryAnalysis::Debugger "debuggers" all share one facility. The facilities are collected into one encompassing
 *  @ref Sawyer::Message::Facilities object from which they can all be controlled.
 *
 *  @note Facilities in ROSE are usually named `mlog` so that at any point in ROSE the name `mlog` refers to the inner,
 *  most-specific facility according to the usual C++ name resolution rules.
 *
 *  Our example tool creates a message facility named `mlog` at file scope.
 *
 *  @snippet{trimleft} disassembler.C mlog decl
 *
 *  A diagnostic message facility needs to be initialized at runtime, which we do near the top of our `main` function. In fact,
 *  the ROSE library itself needs to also be initialized, and we use the @c ROSE_INITIALIZE macro for this which also checks
 *  certain things, like that the ROSE header files that have been included match the version used to compiled the ROSE library
 *  (to adhere to the [C++ ODR](https://en.wikipedia.org/wiki/One_Definition_Rule)).
 *
 *  @snippet{trimleft} disassembler.C init
 *
 *  Individual streams can be turned on or off in source code with their @ref Sawyer::Message::Stream::enable "enable"
 *  functions. The ROSE binary analysis tools also have a "--log" switch to query and control the various streams. To get
 *  started, use "--log=help". For example, here's our example tool listing all known diagnostic facilities and what streams
 *  within each facility are enabled (our own tool's facility is listed last):
 *
 *  ```console
 *  $ ./example --log=list
 *  Logging facilities status
 *    Letters indicate a stream that is enabled; hyphens indicate disabled.
 *    D=debug, T=trace, H=where, M=march, I=info, W=warning, E=error, F=fatal
 *  ---MIWEF default enabled levels
 *  ---MIWEF Rose                                                    -- top-level ROSE diagnostics
 *  ---MIWEF Rose::BinaryAnalysis::Architecture                      -- defining hardware architectures
 *  ---MIWEF Rose::BinaryAnalysis::AsmUnparser                       -- generating assembly listings (vers 1)
 *  ---MIWEF Rose::BinaryAnalysis::Ast                               -- operating on binary abstract syntax trees
 *  ---MIWEF Rose::BinaryAnalysis::BestMapAddress                    -- computing mem mapping based on insns
 *  ---MIWEF Rose::BinaryAnalysis::BinaryLoader                      -- mapping files into virtual memory
 *  ---MIWEF Rose::BinaryAnalysis::BinaryToSource                    -- lifting assembly to C
 *  ---MIWEF Rose::BinaryAnalysis::CallingConvention                 -- computing function calling conventions
 *  ---MIWEF Rose::BinaryAnalysis::CodeInserter                      -- inserting code into an existing specimen
 *  ---MIWEF Rose::BinaryAnalysis::Concolic                          -- concolic testing
 *  ---MIWEF Rose::BinaryAnalysis::DataFlow                          -- solving data-flow analysis problems
 *  ---MIWEF Rose::BinaryAnalysis::Debugger                          -- debugging other processes
 *  ---MIWEF Rose::BinaryAnalysis::Disassembler                      -- decoding machine language instructions
 *  ---MIWEF Rose::BinaryAnalysis::Dwarf                             -- parsing DWARF debug information
 *  ---MIWEF Rose::BinaryAnalysis::FeasiblePath                      -- model checking and path feasibility
 *  ---MIWEF Rose::BinaryAnalysis::FunctionSimilarity                -- matching function pairs based on similarity
 *  ---MIWEF Rose::BinaryAnalysis::HotPatch                          -- semantic hot patching
 *  ---MIWEF Rose::BinaryAnalysis::InstructionSemantics              -- evaluating instructions based on their behaviors
 *  ---MIWEF Rose::BinaryAnalysis::LibraryIdentification             -- fast library identification and recognition (FLIR)
 *  ---MIWEF Rose::BinaryAnalysis::ModelChecker                      -- model checking
 *  ---MIWEF Rose::BinaryAnalysis::NoOperation                       -- determining insn sequences having no effect
 *  ---MIWEF Rose::BinaryAnalysis::Partitioner2                      -- partitioning insns to basic blocks and functions
 *  ---MIWEF Rose::BinaryAnalysis::PointerDetection                  -- finding pointers to code and data
 *  ---MIWEF Rose::BinaryAnalysis::Reachability                      -- propagating reachability through a CFG
 *  ---MIWEF Rose::BinaryAnalysis::ReturnValueUsed                   -- determining whether a return value is used
 *  ---MIWEF Rose::BinaryAnalysis::SerialIo                          -- reading/writing serialized analysis states
 *  ---MIWEF Rose::BinaryAnalysis::SmtSolver                         -- invoking a satisfiability modulo theory solver
 *  ---MIWEF Rose::BinaryAnalysis::StackDelta                        -- analyzing stack pointer behavior
 *  ---MIWEF Rose::BinaryAnalysis::Strings                           -- detecting string constants
 *  ---MIWEF Rose::BinaryAnalysis::SymbolicExpressionParser          -- parsing symbolic expressions
 *  ---MIWEF Rose::BinaryAnalysis::TaintedFlow                       -- analyzing based on tainted-flow
 *  ---MIWEF Rose::BinaryAnalysis::Unparser                          -- generating assembly listings (vers 2)
 *  ---MIWEF Rose::BinaryAnalysis::Variables                         -- local and global variable detection
 *  ---MIWEF Rose::BinaryAnalysis::VxcoreParser                      -- parsing and unparsing vxcore format
 *  ---MIWEF Rose::EditDistance                                      -- measuring differences using edit distance
 *  ---MIWEF Rose::FixupAstDeclarationScope                          -- normalizing AST declarations
 *  ---MIWEF Rose::FixupAstSymbolTablesToSupportAliasedSymbols       -- normalizing symbol tables for aliased symbols
 *  ---MIWEF Rose::SageBuilder                                       -- building abstract syntax trees
 *  ---MIWEF Rose::TestChildPointersInMemoryPool                     -- testing AST child pointers in memory pools
 *  ---MIWEF Rose::UnparseLanguageIndependentConstructs              -- generating source code for language-indepentend constructs
 *  ---MIWEF rose_ir_node                                            -- operating on ROSE internal representation nodes
 *  -----WEF sawyer                                                  -- Sawyer C++ support library
 *  ---MIWEF tool                                                    -- binary analysis tutorial
 *  ```
 *
 *  If we wanted to enable all diagnostic output from our tool, and only errors and fatal diagnostics from the rest of ROSE, we
 *  could invoke our tool like this, which first disables all streams, then enables those for errors and greater, and finally
 *  enables all streams for our tool facility. By appending "--log=list" we can see the affect this has:
 *
 *  ```console
 *  $ ./example --log='none,>=error,tool(all)' --log=list
 *  Logging facilities status
 *    Letters indicate a stream that is enabled; hyphens indicate disabled.
 *    D=debug, T=trace, H=where, M=march, I=info, W=warning, E=error, F=fatal
 *  ------EF default enabled levels
 *  ------EF Rose                                                    -- top-level ROSE diagnostics
 *  ------EF Rose::BinaryAnalysis::Architecture                      -- defining hardware architectures
 *  ------EF Rose::BinaryAnalysis::AsmUnparser                       -- generating assembly listings (vers 1)
 *  ------EF Rose::BinaryAnalysis::Ast                               -- operating on binary abstract syntax trees
 *  ------EF Rose::BinaryAnalysis::BestMapAddress                    -- computing mem mapping based on insns
 *  ------EF Rose::BinaryAnalysis::BinaryLoader                      -- mapping files into virtual memory
 *  ------EF Rose::BinaryAnalysis::BinaryToSource                    -- lifting assembly to C
 *  ------EF Rose::BinaryAnalysis::CallingConvention                 -- computing function calling conventions
 *  ------EF Rose::BinaryAnalysis::CodeInserter                      -- inserting code into an existing specimen
 *  ------EF Rose::BinaryAnalysis::Concolic                          -- concolic testing
 *  ------EF Rose::BinaryAnalysis::DataFlow                          -- solving data-flow analysis problems
 *  ------EF Rose::BinaryAnalysis::Debugger                          -- debugging other processes
 *  ------EF Rose::BinaryAnalysis::Disassembler                      -- decoding machine language instructions
 *  ------EF Rose::BinaryAnalysis::Dwarf                             -- parsing DWARF debug information
 *  ------EF Rose::BinaryAnalysis::FeasiblePath                      -- model checking and path feasibility
 *  ------EF Rose::BinaryAnalysis::FunctionSimilarity                -- matching function pairs based on similarity
 *  ------EF Rose::BinaryAnalysis::HotPatch                          -- semantic hot patching
 *  ------EF Rose::BinaryAnalysis::InstructionSemantics              -- evaluating instructions based on their behaviors
 *  ------EF Rose::BinaryAnalysis::LibraryIdentification             -- fast library identification and recognition (FLIR)
 *  ------EF Rose::BinaryAnalysis::ModelChecker                      -- model checking
 *  ------EF Rose::BinaryAnalysis::NoOperation                       -- determining insn sequences having no effect
 *  ------EF Rose::BinaryAnalysis::Partitioner2                      -- partitioning insns to basic blocks and functions
 *  ------EF Rose::BinaryAnalysis::PointerDetection                  -- finding pointers to code and data
 *  ------EF Rose::BinaryAnalysis::Reachability                      -- propagating reachability through a CFG
 *  ------EF Rose::BinaryAnalysis::ReturnValueUsed                   -- determining whether a return value is used
 *  ------EF Rose::BinaryAnalysis::SerialIo                          -- reading/writing serialized analysis states
 *  ------EF Rose::BinaryAnalysis::SmtSolver                         -- invoking a satisfiability modulo theory solver
 *  ------EF Rose::BinaryAnalysis::StackDelta                        -- analyzing stack pointer behavior
 *  ------EF Rose::BinaryAnalysis::Strings                           -- detecting string constants
 *  ------EF Rose::BinaryAnalysis::SymbolicExpressionParser          -- parsing symbolic expressions
 *  ------EF Rose::BinaryAnalysis::TaintedFlow                       -- analyzing based on tainted-flow
 *  ------EF Rose::BinaryAnalysis::Unparser                          -- generating assembly listings (vers 2)
 *  ------EF Rose::BinaryAnalysis::Variables                         -- local and global variable detection
 *  ------EF Rose::BinaryAnalysis::VxcoreParser                      -- parsing and unparsing vxcore format
 *  ------EF Rose::EditDistance                                      -- measuring differences using edit distance
 *  ------EF Rose::FixupAstDeclarationScope                          -- normalizing AST declarations
 *  ------EF Rose::FixupAstSymbolTablesToSupportAliasedSymbols       -- normalizing symbol tables for aliased symbols
 *  ------EF Rose::SageBuilder                                       -- building abstract syntax trees
 *  ------EF Rose::TestChildPointersInMemoryPool                     -- testing AST child pointers in memory pools
 *  ------EF Rose::UnparseLanguageIndependentConstructs              -- generating source code for language-indepentend constructs
 *  ------EF rose_ir_node                                            -- operating on ROSE internal representation nodes
 *  ------EF sawyer                                                  -- Sawyer C++ support library
 *  DTHMIWEF tool                                                    -- binary analysis tutorial
 *  ```
 *  @subsection rose_binaryanalysis_example_cmdline Command-line parsing
 *
 *  ROSE binary analysis tools use the @ref Sawyer::CommandLine API for parsing switches and their arguments from the command-line
 *  in order to give a consistent look and feel across all tools. Parsing the positional arguments following the switches is up to
 *  each individual tool, although many tools can simply feed these to any of the ROSE functions responsible for loading a binary
 *  specimen for analysis. Because the switch grammar is known by ROSE but the positional argument grammar is not, it is impossible
 *  for ROSE to interleave these two grammars in a sound way. Therefore all command-line arguments to be treated as switches by ROSE
 *  parser must occur before any other command-line arguments to be parsed by the tool.
 *
 *  A command-line switch parser is built from individual @ref Sawyer::CommandLine::Switch "switch definitions", the switch
 *  definitions are combined to form @ref Sawyer::CommandLine::SwitchGroup "switch groups", and the switch groups are combined
 *  to form a @ref Sawyer::CommandLine::Parser "switch parser". All of these parts are copyable; for instance, a switch definition
 *  is copied when inserted into a switch group.
 *
 *  A switch parser is also responsible for creating the Unix "man" page documentation that describes the command-line switches and
 *  the workings of the tool. Documentation is written in a very simple language consisting of paragraphs separated by blank lines.
 *  A variety of paragraph types are possible (plain, bulleted lists, named lists, quoted text, etc.) and spans of characters within
 *  a paragraph can have styles (plain, variable, emphasized, code, etc). Due to limitations in the ways that man pages are
 *  generated, certain restrictions apply, such as the inability to nest lists.
 *
 *  The example tool creates a new switch group whose switch names can be optionally prefixed with "tool" if they need to be
 *  disambiguated from other ROSE switches (as in "--tool:output"). A "--output" or "-o" switch is added to this group and takes
 *  a single argument that can be any string. After a successful parse of the command-line switches, the argument will be saved
 *  in the @c settings.outputFileName variable. The documentation string demonstrates a single paragraph that makes reference to
 *  the (arbitrary) name of the switch argument.
 *
 *  @snippet{trimleft} disassembler.C switch defn
 *
 *  The man page for this switch, generated from `./example --help` looks like this,
 *  automatically reflowed to the width of the terminal.
 *
 *  ```text
 *  Tool-specific switches
 *      --[tool:]output filename; -o filename
 *          Write the assembly listing to the specified file. If the filename
 *          is "-" then output is send to the standard output stream.
 *  ```
 *
 *  @note Due to limitations of the ROSE documentation system, what you don't see here is that the word "filename"  (all three
 *  occurrences) is underlined to indicate it's a variable, and that the heading is in a bold font face.
 *
 *  Once the tool switch groups (in this case just one) are defined, we can create a parser and add that group to it. ROSE's
 *  command-line parsing is very modular, allowing us to also add a group of standard, generic switches before the tool
 *  switches. We also place a "Synopsis" section in the man page, and tell the parser that if it encounters an error it should
 *  emit an error message using our @c FATAL diagnostic stream.
 *
 *  @snippet{trimleft} disassembler.C switch parser
 *
 *  The ROSE binary analysis tools have a policy of defining the single-line purpose and multi-line description at the very top of
 *  the source file where it's easily discovered.
 *
 *  @snippet{trimleft} disassembler.C description
 *
 *  The following code will show how the command-line parser is used.
 *
 *  @subsection rose_binaryanalysis_example_partitioning Partitioning
 *
 *  ROSE calls the process of distinguishing between instructions and static data, and organizing the instructions into basic blocks
 *  and functions "partitioning". Since some forms of binary specimens can arbitrarily mix code and data and don't have enough
 *  information to distinguish between them, the general partitioning problem is undecidable. ROSE uses certain heuristics to
 *  guess the partitioning, and in most cases these heuristics produce good results.
 *
 *  Partitioning is performed in ROSE with the @ref Rose::BinaryAnalysis::Partitioner2 API, which primarily consists of a number of
 *  different @ref Rose::BinaryAnalysis::Partitioner2::Engine "engines" that interface with and store results in a @ref
 *  Rose::BinaryAnalysis::Partitioner2::Partitioner "partitioner". The partitioner is subsequently used by many other parts of ROSE
 *  during analysis and unparsing.
 *
 *  Different kinds of binary specimens require different kinds of partitioning approaches. For instance, byte code is often packed
 *  in a container from which the code and data can be easily and accurately discovered, while general machine code requires a much
 *  more involved heuristic approach for an ultimately undecidable problem. These approaches are encoded in partitioning engines
 *  like @ref Rose::BinaryAnalysis::Partitioner2::EngineBinary "EngineBinary" and @ref Rose::BinaryAnalysis::Partitioner2::EngineJvm
 *  "EngineJvm", each of which take different command-line switches. A tool must surmount the
 *  [chicken-versus-egg](https://en.wikipedia.org/wiki/Chicken_or_the_egg) problem of being able to parse the command-line switches,
 *  producing documentation, and configuring an engine before knowing what engine is ultimately required. The specifics are
 *  documented in the @ref Rose::BinaryAnalysis::Partitioner2::Engine "engine" base class, but in general the steps are:
 *
 *  @li Create a command-line switch parser without engine-specific switches (presented above).
 *  @li Produce a man page that describes all available engines.
 *  @li Attempt to parse switches with each engine in order to get the the positional arguments.
 *  @li Parse the positional arguments in a tool-specific manner to get the specimen description arguments.
 *  @li Test whether the engine can handle the specimen description arguments.
 *  @li If so, adjust the user-supplied parser by adding that engine's command-lines switches.
 *  @li Return the suitable engine for the tool to use later.
 *
 *  These steps are all performed with two function calls from the tool. One to find the correct engine and another to process the
 *  command-line using the adjusted parser. Command-line parsing has two parts: the actual parsing and verification of the
 *  command-line syntax, and then applying the parse results to update the C++ switch argument destination variables that were noted
 *  when the parser was built, namely our `Settings settings;` variable in `main` and the settings in the @ref
 *  Rose::BinaryAnalysis::Partitioner2::Engine "engine" that was returned. For this reason, the locations bound to the parser must
 *  outlive the call to @ref Sawyer::CommandLine::ParserResult::apply "apply".
 *
 *  @snippet{trimleft} disassembler.C engine config
 *
 *  After parsing the command-line switches, the tool obtains the non-switch, positional arguments by calling @ref
 *  Sawyer::CommandLine::ParserResult::unreachedArgs "unreachedArgs" and we check them to see that a specimen was actually given on
 *  the command-line. The tool also emits some information about what engine is being used.
 *
 *  @snippet{trimleft} disassembler.C arg check
 *
 *  Finally, the tool runs the partitioning engine by calling its @ref Rose::BinaryAnalysis::Partitioner2::Engine::partition
 *  "partition" method with the specimen description positional arguments from the command-line. The tool's man page ("--help")
 *  describes at length the different ways to describe a binary specimen, and ROSE supports parsing containers like ELF, PE and java
 *  class files and initializing virtual analysis memory from their segment/section descriptions and/or initializing memory in other
 *  ways: from raw memory dumps, from Motorola S-Records, from Intel HEX files, from running Linux processes, by creating a Linux
 *  process and interrupting it, from ELF core dumps, from previously saved ROSE Binary Analysis (RBA) files, from data bytes
 *  specified on the command-line, or from custom-designed formats. Additionally, parts of the address space can be subsequently
 *  adjusted such as setting or clearing access permissions, or erasing parts of the space. Ultimately, what gets created at this
 *  step before the core partitioning algorithms run is a virtual address space represented by a @ref
 *  Rose::BinaryAnalysis::MemoryMap object.
 *
 *  The partitioning process can be controlled at a finer granularity, but the most common approach is to call @ref
 *  Rose::BinaryAnalysis::Partitioner2::Engine::partition and do everything at once. The result is a @ref
 *  Rose::BinaryAnalysis::Partitioner2::Partitioner "Partitioner" object that holds information that later analyses can obtain
 *  efficiently, such as the virtual memory map and the processor architecture.
 *
 *  @snippet{trimleft} disassembler.C partition
 *
 *  @subsection rose_binaryanalysis_example_pointers Shared-ownership pointers
 *
 *  This is a good time to mention the various @c Ptr types already encountered in this example. ROSE binary analysis objects are
 *  mostly allocated on the heap and accessed through reference counting pointers. These objects are not created with the usual
 *  C++ constructors (which have been made protected to prevent accidental misuse), but rather with static member functions usually
 *  named @c instance. Therefore, calls to the @c new and @c delete operators are almost never encountered in ROSE binary analysis
 *  tools and memory errors are less likely. The type @c ConstPtr is a shared-ownership pointer to a @c const object.
 *
 *  The reason that the pointer types have these names is twofold. It's shorter to write `Foo::Ptr` than to write
 *  `std::shared_ptr<Foo>`, but mainly because ROSE uses three kinds of shared-ownership pointers:
 *
 *  @li `std::shared_ptr<T>` is used for the majority of most recently-written parts of ROSE.
 *  @li `boost::shared_ptr<T>` is used for most legacy parts of ROSE before C++11 was available.
 *  @li @ref Sawyer::SharedPointer is a type of intrusive pointer used where performance is critical.
 *
 *  All of these pointers support the usual pointer-like operations of dereferencing with `operator*` and `operator->`, assignment
 *  from compatible pointers with `operator=`, and testing whether the pointer is null with `explicit operator bool`.
 *
 *  When only forward declarations are present (when you only include "BasicTypes.h" header files), the pointer types are juxtaposed
 *  with the class name, as in `FooPtr` and `FooConstPtr` versus `Foo::Ptr` and `Foo::ConstPtr`. The binary analysis convention is
 *  that the juxtaposed names are used in header files (and thus most documentation) and the separate names are used everywhere
 *  else. This convention enables header files to have fewer dependencies.
 *
 *  @subsection rose_binaryanalysis_example_types Other special types
 *
 *  One thing to notice with binary analysis is that the C++ signed integral types are almost never used; only unsigned types are
 *  used. This is because most modern arithmetic machine instructions operate on unsigned values, and relatively few arithmetic
 *  instructions need to make a distinction between signed and unsigned operation. Because of this, subtraction in a two's
 *  complement paradigm is usually accomplished by overflow of unsigned addition.
 *
 *  Another reason for using almost exclusively unsigned types is that binary specimens routinely have values near the high end of
 *  the unsigned range, and even values like 32-bit `0xffffffff` is common. This tidbit is one reason ROSE has its own @ref
 *  Sawyer::Container::Interval "interval" types that store the interval's maximum value instead of the more usual C++
 *  one-after-the-end value, and why binary analysis tends to use intervals instead of size (e.g., the size of (number of values
 *  in) an interval that spans the entire domain is one larger than can be represented in that domain).
 *
 *  One important thing to be aware of for C++ signed integral values is that the compiler can assume that signed overflow is not
 *  possible, and if it actually occurs the behavior is [undefined](https://en.cppreference.com/w/cpp/language/ub). The C++ compiler
 *  is not required to diagnose the undefined behavior nor is the compiled program required to do anything meaningful.
 *
 *  @subsection rose_binaryanaysis_example_unparse Unparsing
 *
 *  Unparsing means two things in ROSE binary analysis: recreating the specimen from ROSE's (possibly modified) intermediate
 *  representation (IR), and producing an assembly listing. Although it is possible to modify the IR to produce a new specimen,
 *  doing so is generally unsound because not only is the partitioning undecidable, but also because a binary generally doesn't
 *  have enough information to adjust all cross-references to moved code and data (or for that matter, to even know about all
 *  the cross references).
 *
 *  Therefore, unparsing in the context of binary analysis usually means unparsing the IR to an assembly code representation. ROSE,
 *  being a library to aid human understanding of binaries and because of the undecidability of partitioning in the first place, has
 *  it's own assembly format that's not necessarily intended to be reassembled into a binary. To that end, ROSE's syntax is more
 *  uniform across instruction sets, and often more verbose than the typical assembler for that architecture. ROSE does, however,
 *  generally preserve native instruction mnemonics and operand order.
 *
 *  ROSE's unparser is a hierarchy of classes fully extensible by the user in their own tools at compile time. These unparsers are
 *  highly configurable yet creating a specialized unparser is very simple. An unparser can:
 *
 *  @li unparse a single instruction, a basic block, a single function, or an entire specimen
 *  @li produce colorized output with ANSI escape codes
 *  @li show reasons why ROSE thinks code exists at certain addresses
 *  @li show static data as bytes and/or decoded machine instructions not part of control flow
 *  @li show a representation of the function call graph in the function headers
 *  @li show control flow information as comments and as margin arrows
 *  @li show descriptions for each instruction mnemonic to aid understanding of uncommon instruction sets
 *  @li show signed and unsigned decimal values
 *  @li comment on various parts of instruction operand expressions
 *  @li show raw addresses or only generated basic block labels
 *  @li show machine instruction bytes
 *  @li show how instructions are organized into basic blocks
 *  @li show stack pointer offset from the beginning of the function
 *  @li show detected calling conventions and argument usage
 *  @li show user-defined information by extending the unparser through class derivation
 *
 *  An easy way to unparse a single instruction without leveraging all the power of a full unparser, is to call the instruction's
 *  @ref SgAsmInstruction::toString "toString" method.  But this example, because it wants to unparse an entire specimen, obtains
 *  a suitable unparser for the specimen's architecture and then calls its @ref Rose::BinaryAnalysis::Unparser::Base::unparse
 *  "unparse" method. The `outputFile` function is defined in the example tool to return a `std::ostream` reference based on the
 *  argument to the "--output" command-line switch (if any).
 *
 *  @snippet{trimleft} disassembler.C unparse
 *
 *  @note ROSE also has a legacy unparser framework which is being phased out. It's defined in "src/backend/asmUnparser" in the
 *  class @c Rose::BinaryAnalysis::AsmUnaprser (intentionally not cross referenced here since you shouldn't use it).
 *
 *  @subsection rose_binaryanalysis_example_arch Architecture abstraction
 *
 *  You may have noticed the call(s) to obtain an architecture. ROSE abstracts most architecture-specific information and algorithms
 *  into classes derived from @ref Rose::BinaryAnalysis::Architecture::Base. A number of these architectures are built into the ROSE
 *  library, but users can also define their own architectures and register them using the functions in the @ref
 *  Rose::BinaryAnalysis::Architecture namespace. In fact, the user's architecture definition can be compiled into its own library
 *  and made available to standard ROSE tools at runtime with their "--architectures" switch.
 *
 *  @subsection rose_binaryanalysis_example_listing Full listing
 *
 *  Here is the full listing of the example program with cross references.
 *
 *  @includelineno disassembler.C */
namespace BinaryAnalysis {}
} // namespace

#endif
