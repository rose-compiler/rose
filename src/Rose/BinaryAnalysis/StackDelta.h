#ifndef ROSE_BinaryAnalysis_StackDelta_H
#define ROSE_BinaryAnalysis_StackDelta_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/Disassembler/BasicTypes.h>
#include <Rose/BinaryAnalysis/Partitioner2/BasicTypes.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics.h>
#include <Sawyer/Map.h>

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
#include <boost/serialization/access.hpp>
#include <boost/serialization/split_member.hpp>
#endif

namespace Rose {
namespace BinaryAnalysis {

// Forwards
namespace Partitioner2 {
    namespace DataFlow {
        class InterproceduralPredicate;
    }
}

/** Stack delta analysis.
 *
 *  See the @ref Analysis class in this namespace. */
namespace StackDelta {

/** Initialize diagnostics.
 *
 *  This is normally called as part of ROSE's diagnostics initialization, but it doesn't hurt to call it often. */
void initDiagnostics();

/** Facility for diagnostic output.
 *
 *  The facility can be controlled directly or via ROSE's command-line. */
extern Sawyer::Message::Facility mlog;

/** Initialize this namespace.
 *
 *  This is called from @ref Rose::initialize, and must be called before any other functions in this class are called. */
void initNamespace();

/** Stack delta anzalyzer.
 *
 *  Local variables within a function are generally found at constant offsets from a stack frame located on the stack. Some
 *  architectures have a stack frame pointer register that points to the current frame, but even on these architectures the
 *  register can sometimes be used for other purposes.  If there is no frame pointer register, then the stack frame still
 *  exists and can be found at a constant offset from the stack pointer's initial value (at the start of the function).
 *
 *  This class performs a data-flow analysis to attempt to assign stack deltas to each instruction: one delta at the start of
 *  the instruction and one delta after the instruction executes. A stack delta is the difference between the current
 *  top-of-stack and the original top-of-stack.
 *
 *  If a stack delta is known at a particular instruction, then it can be used to find the original stack pointer by
 *  subtracting it from the current stack pointer.  For example, if a function's first instruction is x86 "PUSH EAX" then the
 *  pre-instruction delta is zero and the post-instruction delta will be -4 due to the "push" decrementing ESP by four. After
 *  the "push", subtracting -4 from the current ESP value will give you the original ESP, from which you can find the frame. */
class Analysis {
public:
    typedef Sawyer::Container::Map<rose_addr_t, InstructionSemantics::BaseSemantics::SValuePtr> DeltasPerAddress;
    typedef std::pair<InstructionSemantics::BaseSemantics::SValuePtr,
                      InstructionSemantics::BaseSemantics::SValuePtr> SValuePair;
    typedef Sawyer::Container::Map<rose_addr_t, SValuePair> SValuePairPerAddress;

private:
    InstructionSemantics::BaseSemantics::DispatcherPtr cpu_;
    Sawyer::Optional<rose_addr_t> initialConcreteStackPointer_; // where to start

    bool hasResults_;                                   // Are the following data members initialized?
    bool didConverge_;                                  // Are the following data membeers valid (else only approximations)?

    SValuePair functionStackPtrs_;                      // Initial and final stack pointers
    InstructionSemantics::BaseSemantics::SValuePtr functionDelta_; // Stack delta for entire function
    
    SValuePairPerAddress bblockStackPtrs_;              // Per-basic block initial and final stack pointers
    DeltasPerAddress bblockDeltas_;                     // Stack delta per basic block (net effect of BB on stack ptr)

    SValuePairPerAddress insnStackPtrs_;                // Per-instruction initial and final stack pointers
    SValuePairPerAddress insnFramePtrs_;                // Per-instruction initial and final frame pointers if known
    DeltasPerAddress insnSpDeltas_;                     // Stack delta per instruction (net effect of insn on stack ptr)
    bool hasConsistentFramePointer_ = false;            // Arch has FP register that's used as a frame pointer for this function

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
private:
    friend class boost::serialization::access;

