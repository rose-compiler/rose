#ifndef ROSE_BinaryAnalysis_PointerDetection_H
#define ROSE_BinaryAnalysis_PointerDetection_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/Disassembler/BasicTypes.h>
#include <Rose/BinaryAnalysis/Partitioner2/BasicTypes.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics.h>
#include <Sawyer/Set.h>

namespace Rose {
namespace BinaryAnalysis {

/** Pointer detection analysis.
 *
 *  This analysis attempts to discover which memory addresses store pointer variables and whether those pointer variables point
 *  to code or data.  The goal is to detect the storage location of things like "arg1", "arg2", and "var2" in the following C
 *  code after it is compiled into a binary:
 *
 *  @code
 *   int f1(bool (*arg1)(), int *arg2) {
 *       int *var2 = arg2;
 *       return arg1() ? 1 : *var2;
 *  }
 *  @endcode
 *
 *  Depending on how the binary is compiled (e.g., which compiler optimizations where applied), it may or may not be possible
 *  to detect all the pointer variables.  On the other hand, the compiler may generate temporary pointers that don't exist in
 *  the source code. Since binary files have no explicit type information (except perhaps in debug tables upon which we don't
 *  want to depend), we have to discover that something is a pointer by how it's used.  The property that distinguishes data
 *  pointers from non-pointers is that they're used as addresses when reading from or writing to memory.
 *
 *  @section binary_ptrdetect_algo Algorithm
 *
 *  The algorithm works by performing a data-flow analysis in the symbolic domain with each CFG vertex also keeping track of
 *  which memory locations are read.  When the data-flow step completes, the algorithm scans all memory locations (across all
 *  CFG vertices) to get a list of addresses.  Each address expression includes a list of all instructions that were used to
 *  define the address. For instance, given this simpler code:
 *
 *  @code
 *  ; int deref(int *ptr, int index) { return ptr[index]; }
 *  L0: push ebp
 *  L1: mov ebp, esp
 *  L3: mov eax, [ebp+8]
 *  L6: mov ecx, [ebp+12]
 *  L9: mov eax, [eax + ecx*4]
 *  Lc: leave
 *  Ld: ret
 *  @endcode
 *
 *  L9 reads from memory address `eax + ecx * 4`, and that address was calculated by previous instructions:
 *
 *  @li L3 read a value from the stack, therefore L3 is a definer of EAX's value before L9
 *  @li L6 read a value from the stack, therefore L6 is a definer of ECX's value before L9
 *  @li L9 performed arithmetic on EAX and ECX, the result of which is defined by L3, L6, and L9.
 *
 *  Other addresses in addition to the one read by L9 are:
 *
 *  @li The return address stored at the top of the initial stack used by the @c RET instruction. Defined by L0 and Lc.
 *  @li The location of the first program argument, defined by L0 and L3.
 *  @li The location of the second program argument, defined by L0 and L6.
 *  @li The location of the saved EBP, defined by L0.
 *
 *  A second step (not requiring a second data-flow, but using information gathered by the first data flow), looks at addresses
 *  that were read by instructions that defined an address. For instance, L3, L6, and L9 are the instructions that defined the
 *  address used by L9, and all three of them read some memory:
 *
 *  @li L3 read the first argument starting at four bytes past the original ESP.
 *  @li L6 read the second argument starting at eight bytes past the original ESP.
 *  @li L9 read an element of the array.
 *
 *  Since L9 reads from the same address whose definers we are processing, we discard the information from L9, keeping only the
 *  two reads from L3 and L6.  Both of these reads match the width of the stack pointer, therefore we keep both (this is an
 *  optional setting for this analysis) and the analysis deems them "addressses of data pointers".  Incidentally, the width of
 *  the stack pointer is used as the width of data pointers, and the width of the instruction pointer is used as the width of
 *  code pointers.  The result is that eight bytes on the stack are deemed addresses of data pointers. They are:
 *
 *  @code
 *  (add[32] esp_0[32] 0x00000004[32])
 *  (add[32] esp_0[32] 0x00000005[32])
 *  (add[32] esp_0[32] 0x00000006[32])
 *  (add[32] esp_0[32] 0x00000007[32])
 *  (add[32] esp_0[32] 0x00000008[32])
 *  (add[32] esp_0[32] 0x00000009[32])
 *  (add[32] esp_0[32] 0x0000000a[32])
 *  (add[32] esp_0[32] 0x0000000b[32])
 *  @endcode
 *
 *  An astute observer will notice that the algorithm has detected that both "ptr" and "index" are detected as
 *  pointers. Although they are not "pointers" per se in the C language, they are indeed both pointers by some definition of
 *  assembly language: they're both used as indexes into a global memory address space.
 *
 *  The analysis also detects other pointers that are not evident from the C source code: EBP's stored location just below the
 *  original top-of-stack is a pointer, and the return address stored at the top of the stack is a pointer.
 *
 *  @section binary_ptrdetect_usage Usage
 *
 *  Like most binary analysis functionality, binary pointer detection is encapsulated in its own namespace. The main class,
 *  @ref Analysis, performs most of the work. A user instantiates an analysis object giving it a certain configuration at the
 *  same time. He then invokes one of its analysis methods, such @ref Analysis::analyzeFunction, one or more times and queries
 *  the results after each analysis.  The results are returned as symbolic address expressions relative to some initial state.
 *
 *  The "testPointerDetection.C" tester has an example use case:
 *
 *  @snippet testPointerDetection.C documentation guts */
namespace PointerDetection {

/** Initialize diagnostics.
 *
 *  This is normally called as part of ROSE's diagnostics initialization, but it doesn't hurt to call it often. */
void initDiagnostics();

/** Facility for diagnostic output.
 *
 *  The facility can be controlled directly or via ROSE's command-line. */
extern Sawyer::Message::Facility mlog;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Settings
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Settings to control the pointer analysis. */
class Settings {
public:
    /** Whether to ignore branches to concrete addresses.
     *
     *  If set, then conditional branches to concrete addresses are ignored, not treated as code pointers.  For instance, the
     *  x86 "je 0x08048504" instruction would not be considered significant for modifying the instruction pointer since both
     *  target addresses are constants. */
    bool ignoreConstIp = true;

