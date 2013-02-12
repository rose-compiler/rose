///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Binary instruction semantics policy, machine state, and value types for binary clone detection.      __THIS_HEADER_IS_USED__
// This file contains fairly low-level details closely tied to the simulator and is meant to be
// modified by someone that understands how the simulator works.  Higher-level stuff is in other
// source files in this same directory.
//
// The doxygen documentation for BinaryAnalysis::InstructionSemantics namespace is required reading in order to have any hope
// of understanding what this header is doing.  In short, this header is causing the simulator to replace its normal concrete
// semantics domain with ROSE's MultiSemantics domain (see doxygen for the BinaryAnalysis::InstructionSemantics::MultiSemantics
// class and nested classes).  We instantiate a MultiSemantics class template that defines these sub-domains:
//     0. the simulator's usual concrete semantics (RSIM_SemanticsInner)
//     1. interval semantics
//     2. symbolic semantics
//     3. null semantics                (not used, but MultiSemantics requires four sub-domains)
//
// Our multi-domain semantics class is named CloneDetection::Policy and inherits from RSIM_Semantics::OuterPolicy, which in
// turn inherits from ROSE's MultiSemantics.  The simulator's OuterPolicy class extends MultiSemantics in such a way as to make
// OuterPolicy and InnerPolicy have the same interface, at least to the extent that they're used by the simulator proper.
//
// The CloneDetection::Policy's constructor constructs the sub-domains but disables all of them except the simulator's concrete
// domain.  This allows the simulator to run unimpeded until we "trigger" the CloneDetection::Policy via its trigger()
// method. Once the policy is triggered, the rest of our subdomains are enabled and start participating in the simulator's
// instruction semantics calls.  The triggering is controlled by the SemanticController instruction callback defined in
// CloneDetection.C, which is a typical, straightforward instruction callback.
//
// Most of the methods for CloneDetection::Policy are defined in CloneDetectionTpl.h, which is included at the
// end of RSIM_Templates.h. We have to do it this way because some of the simulator's higher-level data structures (like
// RSIM_Thread) are only partially defined here in this header.
//
// The following changes are necessary in order to use the CloneDetection::Policy (see CloneDetection.patch for a patch)
//     In RSIM_SemanticsSettings.h
//         1. Tell the simulator that we'll be using multi-domain semantics by specifying "Paradigm C".
//         2. Set the RSIM_SEMANTICS_POLICY to our CloneDetection::Policy class, using the default state and value type.
//         3. Set sub-domains 1 & 2 to IntervalSemantics and SymbolicSemantics, respectively.
//         4. Include headers for IntervalSemantics, SymbolicSemantics, and this header file.
//     In RSIM_Templates.h
//         5. Include our method definitions from CloneDetectionTpl.h
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef RSIM_CloneDetectionSemantics_H
#define RSIM_CloneDetectionSemantics_H                   /* used by CloneDetection.C */

#if 0 // used to fool scripts/policies/UnusedHeaders
#include "CloneDetectionSemantics.h"
#include "CloneDetectionTpl.h"
#endif

#include "BinaryPointerDetection.h"