    template<class S>
    void serializeCommon(S &s, const unsigned version) {
        if (version < 1)
            s & BOOST_SERIALIZATION_NVP(cpu_);
        s & BOOST_SERIALIZATION_NVP(initialConcreteStackPointer_);
        s & BOOST_SERIALIZATION_NVP(hasResults_);
        s & BOOST_SERIALIZATION_NVP(didConverge_);
        s & BOOST_SERIALIZATION_NVP(functionStackPtrs_);
        s & BOOST_SERIALIZATION_NVP(functionDelta_);
        s & BOOST_SERIALIZATION_NVP(bblockStackPtrs_);
        s & BOOST_SERIALIZATION_NVP(bblockDeltas_);
        s & BOOST_SERIALIZATION_NVP(insnStackPtrs_);
        s & BOOST_SERIALIZATION_NVP(insnSpDeltas_);
        if (version >= 2) {
            s & BOOST_SERIALIZATION_NVP(insnFramePtrs_);
            s & BOOST_SERIALIZATION_NVP(hasConsistentFramePointer_);
        }
    }

    template<class S>
    void save(S &s, const unsigned version) const {
        const_cast<Analysis*>(this)->serializeCommon(s, version);
        if (version >= 1) {
            std::string archName;
            InstructionSemantics::BaseSemantics::RiscOperators::Ptr ops;
            if (cpu_) {
                archName = Architecture::name(cpu_->architecture());
                ops = cpu_->operators();
            }
            s & BOOST_SERIALIZATION_NVP(archName);
            s & BOOST_SERIALIZATION_NVP(ops);
        }
    }

    template<class S>
    void load(S &s, const unsigned version) {
        serializeCommon(s, version);
        if (version >= 1) {
            std::string archName;
            InstructionSemantics::BaseSemantics::RiscOperators::Ptr ops;
            s & BOOST_SERIALIZATION_NVP(archName);
            s & BOOST_SERIALIZATION_NVP(ops);
            if (!archName.empty())
                cpu_ = Architecture::newInstructionDispatcher(archName, ops);
        }
    }

    BOOST_SERIALIZATION_SPLIT_MEMBER();
#endif
    
public:
    /** Default constructor.
     *
     *  Creates an analyzer that is not suitable for analysis since it doesn't know anything about the architecture it would be
     *  analyzing. This is mostly for use in situations where an analyzer must be constructed as a member of another class's
     *  default constructor, in containers that initialize their contents with a default constructor, etc. */
    Analysis()
        : hasResults_(false), didConverge_(false) {}

    /** Construct an analyzer using a specified disassembler.
     *
     *  This constructor chooses a symbolic domain and a dispatcher appropriate for the disassembler's architecture. */
    explicit Analysis(const Disassembler::BasePtr &d)
        : hasResults_(false), didConverge_(false) {
        init(d);
    }

    /** Construct an analysis using a specified dispatcher.
     *
     *  This constructor uses the supplied dispatcher and associated semantic domain. For best results, the semantic domain should
     *  be a symbolic domain that uses @ref InstructionSemantics::BaseSemantics::RegisterStateGeneric "RegisterStateGeneric". The
     *  memory state can be the @ref InstructionSemantics::NullSemantics memory state to speed up dataflow converging when the stack
     *  pointer is known to not be saved/restored (which is usually the case), and is what the analysis uses when no state is
     *  specified. */
    explicit Analysis(const InstructionSemantics::BaseSemantics::DispatcherPtr &cpu)
        : cpu_(cpu), hasResults_(false), didConverge_(false) {}

    /** Property: Initial value to use for stack pointers.
     *
     *  A concrete value can be specified for the initial stack pointer, in which case many arithmetic operations on the stack
     *  pointer are constant folded, which leads to more data-flow solutions.  The default is to not initialize the stack
     *  pointer at the start of the analysis.
     *
     *  @{ */
    Sawyer::Optional<rose_addr_t> initialConcreteStackPointer() const { return initialConcreteStackPointer_; }
    void initialConcreteStackPointer(const Sawyer::Optional<rose_addr_t> &val) { initialConcreteStackPointer_ = val; }
    /** @} */

    /** Analyze one function.
     *
     *  This analysis method uses @ref Partitioner2 data structures which are generally faster than using the AST. The
     *  specified function need not be attached to the partitioner. Results of the analysis are stored in this analysis object
     *  to be queried after the analysis completes. */
    void
    analyzeFunction(const Partitioner2::PartitionerConstPtr&, const Partitioner2::FunctionPtr&,
                    Partitioner2::DataFlow::InterproceduralPredicate&);

