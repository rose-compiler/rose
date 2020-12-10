#ifndef ROSE_BinaryAnalysis_BinaryNoOperation_H
#define ROSE_BinaryAnalysis_BinaryNoOperation_H
#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

#include <BaseSemantics2.h>
#include <Sawyer/Message.h>

namespace Rose {
namespace BinaryAnalysis {

class Disassembler;

/** Analysis that looks for no-op equivalents. */
class NoOperation {
public:
    /** An interval of instruction indices. */
    typedef Sawyer::Container::Interval<size_t> IndexInterval;

    /** A vector of instruction indices. */
    typedef std::vector<IndexInterval> IndexIntervals;

    /** Base class for normalizing a state. */
    class StateNormalizer: public Sawyer::SharedObject {
    protected:
        size_t ignorePoppedMemory_;
        StateNormalizer(): ignorePoppedMemory_(8192) {}
    public:
        virtual ~StateNormalizer() {}

        /** Shared ownership pointer to a @ref StateNormalizer. See @ref heap_object_shared_ownership. */
        typedef Sawyer::SharedPointer<StateNormalizer> Ptr;

        /** Allocating constructor. */
        static Ptr instance() {
            return Ptr(new StateNormalizer);
        }

        /** Property: ignore recently popped memory.
         *
         *  If the stack is well behaved (they usually are) then ignore memory that is off the top of the stack because it's
         *  been recently popped.  The value if this property is the number of bytes beyond the top of the stack that are
         *  considered to be recently popped.
         *
         * @{ */
        size_t ignorePoppedMemory() const { return ignorePoppedMemory_; }
        void ignorePoppedMemory(size_t nbytes) { ignorePoppedMemory_ = nbytes; }
        /** @} */

        /** Constructs an initial state. */
        virtual InstructionSemantics2::BaseSemantics::StatePtr
        initialState(const InstructionSemantics2::BaseSemantics::DispatcherPtr&, SgAsmInstruction *firstInsn);

        /** Takes a state and returns a normalized string.
         *
         *  States are considered equal if their strings are equal. */
        virtual std::string toString(const InstructionSemantics2::BaseSemantics::DispatcherPtr&,
                                     const InstructionSemantics2::BaseSemantics::StatePtr&);
    };

private:
    InstructionSemantics2::BaseSemantics::DispatcherPtr cpu_;
    StateNormalizer::Ptr normalizer_;
    Sawyer::Optional<rose_addr_t> initialSp_;
    bool ignoreTerminalBranches_;

public:
    static Sawyer::Message::Facility mlog;              /**< Diagnostic streams. */

public:
    /** Default constructor.
     *
     *  Since this default constructor has no information about the virtual CPU, it will assume that all instructions have an
     *  effect. */
    NoOperation()
        : ignoreTerminalBranches_(true) {}

    /** Construct a new analysis with specified virtual CPU. */
    explicit NoOperation(const InstructionSemantics2::BaseSemantics::DispatcherPtr &cpu)
        : cpu_(cpu), normalizer_(StateNormalizer::instance()), ignoreTerminalBranches_(true) {}

    /** Construct a new analysis for a specific disassembler.
     *
     *  An analysis constructed this way will use the symbolic semantics domain. */
    explicit NoOperation(BinaryAnalysis::Disassembler*);

    /** Property: state normalizer.
     *
     *  The state normalizer is responsible for normalizing a virtual machine state and turning it into a string. When looking
     *  for no-ops, if two state strings compare equal then the instruction(s) that transitioned the machine from one state to
     *  the other are effectively a no-op.  In particular, the state normalizer should probably not try to compare instruction
     *  pointer registers, or memory that was read without being written (i.e., memory that sprang into existence by reading).
     *  The default normalizer does both of these things if the register state is derived from @ref
     *  InstructionSemantics2::BaseSemantics::RegisterStateGeneric and the memory state is derived from @ref
     *  InstructionSemantics2::BaseSemantics::MemoryCellList.
     *
     * @{ */
    StateNormalizer::Ptr stateNormalizer() const { return normalizer_; }
    void stateNormalizer(const StateNormalizer::Ptr &f) { normalizer_ = f; }
    /** @} */

    /** Property: initial concrete value for stack pointer.
     *
     *  A concrete initial value for the stack pointer can be used to help decide whether memory addresses are recently
     *  popped. It may be possible to do this without a concrete value also, depending on the semantic domain.
     *
     * @{ */
    const Sawyer::Optional<rose_addr_t> initialStackPointer() const { return initialSp_; }
    void initialStackPointer(const Sawyer::Optional<rose_addr_t> &v) { initialSp_ = v; }
    /** @} */

    /** Property: Whether terminal branches can be no-ops.
     *
     *  If set (the default) then branch instructions that appear as the last instruction of a basic block, and which have one
     *  constant, known successor which is not the fall-through address are not considered to be part of any no-op sequence.
     *
     * @{ */
    bool ignoreTerminalBranches() const { return ignoreTerminalBranches_; }
    void ignoreTerminalBranches(bool b) { ignoreTerminalBranches_ = b; }
    /** @} */

    /** Determines if an instruction is a no-op. */
    bool isNoop(SgAsmInstruction*) const;

    /** Determines if a sequence of instructions is a no-op. */
    bool isNoop(const std::vector<SgAsmInstruction*>&) const;

    /** Finds all sequences of instructions that are equivalent to no-operation. */
    IndexIntervals findNoopSubsequences(const std::vector<SgAsmInstruction*>&) const;

    /** Select certain no-op sequences.
     *
     *  Given a list of no-op sequences, such as returned by @ref findNoopSubsequences, process the list so that the largest
     *  non-overlapping sequences are returned.  If sequence A is larger than sequence B, then B is discarded.  If A and B are
     *  the same size and overlap and A starts before B then B is discarded.
     *
     *  The return value is sorted by decreasing size and contains non-overlapping intervals. */
    static IndexIntervals largestEarliestNonOverlapping(const IndexIntervals&);

    /** Return a boolean vector.
     *
     *  Returns a vector with one element per instruction. The element is true if the instruction is part of one of the
     *  specified index intervals. The returned vector will contain at least @p size elements. */
    static std::vector<bool> toVector(const IndexIntervals&, size_t size=0);

    /** Initializes and registers disassembler diagnostic streams. See Diagnostics::initialize(). */
    static void initDiagnostics();

protected:
    InstructionSemantics2::BaseSemantics::StatePtr initialState(SgAsmInstruction *firstInsn) const;
    std::string normalizeState(const InstructionSemantics2::BaseSemantics::StatePtr&) const;
};

} // namespace
} // namespace

#endif
#endif
