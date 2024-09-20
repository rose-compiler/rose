#ifndef ROSE_BinaryAnalysis_Variables_H
#define ROSE_BinaryAnalysis_Variables_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/BasicTypes.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/MemoryCellState.h>
#include <Rose/BinaryAnalysis/Partitioner2/BasicTypes.h>
#include <Rose/BinaryAnalysis/SymbolicExpression.h>

#include <Sawyer/IntervalMap.h>
#include <Sawyer/Message.h>
#include <Sawyer/Optional.h>
#include <Sawyer/Set.h>

#include <boost/serialization/access.hpp>
#include <boost/serialization/split_member.hpp>

#include <chrono>
#include <map>
#include <ostream>
#include <set>
#include <string>
#include <vector>

namespace Rose {
namespace BinaryAnalysis {

/** Facilities for detecting variables in binaries. */
namespace Variables {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Basic types
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Set of addresses. */
typedef Sawyer::Container::Set<rose_addr_t> AddressSet;

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
public:
    /** Whether a variable is read or written. */
    enum class Access {
        READ            = 0x0001,                       /**< Variable is read.  */
        WRITE         = 0x0002,                         /**< Variable is written. */
    };

    /** Whether a variable is read or written by an instruction. */
    using AccessFlags = Sawyer::BitFlags<Access, uint64_t>;

    /** Information about how an instruction accesses a variable. */
    class InstructionAccess {
    public:
        Address insnAddr = 0;                           /**< Address of instruction accessing the variable. */
        AccessFlags access;                             /**< How the instruction accesses the variable. */

    private:
        InstructionAccess() = default;                  // needed for boost::serialization

    public:
        /** Constructor. */
        InstructionAccess(const Address insnAddr, const AccessFlags access)
            : insnAddr(insnAddr), access(access) {}

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_NVP(insnAddr);
            s & BOOST_SERIALIZATION_NVP(access);
        }
#endif
    };

private:
    rose_addr_t maxSizeBytes_ = 0;                      // maximum possible size of this variable in bytes
    std::vector<InstructionAccess> insns_;              // instructions accessing the variable
    std::string name_;                                  // optional variable name

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_NVP(maxSizeBytes_);
        s & BOOST_SERIALIZATION_NVP(insns_);
        s & BOOST_SERIALIZATION_NVP(name_);
    }
#endif
    
protected:
    /** Default constructor.
     *
     *  Constructs a descriptor for a variable whose maximum size is zero. */
    BaseVariable();

    /** Construct a variable with a given maximum size. */
    BaseVariable(size_t maxSizeBytes, const std::vector<InstructionAccess> &definingInstructions, const std::string &name);

public:
    BaseVariable(const BaseVariable&);
    ~BaseVariable();

public:
    /** Property: Maximum variable size in bytes.
     *
     *  This is the maximum size that the variable could be, measured in bytes. A default constructed object will have a size
     *  of zero.
     *
     * @{ */
    rose_addr_t maxSizeBytes() const;
    void maxSizeBytes(rose_addr_t size);
    /** @} */

    /** Property: Defining instructions.
     *
     *  This is information about each instruction that accesses a variable, such as whether the instruction reads or writes to
     *  that variable.
     *
     * @{ */
    const std::vector<InstructionAccess>& instructionsAccessing() const;
    void instructionsAccessing(const std::vector<InstructionAccess>&);
    /** @} */

    /** Property: Addresses of instructions related to this variable.
     *
     *  This is the set of addresses for the instructions from which this variable was detected. This is typically instructions
     *  that read or write to memory using an offset from the function's frame. */
    AddressSet definingInstructionVas() const;

    /** Property: I/O properties.
     *
     *  This property is a set of flags that describe how the variable is accessed. */
    InstructionSemantics::BaseSemantics::InputOutputPropertySet ioProperties() const;

    /** Property: Optional variable name.
     *
     *  There is no constraint on what the variable name may be. At this time it's used mainly for debugging. Therefore it
     *  should always be printed assuming it contains special characters.
     *
     * @{ */
    const std::string& name() const;
    void name(const std::string &s);
    /** @} */

