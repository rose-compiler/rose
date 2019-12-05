#ifndef ROSE_BinaryAnalysis_Variables_H
#define ROSE_BinaryAnalysis_Variables_H

#include <MemoryCellState.h>
#include <Partitioner2/BasicTypes.h>
#include <Sawyer/IntervalMap.h>
#include <Sawyer/Message.h>
#include <Sawyer/Optional.h>

namespace Rose {
namespace BinaryAnalysis {

/** Facilities for detecting variables in binaries. */
namespace Variables {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Basic types
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Set of addresses. */
typedef std::set<rose_addr_t> AddressSet;

/** Mapping from stack offsets to address sets. */
typedef std::map<int64_t /*offset*/, AddressSet> OffsetToAddresses;

/** Mapping from addresses to address sets. */
typedef std::map<rose_addr_t /*globalVa*/, AddressSet /*insns*/> AddressToAddresses;

/** Interval of signed offsets. */
typedef Sawyer::Container::Interval<int64_t> OffsetInterval;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Global variables
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Diagnostic facility. */
extern Sawyer::Message::Facility mlog;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Global utilities
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Initialize diagnostic output. This is called automatically when ROSE is initialized. */
void initDiagnostics();

/** Format a stack offset as a string. */
std::string offsetStr(int64_t offset);

/** Format size as a string. */
std::string sizeStr(uint64_t size);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Base class for variable descriptors
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Describes a local or global variable. */
class BaseVariable {
    rose_addr_t maxSizeBytes_;                      // maximum possible size of this variable in bytes
    AddressSet insnVas_;                            // instructions where the variable was detected that reference the variable
    std::string name_;                              // optional variable name

protected:
    /** Default constructor.
     *
     *  Constructs a descriptor for a variable whose maximum size is zero. */
    BaseVariable()
        : maxSizeBytes_(0) {}

    /** Construct a variable with a given maximum size. */
    BaseVariable(size_t maxSizeBytes, const AddressSet &definingInstructionVas, const std::string &name)
        : maxSizeBytes_(std::min(maxSizeBytes, rose_addr_t(0x7fffffff))), insnVas_(definingInstructionVas), name_(name) {}

public:
    /** Property: Maximum variable size in bytes.
     *
     *  This is the maximum size that the variable could be, measured in bytes. A default constructed object will have a size
     *  of zero.
     *
     * @{ */
    rose_addr_t maxSizeBytes() const { return maxSizeBytes_; }
    void maxSizeBytes(rose_addr_t size);
    /** @} */

    /** Property: Addresses of instructions related to this variable.
     *
     *  This is the set of addresses for the instructions from which this variable was detected. This is typically instructions
     *  that read or write to memory using an offset from the function's frame.
     *
     * @{ */
    const AddressSet& definingInstructionVas() const { return insnVas_; }
    AddressSet& definingInstructionVas() { return insnVas_; }
    void definingInstructionVas(const AddressSet &vas) { insnVas_ = vas; }
    /** @} */

    /** Property: Optional variable name.
     *
     *  There is no constraint on what the variable name may be. At this time it's used mainly for debugging. Therefore it
     *  should always be printed assuming it contains special characters.
     *
     * @{ */
    const std::string& name() const { return name_; }
    void name(const std::string &s) { name_ = s; }
    /** @} */
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Local variable descriptors
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Description of a local variable within a function. */
class LocalVariable: public BaseVariable {
    Partitioner2::FunctionPtr function_;            // function in which local variable exists
    int64_t frameOffset_;                           // offset where variable is located in the function's stack frame

public:
    /** Default constructor.
     *
     *  Creates an invalid (zero-sized) variable descriptor. */
    LocalVariable()
        : frameOffset_(0) {}

    /** Construct a variable descriptor. */
    LocalVariable(const Partitioner2::FunctionPtr&, int64_t frameOffset, rose_addr_t maxSizeBytes,
                  const AddressSet &definingInstructionVas = AddressSet(), const std::string &name = "");

    ~LocalVariable();

    /** Property: Function owning the variable.
     *
     *  A default constructed object will have a null function pointer. All other objects have a valid function pointer.
     *
     * @{ */
    Partitioner2::FunctionPtr function() const;
    void function(const Partitioner2::FunctionPtr&);
    /** @} */

    /** Property: Frame offset.
     *
     *  This is the address of the variable relative to the function's start-of-frame. For multi-byte variables, this is the
     *  lowest address of the variable.  Depending on the architecture, the start-of-frame could be higher or lower than the
     *  variable, thus the return value is signed.
     *
     * @{ */
    int64_t frameOffset() const { return frameOffset_; }
    void frameOffset(int64_t offset) { frameOffset_ = offset; }
    /** @} */

