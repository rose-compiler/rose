#ifndef ROSE_BinaryAnalysis_InstructionSemantics2_BaseSemantics_Types_H
#define ROSE_BinaryAnalysis_InstructionSemantics2_BaseSemantics_Types_H
#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

// Basic types needed by almost all other instruction semantics layers

#include <boost/serialization/shared_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <Sawyer/SharedPointer.h>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics2 {

/** Base classes for instruction semantics.
 *
 *  Basically, anything that is common to two or more instruction semantic domains will be factored out and placed in this name
 *  space. */
namespace BaseSemantics {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Documented where they're defined.
class Formatter;
class Merger;
class SValue;
class RegisterState;
class MemoryState;
class State;
class RiscOperators;
class Dispatcher;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Shared-ownership pointers
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Shared-ownership pointer for @ref Merger classes. See @ref heap_object_shared_ownership. */
typedef Sawyer::SharedPointer<Merger> MergerPtr;

/** Shared-ownership pointer to a semantic value in any domain. See @ref heap_object_shared_ownership. */
typedef Sawyer::SharedPointer<SValue> SValuePtr;

/** Shared-ownership pointer to a register state. See @ref heap_object_shared_ownership. */
typedef boost::shared_ptr<RegisterState> RegisterStatePtr;

/** Shared-ownership pointer to a memory state. See @ref heap_object_shared_ownership. */
typedef boost::shared_ptr<MemoryState> MemoryStatePtr;

/** Shared-ownership pointer to a semantic state. See @ref heap_object_shared_ownership. */
typedef boost::shared_ptr<State> StatePtr;

/** Shared-ownership pointer to a RISC operators object. See @ref heap_object_shared_ownership. */
typedef boost::shared_ptr<RiscOperators> RiscOperatorsPtr;

/** Shared-ownership pointer to a semantics instruction dispatcher. See @ref heap_object_shared_ownership. */
typedef boost::shared_ptr<Dispatcher> DispatcherPtr;



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// I/O Properties
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Boolean properties related to I/O.
 *
 *  These Boolean properties keep track of whether a value was read from and/or written to a register or memory state.  Each
 *  state implementation has different capabilities, so see the implementation for details.  In short, @ref
 *  RegisterStateGeneric tracks these properties per bit of each register while memory states generally track them on a
 *  byte-by-byte basis.
 *
 *  Although the register and memory state objects provide the data members for storing this information, the properties are
 *  generally manipulated by higher layers such as the @c readRegister, @c writeRegister, @c readMemory, and @c writeMemory
 *  methods in a @ref BaseSemantics::RiscOperators "RiscOperators" implementation. */
enum InputOutputProperty {
    IO_READ,                                            /**< The location was read on behalf of an instruction. */
    IO_WRITE,                                           /**< The location was written on behalf of an instruction. */
    IO_INIT,                                            /**< The location was written without an instruction. This
                                                         *   typically happens during state initialization. */
    IO_READ_BEFORE_WRITE,                               /**< The location was read without having the IO_WRITE property. */
    IO_READ_AFTER_WRITE,                                /**< The location was read after being written. */
    IO_READ_UNINITIALIZED,                              /**< The location was read without having the IO_WRITE or IO_INIT
                                                         *   property. */
};

/** Set of Boolean properties. */
typedef Sawyer::Container::Set<InputOutputProperty> InputOutputPropertySet;


} // namespace
} // namespace
} // namespace
} // namespace

#endif
#endif