    /** Insert information about how an instruction accesses this variable. */
    void insertAccess(Address insnAddr, AccessFlags);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Local variable descriptors
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Description of a local stack variable within a function. */
class StackVariable: public BaseVariable {
public:
    /** Purpose of variable. */
    enum class Purpose {
        RETURN_ADDRESS,                                 /**< Possible or known return address. */
        FRAME_POINTER,                                  /**< Pointer to previous stack frame. */
        SPILL_AREA,                                     /**< Callee-saved registers. */
        NORMAL,                                         /**< Normal source code level variable. */
        UNKNOWN,                                        /**< Purpose is unknown. */
        OTHER                                           /**< None of the above purposes. */
    };

    /** Boundary between stack variables.
     *
     *  This is the lowest address for a region of variables along with information about what instructions were used to define
     *  this boundary and what purpose the addresses immediately above this boundary serve. */
    struct Boundary {
        int64_t frameOffset = 0;                        /**< Address of boundary with respect to frame pointer. */
        std::vector<InstructionAccess> definingInsns;   /**< Instructions that define this boundary. */
        Purpose purpose = Purpose::UNKNOWN;             /**< Purpose of addresses above this boundary. */
    };

    /** List of boundaries. */
    using Boundaries = std::vector<Boundary>;

private:
    Partitioner2::FunctionPtr function_;            // function in which local variable exists
    int64_t frameOffset_ = 0;                       // offset where variable is located in the function's stack frame
    Purpose purpose_ = Purpose::UNKNOWN;

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned version) {
        s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(BaseVariable);
        s & BOOST_SERIALIZATION_NVP(function_);
        s & BOOST_SERIALIZATION_NVP(frameOffset_);
        if (version >= 1)
            s & BOOST_SERIALIZATION_NVP(purpose_);
    }
#endif
        
public:
    /** Default constructor.
     *
     *  Creates an invalid (zero-sized) variable descriptor. */
    StackVariable();

    /** Construct a variable descriptor. */
    StackVariable(const Partitioner2::FunctionPtr&, int64_t frameOffset, rose_addr_t maxSizeBytes, Purpose,
                  const std::vector<InstructionAccess> &definingInstructions = std::vector<InstructionAccess>(),
                  const std::string &name = "");

    StackVariable(const StackVariable&);
    ~StackVariable();

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
    int64_t frameOffset() const;
    void frameOffset(int64_t offset);
    /** @} */

    /** Property: Purpose.
     *
     *  Areas of a stack frame serve different purposes. This property describes the purpose.
     *
     *  @{ */
    Purpose purpose() const;
    void purpose(Purpose p);
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
    bool operator==(const StackVariable &other) const;
    bool operator!=(const StackVariable &other) const;
    /** @} */

    /** Location within the function stack frame. */
    OffsetInterval interval() const;

    /** Insert a new boundary or adjust an existing boundary.
     *
     *  The boundaries are assumed to be unsorted, and if a new boundary is inserted it is inserted at the end of the list.
     *
     * @{ */
    static Boundary& insertBoundary(Boundaries& /*in,out*/, int64_t frameOffset, const InstructionAccess&);
    static Boundary& insertBoundaryImplied(Boundaries&/*in,out*/, int64_t frameOffset, Address insnAddr);
    /** @} */

    /** Printing local variable.
     *
     * @{ */
    void print(std::ostream&) const;
    std::string toString() const;
    /** @} */

    /** Predicate to test whether variable is valid.
     *
     *  Returns true if the variable is valid, i.e., if it has a non-zero size. Default constructed variables have a zero
     *  size. */
    explicit operator bool() const {
        return !interval().isEmpty();
    }

    /** Predicate to test whether variable is invalid.
     *
     *  Returns true if the variable is invalid, i.e., if it has a zero size. */
    bool operator!() const {
        return interval().isEmpty();
    }

    /** Print local variable descriptor. */
    friend std::ostream& operator<<(std::ostream&, const Rose::BinaryAnalysis::Variables::StackVariable&);
};

/** Collection of local variables organized by frame offsets. */
using StackVariables = Sawyer::Container::IntervalMap<OffsetInterval, StackVariable>;

/** Print info about multiple local variables.
 *
 *  This output includes such things as the function to which they belong and the defining instructions. The output is
 *  multi-line, intended for debugging. */
void print(const StackVariables&, const Partitioner2::PartitionerConstPtr&, std::ostream &out, const std::string &prefix = "");

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Global variable descriptors
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Description of a global variable. */
class GlobalVariable: public BaseVariable {
    rose_addr_t address_ = 0;                           // starting (lowest) virtual address

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(BaseVariable);
        s & BOOST_SERIALIZATION_NVP(address_);
    }
#endif
    