    /** Whether a function has been analyzed.
     *
     *  Returns true if this analysis object holds results from analyzing a function. The results might be only approximations
     *  depending on whether @ref didConverge also returns true. */
    bool hasResults() const { return hasResults_; }

    /** Whether the analysis results are valid.
     *
     *  Returns true if @ref hasResults is true and the analysis converged to a solution. If the analysis did not converge then
     *  the other results are only approximations. */
    bool didConverge() const { return didConverge_; }

    /** Clear analysis results.
     *
     *  Resets the analysis results so it looks like this analyzer is initialized but has not run yet. When this method
     *  returns, @ref hasResults and @ref didConverge will both return false. */
    void clearResults();

    /** Clear stack pointers, not deltas.
     *
     *  Clears the stack pointer results but not the stack deltas. */
    void clearStackPointers();

    /** Clear the frame pointers. */
    void clearFramePointers();

    /** Clear stack deltas, not pointers.
     *
     *  Clears the stack delta results but not the stack pointers. */
    void clearStackDeltas();

    /** Clears everything but results.
     *
     *  This resets the virtual CPU to the null pointer, possibly freeing some memory if the CPU isn't being used for other
     *  things. Once the CPU is removed it's no longer possible to do more analysis. */
    void clearNonResults();

    /** Initial and final stack pointers for an analyzed function.
     *
     *  These are the initial and final stack pointers for the function as determined by the data-flow analysis. Returns null
     *  pointers if the data-flow did not reach the initial and/or final vertices of the function's control flow graph. */
    SValuePair functionStackPointers() const { return functionStackPtrs_; }

    /** Stack delta for an analyzed function.
     *
     *  Returns the net effect that an analyzed function has on the stack pointer. If the data-flow did not complete then
     *  returns a null pointer. See also, @ref functionStackDeltaConcrete. */
    InstructionSemantics::BaseSemantics::SValuePtr functionStackDelta() const { return functionDelta_; }

    /** Concrete stack delta for an analyzed function.
     *
     *  Returns a concrete stack delta for a function if known, otherwise the @ref SgAsmInstruction::INVALID_STACK_DELTA
     *  constant is returned. See also, @ref functionStackDelta. */
    int64_t functionStackDeltaConcrete() const;

    /** Initial and final stack pointers for a basic block.
     *
     *  Returns the incoming and outgoing stack pointers for each basic block as determined by a data-flow analysis. If the
     *  data-flow did not reach the beginning and/or end of the basic block then null pointers are returned. */
    SValuePair basicBlockStackPointers(rose_addr_t basicBlockAddress) const;

    /** Stack delta for an analyzed basic block.
     *
     *  Returns the net effect that an analyzed basic block has on the stack pointer.  If the data-flow did not reach this
     *  basic block then returns a null pointer. See also, @ref basicBlockStackDeltaConcrete. */
    InstructionSemantics::BaseSemantics::SValuePtr basicBlockStackDelta(rose_addr_t basicBlockAddress) const;

    /** Stack delta for block w.r.t. function.
     *
     *  Returns the incoming or outgoing stack delta for a basic block with respect to the beginning of the function. Returns a
     *  null pointer if the data-flow did not reach the beginning or end of this block.
     *
     * @{ */
    InstructionSemantics::BaseSemantics::SValuePtr basicBlockInputStackDeltaWrtFunction(rose_addr_t basicBlockAddress) const;
    InstructionSemantics::BaseSemantics::SValuePtr basicBlockOutputStackDeltaWrtFunction(rose_addr_t basicBlockAddress) const;
    /** @} */

    /** Concrete stack delta for an analyzed basic block.
     *
     *  Returns the concrete stack delta for a basic block if known, otherwise returns the @ref
     *  SgAsmInstruction::INVALID_STACK_DELTA constant. */
    int64_t basicBlockStackDeltaConcrete(rose_addr_t basicBlockAddress) const;

    /** Initial and final stack ponters for an analyzed instruction.
     *
     *  These are the initial and final stack pointers for the instruction as determined by data-flow analysis. Returns null
     *  pointers if the data-flow did not reach the beginning and/or end of the instruction. */
    SValuePair instructionStackPointers(SgAsmInstruction*) const;