namespace CloneDetection {

// Make names for the sub policies.
static const RSIM_SEMANTICS_OUTER_BASE::SP0 CONCRETE = RSIM_SEMANTICS_OUTER_BASE::SP0();
static const RSIM_SEMANTICS_OUTER_BASE::SP1 INTERVAL = RSIM_SEMANTICS_OUTER_BASE::SP1();
static const RSIM_SEMANTICS_OUTER_BASE::SP2 SYMBOLIC = RSIM_SEMANTICS_OUTER_BASE::SP2();

// Make simpler names for sub policy value types.  These are class templates. The rhs is defined in
// RSIM_SemanticsSettings.h. We have to use #define until c++11.
#define CONCRETE_VALUE RSIM_SEMANTICS_OUTER_0_VTYPE
#define INTERVAL_VALUE RSIM_SEMANTICS_OUTER_1_VTYPE
#define SYMBOLIC_VALUE RSIM_SEMANTICS_OUTER_2_VTYPE

// Shorter type names for policies
typedef RSIM_SEMANTICS_OUTER_0_POLICY<RSIM_SEMANTICS_OUTER_0_STATE, RSIM_SEMANTICS_OUTER_0_VTYPE> ConcretePolicy;
typedef RSIM_SEMANTICS_OUTER_1_POLICY<RSIM_SEMANTICS_OUTER_1_STATE, RSIM_SEMANTICS_OUTER_1_VTYPE> IntervalPolicy;
typedef RSIM_SEMANTICS_OUTER_2_POLICY<RSIM_SEMANTICS_OUTER_2_STATE, RSIM_SEMANTICS_OUTER_2_VTYPE> SymbolicPolicy;

// Functions that convert a value from one domain to another.
template <template <size_t> class ValueType, size_t nBits>
CONCRETE_VALUE<nBits> convert_to_concrete(const ValueType<nBits> &value);
template <template <size_t> class ValueType, size_t nBits>
INTERVAL_VALUE<nBits> convert_to_interval(const ValueType<nBits> &value);
template <template <size_t> class ValueType, size_t nBits>
SYMBOLIC_VALUE<nBits> convert_to_symbolic(const ValueType<nBits> &value);

/** Instruction providor for pointer detection analysis. */
class InstructionProvidor {
public:
    RSIM_Process *process;
    InstructionProvidor(RSIM_Process *process): process(process) { assert(process!=NULL); }
    SgAsmInstruction *get_instruction(rose_addr_t va) {
        SgAsmInstruction *insn = NULL;
        try {
            insn = process->get_instruction(va);
        } catch (const DisassemblerX86::Exception&) {
        }
        return insn;
    }
};

typedef BinaryAnalysis::PointerAnalysis::PointerDetection<InstructionProvidor> PointerDetector;


/**************************************************************************************************************************/

/** Initial values to supply for inputs.  These are defined in terms of integers which are then cast to the appropriate size
 *  when needed.  During fuzz testing, whenever the specimen reads from a register or memory location which has never been
 *  written, we consume the next value from this input object. When all values are consumed, this object begins to return only
 *  zero values. */
class InputValues {
public:
    enum Type { POINTER, NONPOINTER, UNKNOWN_TYPE };
    InputValues(): next_integer_(0), next_pointer_(0) {}
    void add_integer(uint64_t i) { integers_.push_back(i); }
    void add_pointer(uint64_t p) { pointers_.push_back(p); }
    uint64_t next_integer() {
        uint64_t retval = next_integer_ < integers_.size() ? integers_[next_integer_] : 0;
        ++next_integer_; // increment even past the end so we know how many inputs were consumed
        return retval;
    }
    uint64_t next_pointer() {
        uint64_t retval = next_pointer_ < pointers_.size() ? pointers_[next_pointer_] : 0;
        ++next_pointer_; // increment even past the end so we know how many inputs were consumed
        return retval;
    }
    size_t integers_consumed() const { return next_integer_; }
    size_t pointers_consumed() const { return next_pointer_; }
    const std::vector<uint64_t> get_integers() const { return integers_; }
    const std::vector<uint64_t> get_pointers() const { return pointers_; }
    size_t num_inputs() const { return integers_consumed() + pointers_consumed(); }
    void reset() { next_integer_ = next_pointer_ = 0; }
    void clear() {
        reset();
        integers_.clear();
        pointers_.clear();
    }
    void shuffle() {
        for (size_t i=0; i<integers_.size(); ++i) {
            size_t j = rand() % integers_.size();
            std::swap(integers_[i], integers_[j]);
        }
        for (size_t i=0; i<pointers_.size(); ++i) {
            size_t j = rand() % pointers_.size();
            std::swap(pointers_[i], pointers_[j]);
        }
    }
    std::string toString() const {
        std::ostringstream ss;
        print(ss);
        return ss.str();
    }
    void print(std::ostream &o) const {
        o <<"non-pointer inputs (" <<integers_.size() <<" total):\n";
        for (size_t i=0; i<integers_.size(); ++i)
            o <<"  " <<integers_[i] <<(i==next_integer_?"\t<-- next input":"") <<"\n";
        if (next_integer_>=integers_.size())
            o <<"  all non-pointers have been consumed; returning zero\n";
        o <<"pointer inputs (" <<pointers_.size() <<" total):\n";
        for (size_t i=0; i<pointers_.size(); ++i)
            o <<"  " <<pointers_[i] <<(i==next_pointer_?"\t<-- next input":"") <<"\n";
        if (next_pointer_>=pointers_.size())
            o <<"  all pointers have been consumed; returning null\n";
    }
        
protected:
    std::vector<uint64_t> integers_;
    std::vector<uint64_t> pointers_;
    size_t next_integer_, next_pointer_;        // May increment past the end of its array
};

/**************************************************************************************************************************/

/** Exception thrown by this semantics domain. */
class Exception {
public:
    Exception(const std::string &mesg): mesg(mesg) {}
    std::string mesg;
};

/** Exception thrown when we've processed too many instructions. This is used by the Policy<>::startInstruction to prevent
 *  infinite loops in the specimen functions. */
class InsnLimitException: public Exception {
public:
    InsnLimitException(const std::string &mesg): Exception(mesg) {}
};

/**************************************************************************************************************************/

/** Bits to track variable access. */
enum {
    NO_ACCESS=0,                        /**< Variable has been neither read nor written. */
    HAS_BEEN_READ=1,                    /**< Variable has been read. */
    HAS_BEEN_WRITTEN=2                  /**< Variable has been written. */ 
};

/** Semantic value to track read/write state of registers. The basic idea is that we have a separate register state object
 *  whose values are instances of this ReadWriteState type. We can use the same RegisterStateX86 template for the read/write
 *  state as we do for the real register state. */
template<size_t nBits>
struct ReadWriteState {
    unsigned state;                     /**< Bit vector containing HAS_BEEN_READ and/or HAS_BEEN_WRITTEN, or zero. */
    ReadWriteState(): state(NO_ACCESS) {}
};

/**************************************************************************************************************************/

/** Collection of output values. The output values are gathered from the instruction semantics state after a specimen function
 *  is analyzed.  The outputs consist of those interesting registers that are marked as having been written to by the specimen
 *  function, and the memory values whose memory cells are marked as having been written to.  We omit status flags since they
 *  are not typically treated as function call results, and we omit the instruction pointer (EIP).
 *
 *  The orders of outputs does not matter for clone detection, but we must use a list rather than a set because we don't define
 *  an operator< for the value types. There's nothing stopping a user from sorting the values, but it might incur the overhead
 *  of calling an SMT solver for the symbolic values, and even then a less-than relation might not be strict. */ 
template <template <size_t> class ValueType>
class Outputs {
public:
    std::list<ValueType<32> > values32;
    std::list<ValueType<8> > values8;
    std::set<uint32_t> get_values() const;
    void print(std::ostream&, const std::string &title="", const std::string &prefix="") const;
    void print(RTS_Message*, const std::string &title="", const std::string &prefix="") const;
    friend std::ostream& operator<<(std::ostream &o, const Outputs &outputs) {
        outputs.print(o);
        return o;
    }
};

/**************************************************************************************************************************/

// If USE_SYMBOLIC_MEMORY is defined then all memory address operations are done in the symbolic domain, otherwise they are
// performed in the concrete domain.  The concrete domain is MUCH faster since alias determination is a simple comparison of
// two integers rather than an expensive call to an SMT solver.  (You can define it right here, or in the makefile).

#ifdef USE_SYMBOLIC_MEMORY
#define MEMORY_ADDRESS_TYPE SYMBOLIC_VALUE<32>
#define MEMORY_ADDRESS_DOMAIN SYMBOLIC
#else // concrete
#define MEMORY_ADDRESS_TYPE CONCRETE_VALUE<32>
#define MEMORY_ADDRESS_DOMAIN CONCRETE
#endif

/** One cell of memory.  A cell contains an address and a byte value and an indicatation of how the cell has been accessed. */
struct MemoryCell {
    MemoryCell(): addr(MEMORY_ADDRESS_TYPE(0)), val(RSIM_SEMANTICS_VTYPE<8>(0)), rw_state(NO_ACCESS) {}
    MemoryCell(const MEMORY_ADDRESS_TYPE &addr, const RSIM_SEMANTICS_VTYPE<8> &val, unsigned rw_state)
        : addr(addr), val(val), rw_state(rw_state) {}
    MEMORY_ADDRESS_TYPE addr;                                   // Virtual address of memory cell
    RSIM_SEMANTICS_VTYPE<8> val;                                // Byte stored at that address
    unsigned rw_state;                                          // NO_ACCESS or HAS_BEEN_READ and/or HAS_BEEN_WRITTEN bits
};
/**************************************************************************************************************************/

/** Mixed-interpretation memory.  Addresses are symbolic expressions and values are multi-domain.  We'll override the
 * multi-domain policy's memory access functions to use this state rather than chaining to the memory states associated with
 * the sub-policies. */
template <template <size_t> class ValueType>
class State: public RSIM_Semantics::OuterState<ValueType> {
public:
#ifdef USE_SYMBOLIC_MEMORY
    typedef std::list<MemoryCell> MemoryCells;                  // list of memory cells in reverse chronological order
#else // concrete
    typedef std::map<uint32_t, MemoryCell> MemoryCells;         // memory cells indexed by address
#endif
    MemoryCells stack_cells;                                    // memory state for stack memory (accessed via SS register)
    MemoryCells data_cells;                                     // memory state for anything that non-stack (e.g., DS register)
    BinaryAnalysis::InstructionSemantics::BaseSemantics::RegisterStateX86<RSIM_SEMANTICS_VTYPE> registers;
    BinaryAnalysis::InstructionSemantics::BaseSemantics::RegisterStateX86<ReadWriteState> register_rw_state;
    InputValues *input_values;                                  // user-supplied input values