public:
    /** Default constructor.
     *
     *  Constructs a descriptor for an invalid global variable whose maximum size is zero. */
    GlobalVariable();

    /** Constructor.
     *
     *  Creates a global variable descriptor for a variable whose lowest address is @p startingAddress and whose maximum size
     *  is @p maxSizeBytes bytes. The optional @p definingInstruction are the addresses of the instructions from which this
     *  variable was discerned and are usually memory dereferencing instructions. */
    GlobalVariable(rose_addr_t startingAddress, rose_addr_t maxSizeBytes,
                   const std::vector<InstructionAccess> &definingInstructions = std::vector<InstructionAccess>(),
                   const std::string &name = "");

    ~GlobalVariable();

    /** Property: Starting address.
     *
     *  This property is the lowest address for the variable.
     *
     * @{ */
    rose_addr_t address() const;
    void address(rose_addr_t va);
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

    /** Predicate to test whether variable is valid.
     *
     *  Returns true if the variable is valid, i.e., if it has a non-zero size. Default constructed variables have a zero
     *  size. */
    explicit operator bool() const {
        return !interval().isEmpty();
    }

    /** Predicate to test whether variable is invalid.
     *
     *  Returns true if the variable is invalid, i.e., if it has a zero size. */
    bool operator!() const {
        return interval().isEmpty();
    }

    /** Print global variable descriptor. */
    friend std::ostream& operator<<(std::ostream&, const Rose::BinaryAnalysis::Variables::GlobalVariable&);
};

/** Maps virtual addresses to global variables.
 *
 *  The global variable will be represented in the map at all addresses that the global variable could occupy.  E.g., if global
 *  variable "g1" starts at 0x4000 and can be up to 1k 4-byte integers, then a record will exist in the map at all addresses
 *  from 0x4000 (inclusive) to 0x4100 (exclusive). */
typedef Sawyer::Container::IntervalMap<AddressInterval, GlobalVariable> GlobalVariables;

/** Erase some global variables.
 *
 *  Erases global variables from the specified memory region. It is not sufficient to only remove addresses from the map; the
 *  addresses stored in the variables themselves (values in the map) may need to be adjusted so they don't overlap with the erased
 *  range. */
void erase(GlobalVariables&, const AddressInterval &toErase);

/** Print info about multiple global variables.
 *
 *  This output includes such things as their addresses, sizes, and the defining instructions. The output is
 *  multi-line, intended for debugging. */
void print(const GlobalVariables&,const Partitioner2::PartitionerConstPtr&, std::ostream &out, const std::string &prefix = "");

/** Check that the map is consistent.
 *
 *  Test that the keys of the map match up with the variables contained therein. If a map node is found where the interval key for
 *  the node doesn't match the addresses of the global variable stored in that node, then the map contains an inconsistency.  When
 *  an inconsistency is found, and the output stream is enabled then the map is printed and all inconsistencies are highlighted.
 *
 *  Returns the first address interval (key) where an inconsistency is detected, or an empty interval if there are no
 *  inconsistencies. */
