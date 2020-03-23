#ifndef ROSE_BinaryAnalysis_InstructionSemantics2_BaseSemantics_State_H
#define ROSE_BinaryAnalysis_InstructionSemantics2_BaseSemantics_State_H
#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

#include <BaseSemanticsTypes.h>

#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/shared_ptr.hpp>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics2 {
namespace BaseSemantics {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      State
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Base class for semantics machine states.
 *
 *  Binary semantic analysis usually progresses one instruction at a time--one starts with an initial state and the act of
 *  processing an instruction modifies the state.  The State is the base class class for the semantic states of various
 *  instruction semantic policies.  It contains storage for all the machine registers and memory.
 *
 *  Sometimes it's useful to have a state that contains only registers or only memory.  Although this class doesn't allow its
 *  register or memory state children to be null pointers, the @ref NullSemantics class provides register and memory states
 *  that are mostly no-ops.
 *
 *  States must be copyable objects.  Many analyses keep a copy of the machine state for each instruction or each CFG
 *  vertex.
 *
 *  State objects are allocated on the heap and reference counted.  The BaseSemantics::State is an abstract class that defines
 *  the interface.  See the Rose::BinaryAnalysis::InstructionSemantics2 namespace for an overview of how the parts fit
 *  together.  */
class State: public boost::enable_shared_from_this<State> {
    SValuePtr protoval_;                                // Initial value used to create additional values as needed.
    RegisterStatePtr registers_;                        // All machine register values for this semantic state.
    MemoryStatePtr memory_;                             // All memory for this semantic state.

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Serialization
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_NVP(protoval_);
        s & BOOST_SERIALIZATION_NVP(registers_);
        s & BOOST_SERIALIZATION_NVP(memory_);
    }
#endif


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Real constructors
protected:
    // needed for serialization
    State();
    State(const RegisterStatePtr &registers, const MemoryStatePtr &memory);

    // deep-copy the registers and memory
    State(const State &other);

public:
    /** Shared-ownership pointer for a @ref State. See @ref heap_object_shared_ownership. */
    typedef StatePtr Ptr;

public:
    virtual ~State();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Static allocating constructors
public:
    /** Instantiate a new state object with specified register and memory states. */
    static StatePtr instance(const RegisterStatePtr &registers, const MemoryStatePtr &memory) {
        return StatePtr(new State(registers, memory));
    }