    /** Whether to ignore strange-sized pointers.
     *
     *  If set, then ignore pointer addresses that are not the same width as the stack pointer (data) or instruction pointer
     *  (code). */
    bool ignoreStrangeSizes = true;

    /** Save information about data pointers. */
    bool saveDataPointers = true;

    /** Save information about code pointers. */
    bool saveCodePointers = true;

    /** Save the pointer variable addresses in the results. */
    bool savePointerVas = true;

    /** Save information about where pointer variables are accessed. */
    bool savePointerAccesses = true;

    /** Save pointer accessed values if pointer accesses are saved. */
    bool savePointerAccessValues = true;

    /** Save information about where pointer values are dereferenced. */
    bool savePointerDereferences = true;

    /** Save pointer dereferenced values if dereferences are saved. */
    bool savePointerDereferenceValues = true;

    /** Threshold for replacing large symbolic expressions with new variables. */
    uint64_t symbolicTrimThreshold = std::numeric_limits<uint64_t>::max();

    /** Maximum data-flow iteration factor.
     *
     *  The iteration factor is multiplied by the number of vertices in the function control flow graph to determine
     *  how many data flow iterations occur before giving up. */
    size_t maximumDataFlowIterationFactor = 5;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PointerDescriptor
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Description of one pointer. */
class PointerDescriptor {
public:
    /** Information about how a pointer is dereferenced. */
    enum Direction {
        READ,                                           /**< Pointer is used to read from memory. */
        WRITE                                           /**< Pointer is used to write to memory. */
    };