    /** Give variable a defult name.
     *
     *  This variable's name is replaced by a generated name and the name is returned. */
    const std::string& setDefaultName();

    /** Compare two local variables.
     *
     *  Local variables are equal if and only if they belong to the same function, have the same frame offset, and have the
     *  same maximum size or both are default constructed.
     *
     * @{ */
    bool operator==(const LocalVariable &other) const;
    bool operator!=(const LocalVariable &other) const;
    /** @} */

    /** Location within the function stack frame. */
    OffsetInterval interval() const;

    /** Printing local variable.
     *
     * @{ */
    void print(std::ostream&) const;
    std::string toString() const;
    /** @} */

    /** Print local variable descriptor. */
    friend std::ostream& operator<<(std::ostream&, const Rose::BinaryAnalysis::Variables::LocalVariable&);
};

/** Collection of local variables. */
typedef Sawyer::Container::IntervalMap<OffsetInterval, LocalVariable> LocalVariables;

/** Print info about multiple local variables.
 *
 *  This output includes such things as the function to which they belong and the defining instructions. The output is
 *  multi-line, intended for debugging. */
void print(const LocalVariables&,const Partitioner2::Partitioner&, std::ostream &out, const std::string &prefix = "");

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Global variable descriptors
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Description of a global variable. */
class GlobalVariable: public BaseVariable {
    rose_addr_t address_;                               // starting (lowest) virtual address

public:
    /** Default constructor.
     *
     *  Constructs a descriptor for an invalid global variable whose maximum size is zero. */
    GlobalVariable();

    /** Constructor.
     *
     *  Creates a global variable descriptor for a variable whose lowest address is @p startingAddress and whose maximum size
     *  is @p maxSizeBytes bytes. The optional @p definingInstructionVas are the addresses of the instructions from which this
     *  variable was discerned and are usually memory dereferencing instructions. */
    GlobalVariable(rose_addr_t startingAddress, rose_addr_t maxSizeBytes,
                   const AddressSet &definingInstructionVas = AddressSet(), const std::string &name = "");

    /** Property: Starting address.
     *
     *  This property is the lowest address for the variable.
     *
     * @{ */
    rose_addr_t address() const { return address_; }
    void address(rose_addr_t va) { address_ = va; }
    /** @} */

    /** Give variable a defult name.
     *
     *  This variable's name is replaced by a generated name and the name is returned. */
    const std::string& setDefaultName();

    /** Compare two global variable descriptors.
     *
     *  Two global variable descriptors are equal if and only if they have the same starting address and maximum size.
     *
     * @{ */
    bool operator==(const GlobalVariable &other) const;
    bool operator!=(const GlobalVariable &other) const;
    /** @} */

    /** Location of variable in memory. */
    AddressInterval interval() const;

    /** Printing global variable.
     *
     * @{ */
    void print(std::ostream&) const;
    std::string toString() const;
    /** @} */

    /** Print global variable descriptor. */
    friend std::ostream& operator<<(std::ostream&, const Rose::BinaryAnalysis::Variables::GlobalVariable&);
};

/** Maps virtual addresses to global variables.
 *
 *  The global variable will be represented in the map at all addresses that the global variable could occupy.  E.g., if global
 *  variable "g1" starts at 0x4000 and can be up to 1k 4-byte integers, then a record will exist in the map at all addresses
 *  from 0x4000 (inclusive) to 0x4100 (exclusive). */
typedef Sawyer::Container::IntervalMap<AddressInterval, GlobalVariable> GlobalVariables;

/** Print info about multiple global variables.
 *
 *  This output includes such things as their addresses, sizes, and the defining instructions. The output is
 *  multi-line, intended for debugging. */
void print(const GlobalVariables&,const Partitioner2::Partitioner&, std::ostream &out, const std::string &prefix = "");

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Analyzer
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Analysis to find variable locations. */
class VariableFinder {
public:
    /** Settings that control this analysis. */
    struct Settings {};

private:
    Settings settings_;

public:
    /** Construct a new analyzer. */
    explicit VariableFinder(const Settings &settings = Settings());

    /** Find local variables in a function.
     *
     *  Analyzes the given function to find the local (stack) variables, caches and returns the list.  If this information
     *  is already cached, then this method just returns the cached value.
     *
     *  The second argument can be either a function or an instruction. When it's an instruction, we arbitrarily choose one
     *  of the functions that owns the instruction. Usually each instruction is owned by only one function, but there are
     *  exceptions.
     *
     * @{ */
    LocalVariables findLocalVariables(const Partitioner2::Partitioner&, const Partitioner2::FunctionPtr&);
    LocalVariables findLocalVariables(const Partitioner2::Partitioner&, SgAsmInstruction*);
    /** @} */