    /** Stack delta for an instruction.
     *
     *  Returns the stack delta for a single instruction if known, otherwise a null pointer. See also, @ref
     *  instructionStackDeltaConcrete. The stack delta for an instruction is the difference between the stack pointer after the
     *  instruction executes and the stack pointer before the instruction executes. */
    InstructionSemantics::BaseSemantics::SValuePtr instructionStackDelta(SgAsmInstruction*) const;

    /** Frame delta for an instruction.
     *
     *  Returns the difference between the frame pointer and the stack pointer before or after the instruction executes, or returns
     *  a null pointer if either value is unknown.
     *
     * @{ */
    InstructionSemantics::BaseSemantics::SValuePtr instructionInputFrameDelta(SgAsmInstruction*) const;
    InstructionSemantics::BaseSemantics::SValuePtr instructionOutputFrameDelta(SgAsmInstruction*) const;
    /** @} */

    /** Stack delta for instruction w.r.t. function.
     *
     *  Returns the incoming or outgoing stack delta for an instruction with respect to the beginning of the function. Returns
     *  a null pointer if the data-flow did not reach the beginning or end of the instruction.
     *
     * @{ */
    InstructionSemantics::BaseSemantics::SValuePtr instructionInputStackDeltaWrtFunction(SgAsmInstruction*) const;
    InstructionSemantics::BaseSemantics::SValuePtr instructionOutputStackDeltaWrtFunction(SgAsmInstruction*) const;
    /** @} */

    /** Concrete stack delta for an instruction.
     *
     *  Returns the concrete stack delta for an instruction if known, otherwise returns the @ref
     *  SgAsmInstruction::INVALID_STACK_DELTA constants. See also, @ref instructionStackDelta. */
    int64_t instructionStackDeltaConcrete(SgAsmInstruction*) const;

    /** Update AST with analysis results.
     *
     *  The analysis results are copied into the AST, Instructions in the AST that belong to the function but were not present
     *  during the analysis have their stack deltas set to @ref SgAsmInstruction::INVALID_STACK_DELTA. Only the specified
     *  function, its basic blocks, and their instructions are affected. */
    void saveAnalysisResults(SgAsmFunction*) const;

    /** Clear AST stack deltas.
     *
     *  Clear all stack deltas stored in the specified AST by setting them to @ref SgAsmInstruction::INVALID_STACK_DELTA. */
    static void clearAstStackDeltas(SgNode*);

    /** Virtual CPU used for analysis.
     *
     *  Returns the dispatcher set by the constructor or the latest analysis. If no dispatcher is set in the constructor then
     *  the analysis itself may set one. */
    InstructionSemantics::BaseSemantics::DispatcherPtr cpu() const { return cpu_; }

    /** Convert a symbolic value to an integer.
     *
     *  Converts the specified symbolic value to a 64-bit signed stack delta.  If the symbolic value is a null pointer or is not an
     *  integer, or is wider than 64 bits, then nothing is returned. */
    static Sawyer::Optional<int64_t> toInt(const InstructionSemantics::BaseSemantics::SValuePtr&);

    /** True if the function appears to have a frame pointer.
     *
     *  Returns true if the function has a frame pointer register that appears to be used as a frame pointer register. */
    bool hasConsistentFramePointer() const;

    /** Print multi-line value to specified stream. */
    void print(std::ostream&) const;

public:
    // Used internally. Do not document with doxygen.
    void adjustInstruction(SgAsmInstruction*,
                           const InstructionSemantics::BaseSemantics::SValuePtr &spIn,     // SP before execution
                           const InstructionSemantics::BaseSemantics::SValuePtr &spOut,    // SP after execution
                           const InstructionSemantics::BaseSemantics::SValuePtr &spDelta,  // spOut - spIn
                           const InstructionSemantics::BaseSemantics::SValuePtr &fpIn,     // FP before execution
                           const InstructionSemantics::BaseSemantics::SValuePtr &fpOut);   // FP after execution

private:
    void init(const Disassembler::BasePtr&);
};

std::ostream& operator<<(std::ostream&, const Analysis&);

} // namespace
} // namespace
} // namespace

// Class versions must be at global scope
#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
BOOST_CLASS_VERSION(Rose::BinaryAnalysis::StackDelta::Analysis, 2);
#endif

#endif
#endif