    /** Instantiate a new copy of an existing state. */
    static StatePtr instance(const StatePtr &other) {
        return StatePtr(new State(*other));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Virtual constructors
public:
    /** Virtual constructor. */
    virtual StatePtr create(const RegisterStatePtr &registers, const MemoryStatePtr &memory) const {
        return instance(registers, memory);
    }

    /** Virtual copy constructor. Allocates a new state object which is a deep copy of this state. States must be copyable
     *  objects because many analyses depend on being able to make a copy of the entire semantic state at each machine
     *  instruction, at each CFG vertex, etc. */
    virtual StatePtr clone() const {
        StatePtr self = boost::const_pointer_cast<State>(shared_from_this());
        return instance(self);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Dynamic pointer casts.  No-op since this is the base class.
public:
    static StatePtr promote(const StatePtr &x) {
        ASSERT_not_null(x);
        return x;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Other methods that are part of our API. Most of these just chain to either the register state and/or the memory state.
public:
    /** Return the protoval.  The protoval is used to construct other values via its virtual constructors. */
    SValuePtr protoval() const { return protoval_; }

    /** Initialize state.  The register and memory states are cleared. */
    virtual void clear();

    /** Initialize all registers to zero.
     *
     *  Calls the @ref RegisterState::zero method. Memory is not affected. */
    virtual void zero_registers();

    /** Clear all memory locations.
     *
     *  Calls the @ref MemoryState::clear method. Registers are not affected. */
    virtual void clear_memory();

    /** Property: Register state.
     *
     *  This read-only property is the register substate of this whole state. */
    RegisterStatePtr registerState() const {
        return registers_;
    }

    /** Property: Memory state.
     *
     *  This read-only property is the memory substate of this whole state. */
    MemoryStatePtr memoryState() const {
        return memory_;
    }

    /** Read a value from a register.
     *
     *  The @ref BaseSemantics::readRegister implementation simply delegates to the register state member of this state.  See
     *  @ref BaseSemantics::RiscOperators::readRegister for details. */
    virtual SValuePtr readRegister(RegisterDescriptor desc, const SValuePtr &dflt, RiscOperators *ops);

    /** Read register without side effects.
     *
     *  The @ref BaseSemantics::peekRegister implementation simply delegates to the register state member of this state.  See
     *  @ref BaseSemantics::RiscOperators::peekRegister for details. */
    virtual SValuePtr peekRegister(RegisterDescriptor desc, const SValuePtr &dflt, RiscOperators *ops);

    /** Write a value to a register.
     *
     *  The @ref BaseSemantics::writeRegister implementation simply delegates to the register state member of this state.  See
     *  @ref BaseSemantics::RiscOperators::writeRegister for details. */
    virtual void writeRegister(RegisterDescriptor desc, const SValuePtr &value, RiscOperators *ops);

    /** Read a value from memory.
     *
     *  The BaseSemantics::readMemory() implementation simply delegates to the memory state member of this state.  See
     *  BaseSemantics::RiscOperators::readMemory() for details.  */
    virtual SValuePtr readMemory(const SValuePtr &address, const SValuePtr &dflt,
                                 RiscOperators *addrOps, RiscOperators *valOps);

    /** Read from memory without side effects.
     *
     *  The BaseSemantics::peekMemory() implementation simply delegates to the memory state member of this state.  See
     *  BaseSemantics::RiscOperators::peekMemory() for details.  */
    virtual SValuePtr peekMemory(const SValuePtr &address, const SValuePtr &dflt,
                                 RiscOperators *addrOps, RiscOperators *valOps);

    /** Write a value to memory.
     *
     *  The BaseSemantics::writeMemory() implementation simply delegates to the memory state member of this state.  See
     *  BaseSemantics::RiscOperators::writeMemory() for details. */
    virtual void writeMemory(const SValuePtr &addr, const SValuePtr &value, RiscOperators *addrOps, RiscOperators *valOps);

    /** Print the register contents.
     *
     *  This method emits one line per register and contains the register name and its value.
     *
     * @{ */
    void printRegisters(std::ostream &stream, const std::string &prefix = "");
    virtual void printRegisters(std::ostream &stream, Formatter &fmt) const;
    /** @} */

    /** Print memory contents.
     *
     *  This simply calls the MemoryState::print method.
     *
     * @{ */
    void printMemory(std::ostream &stream, const std::string &prefix = "") const;
    virtual void printMemory(std::ostream &stream, Formatter &fmt) const;
    /** @} */

    /** Print the state.  This emits a multi-line string containing the registers and all known memory locations.
     * @{ */
    void print(std::ostream &stream, const std::string &prefix = "") const;
    virtual void print(std::ostream&, Formatter&) const;
    /** @} */

    /** State with formatter. See with_formatter(). */
    class WithFormatter {
        StatePtr obj;
        Formatter &fmt;
    public:
        WithFormatter(const StatePtr &obj, Formatter &fmt): obj(obj), fmt(fmt) {}
        void print(std::ostream &stream) const { obj->print(stream, fmt); }
    };

    /** Used for printing states with formatting. The usual way to use this is:
     * @code
     *  StatePtr obj = ...;
     *  Formatter fmt = ...;
     *  std::cout <<"The value is: " <<(*obj+fmt) <<"\n";
     * @endcode
     *
     * Since specifying a line prefix string for indentation purposes is such a common use case, the
     * indentation can be given instead of a format, as in the following code that indents the
     * prefixes each line of the expression with four spaces.
     *
     * @code
     *  std::cout <<"Current state:\n" <<*(obj + "    ");
     * @code
     * @{ */
    WithFormatter with_format(Formatter &fmt) { return WithFormatter(shared_from_this(), fmt); }
    WithFormatter operator+(Formatter &fmt) { return with_format(fmt); }
    WithFormatter operator+(const std::string &linePrefix);
    /** @} */

    /** Merge operation for data flow analysis.
     *
     *  Merges the @p other state into this state. Returns true if this state changed, false otherwise.  This method usually
     *  isn't overridden in subclasses since all the base implementation does is invoke the merge operation on the memory state
     *  and register state. */
    virtual bool merge(const StatePtr &other, RiscOperators *ops);
};

std::ostream& operator<<(std::ostream&, const State&);
std::ostream& operator<<(std::ostream&, const State::WithFormatter&);

} // namespace
} // namespace
} // namespace
} // namespace

#endif
#endif