    /** Find global variables.
     *
     *  Finds global variables by examining all functions represented by the given partitioner. Global variables can be found
     *  only if they're read or written by some code, and not through a pointer. For example, this method detects variable @c g1
     *  but not @c g2 in the following compiled C code:
     *
     * @code
     *     int g1, g2;
     *
     *     int foo() {
     *         return g1;
     *     }
     *
     *     int bar(int *ptr) {
     *         return *ptr; // no static address even if ptr points to g2
     *     }
     * @endcode
     *
     * If previous results are already cached then they're returned, otherwise new results are computed, cached, and returned. */
    GlobalVariables findGlobalVariables(const Partitioner2::Partitioner&);

    /** Removed cached information.
     *
     *  If a function is supplied as an argument, remove information cached by this analysis for that function. If a
     *  partitioner is specified then remove information from all functions.
     *
     * @{ */
    void evict(const Partitioner2::FunctionPtr&);
    void evict(const Partitioner2::Partitioner&);
    /** @} */

    /** Test whether local variable information is cached.
     *
     *  Returns true if @ref findLocalVariables has been run for the specified function and the results are currently
     *  cached in that function. */
    bool isCached(const Partitioner2::FunctionPtr&);

    /** Figure out the amount of stack space reserved by this function for local variables. */
    Sawyer::Optional<uint64_t> functionFrameSize(const Partitioner2::Partitioner&, const Partitioner2::FunctionPtr&);

    /** The register typically used as local variable base address. */
    RegisterDescriptor frameOrStackPointer(const Partitioner2::Partitioner&);

    /** Find stack variable addresses.
     *
     *  Given an instruction, look for operand subexpressions that reference memory based from a stack frame pointer, such as
     *  x86 "mov eax, [ebp - 12]". Returns the set of offsets from the frame pointer. */
    std::set<int64_t> findStackOffsets(const Partitioner2::Partitioner&, SgAsmInstruction*);

    /** Function that owns an instruction.
     *
     *  Given an instruction, return one of the owning functions chosen arbitrarily.  This is the method used by the version
     *  of @ref findLocalVariables that takes an instruction argument. */
    Partitioner2::FunctionPtr functionForInstruction(const Partitioner2::Partitioner&, SgAsmInstruction*);

    /** Find global variable addresses.
     *
     *  Returns a list of addresses that are possibly the beginning of global variables. We're only able to find global
     *  variables that are referenced by instructions that read or write to the variable.  If the global variable is an array
     *  then one of two things happen:
     *
     *  @li If the array elements are only accessed with indices that are unknown statically, then the address of the beginning
     *      of the array is returned.  This applies to C code like:
     *
     * @code
     *      int get(int i) {
     *          return array[i];
     *      }
     *      int sum(int n) {
     *          int s = 0;
     *          for (int i=0; i < n; ++i)
     *              s += array[i];
     *          return s;
     *      }
     * @endcode
     *
     *  @li If the array elements are accessed with constant indices, then it's impossible to distinguish between accessing an
     *      array with constant indexes and accessing a non-array variable. This applies to C code like:
     *
     * @code
     *      int get() {
     *          return array[3];
     *      }
     *      int sum() {
     *          int s = 0;
     *          for (int i=0; i < 4; ++i) // assume loop unrolling optimization
     *              s += array[i];
     *          return s;
     *      }
     * @endcode */
    AddressToAddresses findGlobalVariableVas(const Partitioner2::Partitioner&);

    /** Find address constants in an expression.
     *
     *  Given a symbolic expression, return all the constants that appear in it that have a potential for being addresses. */
    std::set<rose_addr_t> findConstants(const SymbolicExpr::Ptr&);

    /** Find constants syntactically in an instruction. */
    std::set<rose_addr_t> findConstants(SgAsmInstruction*);

    /** Find addresses in memory state.
     *
     *  Given a cell-based symbolic memory state, return all the memory addresses that appear in it. */
    std::set<SymbolicExpr::Ptr> getMemoryAddresses(const InstructionSemantics2::BaseSemantics::MemoryCellStatePtr&);

    /** Find constants in memory.y
     *
     *  Given an cell-based symbolic memory state, return all constants that appear in the cell addresses. */
    std::set<rose_addr_t> findAddressConstants(const InstructionSemantics2::BaseSemantics::MemoryCellStatePtr&);
};

} // namespace
} // namespace
} // namespace

#endif
