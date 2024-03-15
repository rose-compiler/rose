#ifndef ROSE_BinaryAnalysis_Disassembler_BasicTypes_H
#define ROSE_BinaryAnalysis_Disassembler_BasicTypes_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <RoseFirst.h>

#include <Sawyer/Message.h>
#include <Sawyer/SharedPointer.h>

#include <string>
#include <vector>

// Basic types needed by almost all other disassemblers

namespace Rose {
namespace BinaryAnalysis {
namespace Disassembler {

class Exception;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Types for built-in disassemblers
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Aarch32;
using Aarch32Ptr = Sawyer::SharedPointer<Aarch32>;      /** Reference counted pointer for ARM @ref Aarch32 decoder. */

class Aarch64;
using Aarch64Ptr = Sawyer::SharedPointer<Aarch64>;      /** Reference counted pointer for ARM @ref Aarch64 decoder. */

class Base;
using BasePtr = Sawyer::SharedPointer<Base>;            /** Reference counted pointer for disassemblers. */

class M68k;
using M68kPtr = Sawyer::SharedPointer<M68k>;            /** Reference counted pointer for Motorola @ref M68k decoder. */

class Mips;
using MipsPtr = Sawyer::SharedPointer<Mips>;            /** Reference counted pointer for @ref Mips decoder. */

class Null;
using NullPtr = Sawyer::SharedPointer<Null>;            /** Reference counted pointer for @ref Null decoder. */

class Powerpc;
using PowerpcPtr = Sawyer::SharedPointer<Powerpc>;      /** Reference counted pointer for @ref Powerpc decoder. */

class X86;
using X86Ptr = Sawyer::SharedPointer<X86>;              /** Reference counted pointer for Intel @ref X86 decoder. */

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Diagnostics
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Disassembler diagnostic streams. */
extern Sawyer::Message::Facility mlog;

/** Initializes and registers disassembler diagnostic streams. See Diagnostics::initialize(). */
void initDiagnostics();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Convenience functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
