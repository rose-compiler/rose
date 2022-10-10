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
class Aarch32;
class Aarch64;
class Base;
class Exception;
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

/** Finds a suitable disassembler for a file header.
 *
 *  Looks through the list of registered disassembler factories (from most recently registered to earliest registered) and
 *  returns a clone of the first one whose can_disassemble() predicate returns true.  Throws an @ref Exception if no
 *  suitable disassembler can be found.
 *
 *  Thread safety: Multiple threads can call this class method simultaneously even when other threads are registering
 *  additional disassemblers. */
BasePtr lookup(SgAsmGenericHeader*);

/** Finds a suitable disassembler for a binary interpretation.
 *
 *  Looks through the list of registered disassembler factories (from most recently registered to earliest registered) and
 *  returns a clone of the first one whose can_disassemble() predicate returns true. This is done for each header contained in
 *  the interpretation and the disassembler for each header must match the other headers. An @ref Exception is thrown if no
 *  suitable disassembler can be found.
 *
 *  Thread safety: Multiple threads can call this class method simultaneously even when other threads are registering
 *  additional disassembles. However, no other thread can be changing attributes of the specified interpretation, particularly
 *  the list of file headers referenced by the interpretation. */
BasePtr lookup(SgAsmInterpretation*);

/** Finds a suitable disassembler by name.
 *
 *  Looks through the list of registered disassembler factories (from most recently registered to earliest registered) and
 *  returns a clone of the first one whose name matches the specified string. If the specified string is \"list\" then
 *  the list of disassembler names obtained by @ref isaNames is printed to standard output and the program exits. Otherwise
 *  if no matching disassembler is found, an @ref Exception is thrown.
 *
 *  Thread safety: This function is thread safe. */
BasePtr lookup(const std::string&);

/** List of names recognized by @ref lookup.
 *
 *  Returns the alphabetical list of unique names that the @ref lookup method recognizes. These are the disassemblers that are
 *  actually enabled in this configuration of ROSE.
 *
 *  Thread safety: This function is thread safe. */
std::vector<std::string> isaNames();

/** Append a disassembler factory to the registry.
 *
 *  This stores a reference to the specified disassembler factory. A factory is simply a disassembler that will never be used
 *  to decode any instructions. It's only purpose is to hold settings, to test whether it matches any of the @ref lookup
 *  functions, and to produce by its @ref Base::clone "clone" method a new disassembler that's able to decode instructions.
 *  The various @ref lookup functions will always call the factory's @ref Base::clone "clone" method when returning a
 *  disassembler.
 *
 *  More specific disassembler instances should be registered after more general disassemblers since the lookup() method will
 *  inspect disassemblers in reverse order of their registration.  The ROSE library always initializes the front of the list
 *  with its own disassemblers, but they can be deregistered if desired.
 *
 *  Thread safety: Multiple threads can register disassemblers simultaneously.  However, one seldom does this because the order
 *  that disassemblers are registered determines which disassembler is returned by the lookup() class methods. */
void registerFactory(const BasePtr&);

/** Remove a disassembler factory from the registry.
 *
 *  The most recent occurrence of the specified disassembler factory is removed from the registry if it exists.
 *
 *  Returns true if anything was removed; false otherwise.
 *
 *  Thread safety: This function is thread safe. */
bool deregisterFactory(const BasePtr&);

/** Returns all registered disassembler factories.
 *
 *  The returned value points to the dissemblers that were registered without cloning any of them. These objects are
 *  technically disassembler factories that need to be cloned before they can disassemble anything.
 *
 *  Thread safety: This function is thread safe. */
std::vector<BasePtr> registeredFactories();

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