    /** Description of accessing memory. */
    struct Access {
        rose_addr_t insnVa;                             /**< Instruction location where memory is accessed. */
        Direction direction;                            /**< Whether memory is read or written. */
        SymbolicExpression::Ptr value;                  /**< Value read or written. */

        Access(rose_addr_t insnVa, Direction direction, const SymbolicExpression::Ptr &value)
            : insnVa(insnVa), direction(direction), value(value) {}

        bool operator<(const Access &other) const {
            if (insnVa != other.insnVa)
                return insnVa < other.insnVa;
            if (direction != other.direction)
                return direction < other.direction;
            if (value && other.value) {
                return value->hash() < other.value->hash();
            } else {
                return !value && other.value;
            }
        }
    };

    SymbolicExpression::Ptr pointerVa;                  /**< Symbolic address where pointer variable is stored. */
    size_t nBits;                                       /**< Width of pointer in bits. */
    std::set<Access> pointerAccesses;                   /**< Where pointer variable's value was accessed. */
    std::set<Access> dereferences;                      /**< Where pointer was dereferenced. */

    PointerDescriptor(const SymbolicExpression::Ptr &pointerVa, size_t nBits, rose_addr_t insnVa, Direction dir,
                      const SymbolicExpression::Ptr &pointerValue)
        : pointerVa(pointerVa), nBits(nBits) {
        pointerAccesses.insert(Access(insnVa, dir, pointerValue));
    }
};

/** Set of pointers. */
using PointerDescriptors = std::list<PointerDescriptor>;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Pointer analysis
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Pointer analysis.
 *
 *  This class is the main analysis class for pointer detection.  See the @ref Rose::BinaryAnalysis::PointerDetection namespace
 *  for details. */
class Analysis {
public:

private:
    Settings settings_;
    InstructionSemantics::BaseSemantics::DispatcherPtr cpu_;
    bool hasResults_;                                   // Are the following data members initialized?
    bool didConverge_;                                  // Are the following data members valid (else only appoximations)?
    PointerDescriptors codePointers_;                   // Memory addresses that hold a pointer to code
    PointerDescriptors dataPointers_;                   // Memory addresses that hold a pointer to data
    InstructionSemantics::BaseSemantics::StatePtr initialState_; // Initial state for analysis
    InstructionSemantics::BaseSemantics::StatePtr finalState_;   // Final state for analysis

public:
    /** Default constructor.
     *
     *  This creates an analyzer that is not suitable for analysis since it doesn't know anything about the architecture it
     *  would be analyzing.  This is mostly for use in situations where an analyzer must be constructed as a member of another
     *  class's default constructor, in containers that initialize their contents with a default constructor, etc. */
    Analysis()
        : hasResults_(false), didConverge_(false) {}

    /** Construct an analysis using a specific disassembler.
     *
     *  This constructor chooses a symbolic domain and a dispatcher appropriate for the disassembler's architecture. */
    explicit Analysis(const Disassembler::BasePtr &d, const Settings &settings = Settings())
        : settings_(settings), hasResults_(false), didConverge_(false) {
        init(d);
    }

    /** Construct an analysis using a specified dispatcher.
     *
     *  This constructor uses the supplied dispatcher and associated semantic domain.  For best results, the semantic domain
     *  should be a symbolic domain that uses @ref InstructionSemantics::BaseSemantics::MemoryCellList "MemoryCellList" and
     *  @ref InstructionSemantics::BaseSemantics::RegisterStateGeneric "RegisterStateGeneric". These happen to also be the
     *  defaults used by @ref InstructionSemantics::SymbolicSemantics. */
    explicit Analysis(const InstructionSemantics::BaseSemantics::DispatcherPtr &cpu,
                      const Settings &settings = Settings())
        : settings_(settings), cpu_(cpu), hasResults_(false), didConverge_(false) {}