    // Write a single byte to memory. The rw_state are the HAS_BEEN_READ and/or HAS_BEEN_WRITTEN bits.
    void mem_write_byte(X86SegmentRegister sr, const MEMORY_ADDRESS_TYPE &addr, const ValueType<8> &value,
                        unsigned rw_state=HAS_BEEN_WRITTEN);

    // Read a single byte from memory.  The active_policies is the bit mask of sub-policies that are currently active. The
    // optional SMT solver is used to prove hypotheses about symbolic expressions (like memory addresses).  If the read
    // operation cannot find an appropriate memory cell, then @p uninitialized_read is set (it is not cleared in the counter
    // case).
    ValueType<8> mem_read_byte(X86SegmentRegister sr, const MEMORY_ADDRESS_TYPE &addr, unsigned active_policies,
                               SMTSolver *solver, bool *uninitialized_read/*out*/);

    // Returns true if two memory addresses can be equal.
    static bool may_alias(const MEMORY_ADDRESS_TYPE &addr1, const MEMORY_ADDRESS_TYPE &addr2, SMTSolver *solver);

    // Returns true if two memory address are equivalent.
    static bool must_alias(const MEMORY_ADDRESS_TYPE &addr1, const MEMORY_ADDRESS_TYPE &addr2, SMTSolver *solver);

