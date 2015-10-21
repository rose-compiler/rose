#ifndef ROSE_BinaryAnalysis_PointerDetection_H
#define ROSE_BinaryAnalysis_PointerDetection_H

#include <BaseSemantics2.h>
#include <Sawyer/Set.h>

namespace rose {
namespace BinaryAnalysis {

// Forwards
class Disassembler;
namespace Partitioner2 {
    class Partitioner;
    class Function;
}


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
 *  pointers from non-pointers is that they're used as addresses when reading from or writing to memory.  We use two passes to
 *  find such pointers: the first pass looks at the address expressions used for memory I/O operations in order to discover
 *  which other instructions were used to define that address.  The second monitors those defining instructions to figure out
 *  whether they read from memory, and if they do, then the address being read is assumed to be the address of a pointer.
 *  Code pointers (e.g., pointers to functions in C), are detected in a similar manner to data pointers: we first look for
 *  situations where a value is written to the EIP register, obtain the list of instructions that defined that value, and then
 *  in a second pass monitor those instructions for memory reads. */
namespace PointerDetection {

/** Initialize diagnostics.
 *
 *  This is normally called as part of ROSE's diagnostics initialization, but it doesn't hurt to call it often. */
void initDiagnostics();

/** Facility for diagnostic output.
 *
 *  The facility can be controlled directly or via ROSE's command-line. */
extern Sawyer::Message::Facility mlog;

/** Type of pointer. These bits represent various knowledge about pointer variables. */
enum PointerType {
    UNKNOWN_PTR=0x000,                                  /**< Pointer variable is of unknown type. */
    DATA_PTR=0x0001,                                    /**< Pointer variable points to data. */
    CODE_PTR=0x0002                                     /**< Pointer variable points to code. */
};

/** Settings to control the pointer analysis. */
struct Settings {
    /** Whether to ignore branches to concrete addresses.
     *
     *  If set, then conditional branches to concrete addresses are ignored, not treated as code pointers.  For instance, the
     *  x86 "je 0x08048504" instruction would not be considered significant for modifying the instruction pointer since both
     *  target addresses are constants. */
    bool ignoreConstIp;

    /** Whether to ignore strange-sized pointers.
     *
     *  If set, then ignore pointer addresses that are not the same width as the stack pointer (data) or instruction pointer
     *  (code). */
    bool ignoreStrangeSizes;

    /** Default settings. */
    Settings()
        : ignoreConstIp(true), ignoreStrangeSizes(true) {}
};

/** Pointer analysis. */
class Analysis {
private:
    Settings settings_;
    typedef Sawyer::Container::Map<uint64_t /*expr_hash*/, SymbolicExpr::Ptr> AddressExpressions;
    InstructionSemantics2::BaseSemantics::DispatcherPtr cpu_;
    bool hasResults_;                                   // Are the following data members initialized?
    bool didConverge_;                                  // Are the following data members valid (else only appoximations)?
    AddressExpressions codePointers_;                   // Memory addresses that hold a pointer to code
    AddressExpressions dataPointers_;                   // Memory addresses that hold a pointer to data
    InstructionSemantics2::BaseSemantics::StatePtr initialState_; // Initial state for analysis
    InstructionSemantics2::BaseSemantics::StatePtr finalState_;   // Final state for analysis

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
    explicit Analysis(Disassembler *d, const Settings &settings = Settings())
        : hasResults_(false), didConverge_(false) {
        init(d);
    }

    /** Construct an analysis using a specified dispatcher.
     *
     *  This constructor uses the supplied dispatcher and associated semantic domain.  For best results, the semantic domain
     *  should be a symbolic domain that uses @ref InstructionSemantics2::BaseSemantics::MemoryCellList "MemoryCellList" and
     *  @ref InstructionSemantics2::BaseSemantics::RegisterStateGeneric "RegisterStateGeneric". These happen to also be the
     *  defaults used by @ref InstructionSemantics::SymbolicSemantics. */
    explicit Analysis(const InstructionSemantics2::BaseSemantics::DispatcherPtr &cpu,
                      const Settings &settings = Settings())
        : cpu_(cpu), hasResults_(false), didConverge_(false) {}

    /** Property: Analysis settings.
     *
     *  Returns the settings that are being used for this analysis. Settings are read-only, initialized by the constructor. */
    const Settings& settings() const { return settings_; }
    
    /** Analyze one function.
     *
     *  This analysis method uses @ref Partitioner2 data structures which are generally faster than using the AST. The
     *  specified function need not be attached to the partitioner. Results of the analysis are stored in this analysis object
     *  to be queried after the analysis completes. */
    void analyzeFunction(const Partitioner2::Partitioner&, const Sawyer::SharedPointer<Partitioner2::Function>&);

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

    /** Code pointers.
     *
     *  These are memory addresses that store a value that was used to initialize the instruction pointer register. If @p sort
     *  is true then the return value is sorted lexically. */
    std::vector<SymbolicExpr::Ptr> codePointers(bool sort=false) const;

    /** Data pointers.
     *
     *  These are memory addresses that store a value that was used as an address to dereference other memory. If @p sort is
     *  true then the return value is sorted lexically. */
    std::vector<SymbolicExpr::Ptr> dataPointers(bool sort=false) const;

    /** Initial state for analysis.
     *
     *  Returns symbolic state that initialized the analysis. This is the state at the function entry address and is
     *  reinitialized each time @ref analyzeFunction is called. This state is cleared by calling @ref clearNonResults, after
     *  which this function returns a null pointer. */
    InstructionSemantics2::BaseSemantics::StatePtr initialState() const {
        return initialState_;
    }

    /** Final state for analysis.
     *
     *  Returns the symbolic state for the function return point. If the function has multiple return points then this is the
     *  state resulting from merging the states after each return.  This state is initialized by calling @ref analyzeFunction.
     *  It is cleared by calling @ref clearNonResults, after which it returns a null pointer. */
    InstructionSemantics2::BaseSemantics::StatePtr finalState() const {
        return finalState_;
    }
    
private:
    void init(Disassembler*);

    InstructionSemantics2::BaseSemantics::RiscOperatorsPtr
    makeRiscOperators(const Partitioner2::Partitioner&) const;

    // Given a potential pointer's r-value, determine if the r-value is a pointer and if so, store its address in the
    // result. The pointer's value and the defining instructions are added to the two sets, and the result is not updated for
    // values and instructions that have already been processed.
    void
    conditionallySavePointer(const InstructionSemantics2::BaseSemantics::SValuePtr &ptrValue,
                             Sawyer::Container::Set<uint64_t> &ptrValueSeen,
                             Sawyer::Container::Set<SgAsmInstruction*> &insnSeen,
                             size_t wordSize, AddressExpressions &result);
};

} // namespace
} // namespace
} // namespace

#endif
