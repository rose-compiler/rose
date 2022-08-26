#ifndef ROSE_BinaryAnalysis_Disassembler_BasicTypes_H
#define ROSE_BinaryAnalysis_Disassembler_BasicTypes_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Sawyer/SharedPointer.h>

#include <string>
#include <vector>

// Basic types needed by almost all other disassemblers

namespace Rose {
namespace BinaryAnalysis {
namespace Disassembler {

// Documented where they're defined
class Base;
class Aarch32;
class Aarch64;
class M68k;
class Mips;
class Null;
class Powerpc;
class X86;

/** Reference counted pointer for disassemblers. */
using BasePtr = Sawyer::SharedPointer<Base>;

/** Reference counted pointer for ARM @ref Aarch32 decoder. */
using Aarch32Ptr = Sawyer::SharedPointer<Aarch32>;

/** Reference counted pointer for ARM @ref Aarch64 decoder. */
using Aarch64Ptr = Sawyer::SharedPointer<Aarch64>;

/** Reference counted pointer for Motorola @ref M68k decoder. */
using M68kPtr = Sawyer::SharedPointer<M68k>;

/** Reference counted pointer for @ref Mips decoder. */
using MipsPtr = Sawyer::SharedPointer<Mips>;

/** Reference counted pointer for @ref Null decoder. */
using NullPtr = Sawyer::SharedPointer<Null>;

/** Reference counted pointer for @ref Powerpc decoder. */
using PowerpcPtr = Sawyer::SharedPointer<Powerpc>;

/** Reference counted pointer for Intel @ref X86 decoder. */
using X86Ptr = Sawyer::SharedPointer<X86>;

/** Disassembler diagnostic streams. */
extern Sawyer::Message::Facility mlog;

/** Initializes and registers disassembler diagnostic streams. See Diagnostics::initialize(). */
void initDiagnostics();

/** Finds a suitable disassembler. Looks through the list of registered disassembler instances (from most recently registered
 *  to earliest registered) and returns the first one whose can_disassemble() predicate returns true.  Throws an exception if
 *  no suitable disassembler can be found.
 *
 *  Thread safety: Multiple threads can call this class method simultaneously even when other threads are registering
 *  additional disassemblers. */
BasePtr lookup(SgAsmGenericHeader*);

/** Finds a suitable disassembler. Looks through the list of registered disassembler instances (from most recently
 *  registered to earliest registered) and returns the first one whose can_disassemble() predicate returns true. This is
 *  done for each header contained in the interpretation and the disassembler for each header must match the other
 *  headers. An exception is thrown if no suitable disassembler can be found.
 *
 *  Thread safety: Multiple threads can call this class method simultaneously even when other threads are registering
 *  additional disassembles. However, no other thread can be changing attributes of the specified interpretation,
 *  particularly the list of file headers referenced by the interpretation. */
BasePtr lookup(SgAsmInterpretation*);

/** Finds a suitable disassembler.  Looks up a common disassembler by name.  If the name is the word "list" then a
 *  list of known names is printed to <code>std::cout</code>. */
BasePtr lookup(const std::string&);

/** List of names recognized by @ref lookup.
 *
 *  Returns the list of names that the @ref lookup method recognizes. These are the disassemblers that are actually enabled in
 *  this configuration of ROSE. */
std::vector<std::string> isaNames();

/** Register a disassembler instance.
 *
 *  This stores a reference to the specified disassembler and is what the various @ref lookup functions use to find
 *  disassemblers.
 *
 *  More specific disassembler instances should be registered after more general disassemblers since the lookup() method will
 *  inspect disassemblers in reverse order of their registration.
 *
 *  Thread safety: Multiple threads can register disassemblers simultaneously.  However, one seldom does this because the order
 *  that disassemblers are registered determines which disassembler is returned by the lookup() class methods. */
void registerSubclass(const BasePtr&);

/** Disassembler name free function.
 *
 *  This is a convenient way to obtain a disassembler's name without having to include "Base.h", and is therfore useful
 *  in header files that try to include a minimal number of type definitions. */
const std::string& name(const BasePtr&);

} // namespace
} // namespace
} // namespace
#endif
#endif