    /** Property: Analysis settings.
     *
     *  Returns the settings that are being used for this analysis. Settings are read-only, initialized by the constructor. */
    const Settings& settings() const { return settings_; }
    
    /** Analyze one function.
     *
     *  This analysis method uses @ref Partitioner2 data structures which are generally faster than using the AST. The
     *  specified function need not be attached to the partitioner. Results of the analysis are stored in this analysis object
     *  to be queried after the analysis completes. */
    void analyzeFunction(const Partitioner2::PartitionerConstPtr&, const Sawyer::SharedPointer<Partitioner2::Function>&);

    /** Whether a function has been analyzed.
     *
     *  Returns true if this analysis object holds results from analyzing a function. The results might be only approximations
     *  depending on whether @ref didConverge also returns true. */
    bool hasResults() const { return hasResults_; }

    /** Whether the analysis results are valid.
     *
     *  Returns true if @ref hasResults is true and the analysis converted to a solution. If the analysis did not converge then
     *  the other results are only approximations. */
    bool didConverge() const { return didConverge_; }

    /** Clear analysis results.
     *
     *  Resets the analysis results so it looks like this analyzer is initialized but has not run yet.  When this method
     *  returns, @ref hasResults and @ref didConverge will both return false. */
    void clearResults();

    /** Clears everything but results.
     *
     *  This resets the virtual CPU to the null pointer, possibly freeing some memory if the CPU isn't being used for other
     *  things. Once the CPU is removed it's no longer possible to do more analysis with this object. */
    void clearNonResults();

    /** Property: Code pointers.
     *
     *  These are memory addresses that store a value that was used to initialize the instruction pointer register. If @p sort
     *  is true then the return value is sorted lexically. */
    const PointerDescriptors& codePointers() const {
        return codePointers_;
    }

    /** Property: Data pointers.
     *
     *  These are memory addresses that store a value that was used as an address to dereference other memory. If @p sort is
     *  true then the return value is sorted lexically. */
    const PointerDescriptors& dataPointers() const {
        return dataPointers_;
    }
    
    /** Initial state for analysis.
     *
     *  Returns symbolic state that initialized the analysis. This is the state at the function entry address and is
     *  reinitialized each time @ref analyzeFunction is called. This state is cleared by calling @ref clearNonResults, after
     *  which this function returns a null pointer. */
    InstructionSemantics::BaseSemantics::StatePtr initialState() const {
        return initialState_;
    }

    /** Final state for analysis.
     *
     *  Returns the symbolic state for the function return point. If the function has multiple return points then this is the
     *  state resulting from merging the states after each return.  This state is initialized by calling @ref analyzeFunction.
     *  It is cleared by calling @ref clearNonResults, after which it returns a null pointer. */
    InstructionSemantics::BaseSemantics::StatePtr finalState() const {
        return finalState_;
    }
    
private:
    void init(const Disassembler::BasePtr&);

    InstructionSemantics::BaseSemantics::RiscOperatorsPtr
    makeRiscOperators(const Partitioner2::PartitionerConstPtr&) const;

    // Prints instructions to the mlog[DEBUG] diagnostic stream if that stream is enabled.
    void
    printInstructionsForDebugging(const Partitioner2::PartitionerConstPtr&, const Sawyer::SharedPointer<Partitioner2::Function>&);

    // Given a potential pointer's r-value, determine if the r-value is a pointer and if so, store its address in the
    // result. The pointer's value and the defining instructions are added to the two sets, and the result is not updated for
    // values and instructions that have already been processed.
    void
    conditionallySavePointer(const InstructionSemantics::BaseSemantics::SValuePtr &ptrValue,
                             Sawyer::Container::Set<uint64_t> &ptrValueSeen, PointerDescriptors &result);

    // Prune results based on settings
    void pruneResults(PointerDescriptors&);
};

} // namespace
} // namespace
} // namespace

#endif
#endif