    // Reset the analysis state by clearing all memory (sub-policy memory such as simulator concrete is not cleared, only the
    // memory state stored in the MultiSemantics class) and by resetting the read/written status of all registers.
    void reset_for_analysis() {
        stack_cells.clear();
        data_cells.clear();
        registers.clear();
        register_rw_state.clear();
    }

    // Return output values.  These are the interesting general-purpose registers to which a value has been written, and the
    // memory locations to which a value has been written.  The returned object can be deleted when no longer needed.  The EIP,
    // ESP, and EBP registers are not considered to be interesting.  Memory addresses that are less than or equal to the @p
    // stack_frame_top but larger than @p stack_frame_top - @p frame_size are not considered to be outputs (they are the
    // function's local variables). The @p stack_frame_top is usually the address of the function's return EIP, the address
    // that was pushed onto the stack by the CALL instruction.
    Outputs<ValueType> *get_outputs(const MEMORY_ADDRESS_TYPE &stack_frame_top, size_t frame_size, bool verbose=false) const;

    // Printing
    template<size_t nBits>
    void show_value(std::ostream&, const std::string &hdg, const ValueType<nBits>&, unsigned domains) const;
    void print(std::ostream&, unsigned domain_mask=0x07) const;
    friend std::ostream& operator<<(std::ostream &o, const State &state) {
        state.print(o);
        return o;
    }
};

/**************************************************************************************************************************/

// Define the template portion of the CloneDetection::Policy so we don't have to repeat it over and over in the method
// defintions found in CloneDetectionTpl.h.  This also helps Emac's c++-mode auto indentation engine since it seems to
// get confused by complex multi-line templates.
#define CLONE_DETECTION_TEMPLATE template <                                                                                    \
    template <template <size_t> class ValueType> class State,                                                                  \
    template <size_t nBits> class ValueType                                                                                    \
>


CLONE_DETECTION_TEMPLATE
class Policy: public RSIM_Semantics::OuterPolicy<State, ValueType> {
public:
    typedef                         RSIM_Semantics::OuterPolicy<State, ValueType> Super;

    const char *name;                                   // name to use in diagnostic messages
    bool triggered;                                     // Have we turned on any of our domains yet?
    State<ValueType> state;                             // the mixed-semantic state (symbolic address, multi-value)
    unsigned active_policies;                           // Policies that should be active *during* an instruction
    static const rose_addr_t INITIAL_STACK = 0x80000000;// Initial value for the EIP and EBP registers
    InputValues *inputs;                                // Input values to use when reading a never-before-written variable
    const PointerDetector *pointers;                    // Addresses of pointer variables
    size_t ninsns;                                      // Number of instructions processed since last trigger() call
    size_t max_ninsns;                                  // Maximum number of instructions to process after trigger()