AddressInterval isInconsistent(const GlobalVariables&, Sawyer::Message::Stream&);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// StackFrame
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Information about a stack frame.
 *
 *  A stack frame is a contiguous area of the stack with one frame for each function in a call chain. As functions are called
 *  and return, frames are pushed onto and popped from the stack. Typically, when a frame is pushed onto the stack, a pointer
 *  is also added so that the frames form a linked list from most recent function call to the oldest function call.
 *
 *  Typically, a dedicated register points to the most recent frame. E.g., for x86 this is usually the bp/ebp/rbp
 *  register. Sometimes there is no dedicated frame pointer register (such as when x86 is compiled with GCC/LLVM
 *  -fomit-frame-pointer). In these cases, the compiler keeps track of the frame address and when needed, emits code that finds
 *  the frame based on the current value of the stack pointer register (which can change throughout a function's execution).
 *  The frame pointer (explicit or calculated) defines the zero offset for the frame. The frame itself extends in positive
 *  and/or negative directions from this anchor point.
 *
 *  The frame pointer register can point to any position within the frame. For instance, x86 rbp register points to the top of
 *  the frame, whereas PowerPC's r31 register points to the bottom. Likewise, the pointers forming the linked list can point to
 *  any position within the frame. These pointer offsets into the frame are known by the compiler, which adds/subtracts the
 *  appropriate constant to access the desired bytes in the frame.
 *
 *  A stack containing the frames can grow (when frames are pushed) in a positive or negative direction.
 *
 *  A function can access areas of the stack that are outside its own frame.  For instance, x86 function stack arguments are
 *  stored in the caller's frame. Pointers to local variables can point into earlier frames.  The latest function can use parts
 *  of the stack that are after the latest frame and which therefore out outside any frame. */
class StackFrame {
public:
    enum Direction {
        GROWS_UP,                                       /**< New frames are added at higher addresses than old frames. */
        GROWS_DOWN                                      /**< New frames are added at lower addresses than old frames. */
    };

    Direction growthDirection = GROWS_DOWN;             /**< Direction that the stack grows when pushing a new frame. */
    RegisterDescriptor framePointerRegister;            /**< Optional descriptor for register pointing to latest frame. */
    Sawyer::Optional<int64_t> maxOffset;                /**< Maximum frame offset w.r.t. frame pointer. */
    Sawyer::Optional<int64_t> minOffset;                /**< Minimum frame offset w.r.t. frame pointer. */
    Sawyer::Optional<uint64_t> size;                    /**< Size of the frame in bytes if known. */
    std::string rule;                                   /**< Informal rule name used to detect frame characteristics. */
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Analyzer
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Analysis to find variable locations. */
class VariableFinder: public Sawyer::SharedObject {
public:
    /** Settings that control this analysis. */
    struct Settings {
        /** Max time to spend in Method 1 global variable analysis per function. */
        std::chrono::seconds gvarMethod1MaxTimePerFunction;

        Settings()
            : gvarMethod1MaxTimePerFunction(30) {}
    };

    /** Shared ownership pointer for @ref VariableFinder. */
    using Ptr = VariableFinderPtr;

private:
    Settings settings_;

protected:
    explicit VariableFinder(const Settings&);
public:
    ~VariableFinder();

public:
    /** Allocating constructor. */
    static Ptr instance(const Settings &settings = Settings());

    /** Settings for this analysis.
     *
     * @{ */
    const Settings& settings() const { return settings_; }
    Settings& settings() { return settings_; }
    /** @} */

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
    StackVariables findStackVariables(const Partitioner2::PartitionerConstPtr&, const Partitioner2::FunctionPtr&);
    StackVariables findStackVariables(const Partitioner2::PartitionerConstPtr&, SgAsmInstruction*);
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
    GlobalVariables findGlobalVariables(const Partitioner2::PartitionerConstPtr&);

    /** Removed cached information.
     *
     *  If a function is supplied as an argument, remove information cached by this analysis for that function. If a
     *  partitioner is specified then remove information from all functions.
     *
     * @{ */
    void evict(const Partitioner2::FunctionPtr&);
    void evict(const Partitioner2::PartitionerConstPtr&);
    /** @} */

    /** Test whether local variable information is cached.
     *
     *  Returns true if @ref findStackVariables has been run for the specified function and the results are currently
     *  cached in that function. */
    bool isCached(const Partitioner2::FunctionPtr&);

    /** Figure out attributes describing the stack frame for the specified function. */
    StackFrame detectFrameAttributes(const Partitioner2::PartitionerConstPtr&, const Partitioner2::FunctionPtr&);

    /** Initilialize offsets for function prologue.
     *
     *  At the start of a function, we sometimes know where certain things are on the stack and their sizes. For instance, for
     *  powerpc after the function prologue sets up the stack frame, we know that the stack frame header contain two 4-byte
     *  quantities: the pointer to the parent frame, and the LR save area for callees and therefore we can add the three offsets
     *  that delimit the boundaries of these two "variables". */
    void initializeFrameBoundaries(const StackFrame&, const Partitioner2::PartitionerConstPtr&, const Partitioner2::FunctionPtr&,
                                   StackVariable::Boundaries &boundaries /*in,out*/);

#if 0 // [Robb Matzke 2021-10-27]
    /** Find frame location for address.
     *
     *  Given a symbolic address and size in bytes (presumabely from a memory read or write), calculate the part of the stack
     *  frame that's referenced, if any.  Returns the part of the frame that's referenced, or an empty interval if the address
     *  is outside the frame. */
    OffsetInterval referencedFrameArea(const Partitioner2::Partitioner&,
                                       const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&,
                                       const SymbolicExpression::Ptr &address, size_t nBytes);
#endif

    /** Find stack variable addresses.
     *
     *  Given an instruction, look for operand subexpressions that reference memory based from a stack frame pointer, such as
     *  x86 "mov eax, [ebp - 12]". Returns the set of offsets from the frame pointer. */
    std::set<int64_t> findFrameOffsets(const StackFrame&, const Partitioner2::PartitionerConstPtr&, SgAsmInstruction*);

    /** Function that owns an instruction.
     *
     *  Given an instruction, return one of the owning functions chosen arbitrarily.  This is the method used by the version
     *  of @ref findStackVariables that takes an instruction argument. */
    Partitioner2::FunctionPtr functionForInstruction(const Partitioner2::PartitionerConstPtr&, SgAsmInstruction*);

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
    AddressToAddresses findGlobalVariableVas(const Partitioner2::PartitionerConstPtr&);

    /** Find address constants in an expression.
     *
     *  Given a symbolic expression, return all the constants that appear in it that have a potential for being addresses. */
    std::set<rose_addr_t> findConstants(const SymbolicExpression::Ptr&);

    /** Find constants syntactically in an instruction. */
    std::set<rose_addr_t> findConstants(SgAsmInstruction*);

    /** Find addresses in memory state.
     *
     *  Given a cell-based symbolic memory state, return all the memory addresses that appear in it. */
    std::set<SymbolicExpression::Ptr> getMemoryAddresses(const InstructionSemantics::BaseSemantics::MemoryCellStatePtr&);

    /** Find constants in memory.
     *
     *  Given an cell-based symbolic memory state, return all constants that appear in the cell addresses. */
    std::set<rose_addr_t> findAddressConstants(const InstructionSemantics::BaseSemantics::MemoryCellStatePtr&);

#if 0 // [Robb Matzke 2021-10-27]
    /** Return symbolic address of stack variable.
     *
     *  Given a stack variable, return the symbolic address where the variable is located. */
    InstructionSemantics::BaseSemantics::SValuePtr
    symbolicAddress(const Partitioner2::Partitioner&, const StackVariable&,
                    const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&);
#endif

    /** Remove boundaries that are outside a stack frame.
     *
     *  If the frame's lowest address is known, then boundaries that begin before the frame are removed. Except if there is no
     *  boundary at the beginning of the frame, then the greatest boundary before the frame is moved to the beginning of the
     *  frame instead of being removed entirely.
     *
     *  If the frame's upper address is known, then any boundary above that address is removed from the list. */
    void removeOutliers(const StackFrame&, const Partitioner2::PartitionerConstPtr&, const Partitioner2::FunctionPtr&,
                        StackVariable::Boundaries &sortedBoundaries /*in,out*/);

    /** True if memory region contains any decoded instructions. */
    static bool regionContainsInstructions(const Partitioner2::PartitionerConstPtr&, const AddressInterval&);

    /** True if memory region is fully mapped. */
    static bool regionIsFullyMapped(const Partitioner2::PartitionerConstPtr&, const AddressInterval&);

    /** True if memory region is fully mapped with read and write access. */
    static bool regionIsFullyReadWrite(const Partitioner2::PartitionerConstPtr&, const AddressInterval&);

private:
    // Searches for global variables using a per-function data-flow analysis and distinguishing between global variable
    // addresses based on which instructions wrote to those addresses. The data-flow is inter-procedural only for calls to
    // certain functions (such as the x86 get_pc_thunk variety of functions).
    //
    // The return value is a list of global variable addresses and the instructions at which each global variable address
    // was detected.
    AddressToAddresses findGlobalVariableVasMethod1(const Partitioner2::PartitionerConstPtr&);

    // Searches for global variables by running each instruction individually in symbolic semantics and then searching
    // the memory state addresses. All constants found in address expressions are gathered together.
    //
    // The return value is a list of global variable addresses and the instructions at which each global address was detected.
    AddressToAddresses findGlobalVariableVasMethod2(const Partitioner2::PartitionerConstPtr&);

    // Searches for global variables by looking for constants in instruction ASTs.
    //
    // The retun value is the list of constants and the instructions in which each constant appeared.
    AddressToAddresses findGlobalVariableVasMethod3(const Partitioner2::PartitionerConstPtr&);

    // Merge one set of addresses and their defining instructions into another.
    static void merge(AddressToAddresses&, const AddressToAddresses&);
};

} // namespace
} // namespace
} // namespace

// Class versions must be at global scope
BOOST_CLASS_VERSION(Rose::BinaryAnalysis::Variables::StackVariable, 1);

#endif
#endif
