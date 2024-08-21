#ifndef ROSE_BinaryAnalysis_Architecture_H
#define ROSE_BinaryAnalysis_Architecture_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/Architecture/Amd64.h>
#include <Rose/BinaryAnalysis/Architecture/ArmAarch32.h>
#include <Rose/BinaryAnalysis/Architecture/ArmAarch64.h>
#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <Rose/BinaryAnalysis/Architecture/BasicTypes.h>
#include <Rose/BinaryAnalysis/Architecture/Cil.h>
#include <Rose/BinaryAnalysis/Architecture/Exception.h>
#include <Rose/BinaryAnalysis/Architecture/Intel80286.h>
#include <Rose/BinaryAnalysis/Architecture/Intel8086.h>
#include <Rose/BinaryAnalysis/Architecture/Intel8088.h>
#include <Rose/BinaryAnalysis/Architecture/IntelI386.h>
#include <Rose/BinaryAnalysis/Architecture/IntelI486.h>
#include <Rose/BinaryAnalysis/Architecture/IntelPentium.h>
#include <Rose/BinaryAnalysis/Architecture/IntelPentiumii.h>
#include <Rose/BinaryAnalysis/Architecture/IntelPentiumiii.h>
#include <Rose/BinaryAnalysis/Architecture/IntelPentium4.h>
#include <Rose/BinaryAnalysis/Architecture/Jvm.h>
#include <Rose/BinaryAnalysis/Architecture/Mips32.h>
#include <Rose/BinaryAnalysis/Architecture/Motorola.h>
#include <Rose/BinaryAnalysis/Architecture/Motorola68040.h>
#include <Rose/BinaryAnalysis/Architecture/NxpColdfire.h>
#include <Rose/BinaryAnalysis/Architecture/Powerpc.h>
#include <Rose/BinaryAnalysis/Architecture/Powerpc32.h>
#include <Rose/BinaryAnalysis/Architecture/Powerpc64.h>
#include <Rose/BinaryAnalysis/Architecture/X86.h>

namespace Rose {
namespace BinaryAnalysis {

/** Architecture-specific information and algorithms.
 *
 *  The main feature of this namespace is a class hierarchy rooted at the @ref Rose::BinaryAnalysis::Architecture::Base class. The
 *  hierarchy's leaf subclases contain information and algorithms specific to particular processor architectures such as "Intel
 *  Pentium4", and "NXP Coldfire", while its non-leaf subclasses contain information and algorithms specific to a family of
 *  architectures such as "Intel X86" and "Motorola". The base class provides an architecture-independent API that can be used
 *  throughout the rest of the ROSE library, ROSE tools, and user-created tools, and the subclasses implement that API in
 *  architecture-specific ways.
 *
 *  ROSE has a number of built-in architectures, and additional architectures can be registered at runtime by allocating an
 *  architecture object in the heap and providing a shared-ownership pointer to an architecture registration function in the ROSE
 *  library. There are two ways to register a new architecture:
 *
 *  @li If the architecture is intended to be part of the ROSE library, then it should be compiled when the ROSE library is
 *  compiled, and linked in as part of the ROSE library. These are called the "built-in" architecture definitions and are registered
 *  by modifying the @p initRegistryHelper function in @ref Rose::BinaryAnalysis::Architecture in the "BasicTypes.C" source file.
 *
 *  @li If the architecture is not intended to be part of the ROSE library, then it should be compiled into its own shared library
 *  and provided to binary analysis tools at runtime. These are called "external" architecture definitions. Most tools have a
 *  \"--architectures\" switch that can load the shared library (see @ref Rose::CommandLine::genericSwitches). Each such library
 *  should have a @c registerArchitectures function with C linkage, and this function should create architecture objects and
 *  register them by calling @ref Rose::BinaryAnalysis::Architecture::registerDefinition.
 *
 *  Many of the main analysis objects in ROSE (instruction decoders, instruction dispatchers, partitioner results, assembly
 *  unparsers, etc.) have pointers to an architecture. Additionally, the architecture definition registry can look up an
 *  architecture by various mechanisms, such as by name, by file header, or by finding the best architecture for a binary
 *  interpretation. See the functions in this namespace for more information about the registry.
 *
 *  The ROSE tools come with an example of an external architecture definition called "simple" defined in the "SimpleArch.C" file.
 *  The build system builds a "libSimpleArch.so" shared library and a test case runs various ROSE tools by providing that shared
 *  library at runtime.
 *
 *  @section rose_binaryanalysis_architecture_changes Changes from previous versions
 *
 *  The introduction of this namespace in December 2023 to support user-defined instruction sets necessitated a number of minor
 *  changes but also one major change.
 *
 *  The major change is that instead of defining architecture-specific behavior in the subclasses of @ref SgAsmInstruction, they're
 *  now defined in the subclasses of @ref Rose::BinaryAnalysis::Architecture::Base. This means that the most of the @ref
 *  SgAsmInstruction member functions have moved to @ref Rose::BinaryAnalysis::Architecture::Base and take an @ref SgAsmInstruction
 *  pointer as their first argument. This rearrangement makes it possible for all external architecture definitions to share a
 *  single AST instruction type, @ref SgAsmUserInstruction. We hope that ROSE's ability to describe instruction operands is
 *  sufficiently general that it can be used as-is.
 *
 *  A somewhat major change is that @ref Rose::BinaryAnalysis::Disassembler::Base is no longer the go-to class for obtaining
 *  information about an architecture (other than it has an @ref Rose::BinaryAnalysis::Disassembler::Base::architecture
 *  "architecture" property). Instead of asking a disassembler for a register dictionary, a special register (such as a stack
 *  pointer), an instruction dispatcher (for semantics), or an assembly unparser, one asks the @ref
 *  Rose::BinaryAnalysis::Architecture::Base "architecture". In most situation an architecture is readily available since all the
 *  main objects have a @c architecture property that holds a non-null pointer (even @ref SgAsmInstruction).
 *
 *  The other changes are relatively minor and shouldn't cause many problems. For instance, instruction decoders are no longer
 *  registered individually and looked up at runtime by name. Instead, the architecture is looked up and a decoder is obtained from
 *  the architecture. */
namespace Architecture {}

} // namespace
} // namespace

#endif
#endif