    // "Inherit" super class' constructors (assuming no c++11)
    Policy(RSIM_Thread *thread)
        : Super(thread), name(NULL), triggered(false), active_policies(0x07), inputs(NULL), pointers(NULL),
          ninsns(0), max_ninsns(255) {
        init();
    }

    // Initializer used by constructors.  This is where the SMT solver gets attached to the policy.
    void init();

    // Return output values.  These are the general-purpose registers to which a value has been written, and the memory
    // locations to which a value has been written.  The returned object can be deleted when no longer needed.
    Outputs<ValueType> *get_outputs(bool verbose=false) const;

    // Returns the message stream for the calling threads miscellaneous diagnostics.  We try to always use this for output so
    // that we can turn it on/off via simulator's "--debug" switch, so that output from multiple threads is still readable, and
    // so that all lines are prefixed with information about where/when they were produced. See ROSE's RTS_Message doxygen
    // documentation.
    RTS_Message *trace();

    // Calling this method will cause all our subdomains to be activated and the simulator will branch
    // to the specified target_va.
    void trigger(rose_addr_t target_va, InputValues *inputs, const PointerDetector *pointers);

    // We can get control at the beginning of every instruction.  This allows us to do things like enabling/disabling
    // sub-domains based on the kind of instruction.  We could also examine the entire multi-domain state at this point and do
    // something based on that.
    void startInstruction(SgAsmInstruction *insn) /*override*/;

    // We can get control at the end of most instructions by hooking into finishInstruction.  But be warned: any instruction
    // that throws an exception will skip over this.  The simulator's concrete semantics throw various exceptions for things
    // like signal handling, thread termination, specimen segmentation faults, etc.
    void finishInstruction(SgAsmInstruction *insn) /*override*/;

    // You can get control around individual RISC operations by augmenting the particular function.  For instance, here's how
    // you would intercept an XOR operation.
    template<size_t nBits>
    ValueType<nBits> xor_(const ValueType<nBits> &a, const ValueType<nBits> &b);

    // We want to get control of if-then-else operations so that the condition can be evaluated in symbolic context.
    template<size_t nBits>
    ValueType<nBits> ite(const ValueType<1> &cond, const ValueType<nBits> &a, const ValueType<nBits> &b);

    // Functor that counts nodes in a symbolic expression tree.
    struct ExprNodeCounter: public InsnSemanticsExpr::Visitor {
        size_t nnodes;
        ExprNodeCounter(): nnodes(0) {}
        virtual void operator()(const InsnSemanticsExpr::TreeNodePtr&) { ++nnodes; }
    };

    // Calls the node counter for a particular value type.  T must be based on SymbolicSemantics::ValueType<>.
    template<class T>
    void symbolic_expr_complexity(const T &value, ExprNodeCounter *visitor) {
        value.get_expression()->depth_first_visit(visitor);
    }

    // Counts nodes across all symbolic registers.  Note that for x86, some registers have multiple names depending on the part
    // of the physical register being referenced.  Therefore, by counting the complexity of each named register, we're counting
    // most physical registers multiple times.  That's fine, as long as we're consistent.
    size_t symbolic_state_complexity();

    // We want to do something special with memory in order to implement mixed semantics.  Therefore, we override the
    // readMemory() and writeMemory() methods.
    template<size_t nBits>
    ValueType<nBits> readMemory(X86SegmentRegister sr, ValueType<32> addr, const ValueType<1> &cond);
    template<size_t nBits>
    void writeMemory(X86SegmentRegister sr, ValueType<32> addr, const ValueType<nBits> &data, const ValueType<1> &cond,
                     unsigned rw_access=HAS_BEEN_WRITTEN);

    // We need to hook into register access because we might need to convert values from one domain to another.  The
    // writeRegister() method takes an extra optional argument that indicates which bits should be added to the read/written
    // state for that register.
    template<size_t nBits>
    ValueType<nBits> readRegister(const char *regname);
    template<size_t nBits>
    ValueType<nBits> readRegister(const RegisterDescriptor &reg);
    template<size_t nBits>
    void writeRegister(const char *regname, const ValueType<nBits> &val);
    template<size_t nBits>
    void writeRegister(const RegisterDescriptor &reg, const ValueType<nBits> &val, unsigned update_access=HAS_BEEN_WRITTEN);

    // Print the states for each sub-domain and our own state containing the mixed semantics memory.
    void print(std::ostream&, bool abbreviated=false) const;
    friend std::ostream& operator<<(std::ostream &o, const Policy &p) {
        p.print(o);
        return o;
    }
};

} // namespace

#endif
