#ifndef ROSE_BinaryAnalysis_BinaryNoOperation_H
#define ROSE_BinaryAnalysis_BinaryNoOperation_H

#include <BaseSemantics2.h>

namespace rose {
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
        StateNormalizer() {}
    public:
        virtual ~StateNormalizer() {}

        /** Shared ownership pointer for state normalizer. */
        typedef Sawyer::SharedPointer<StateNormalizer> Ptr;

        /** Allocating constructor. */
        static Ptr instance() {
            return Ptr(new StateNormalizer);
        }

        /** Constructs an initial state. */
        virtual InstructionSemantics2::BaseSemantics::StatePtr
        initialState(const InstructionSemantics2::BaseSemantics::DispatcherPtr&);

        /** Takes a state and returns a normalized string.
         *
         *  States are considered equal if their strings are equal. */
        virtual std::string toString(const InstructionSemantics2::BaseSemantics::DispatcherPtr&,
                                     const InstructionSemantics2::BaseSemantics::StatePtr&);
    };

private:
    InstructionSemantics2::BaseSemantics::DispatcherPtr cpu_;
    StateNormalizer::Ptr normalizer_;

public:
    /** Default constructor.
     *
     *  Since this default constructor has no information about the virtual CPU, it will assume that all instructions have an
     *  effect. */
    NoOperation() {}

    /** Construct a new analysis with specified virtual CPU. */
    explicit NoOperation(const InstructionSemantics2::BaseSemantics::DispatcherPtr &cpu)
        : cpu_(cpu), normalizer_(StateNormalizer::instance()) {}

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
     *  BaseSemantics::RegisterStateGeneric and the memory state is derived from @ref BaseSemantics::MemoryCellList.
     *
     * @{ */
    StateNormalizer::Ptr stateNormalizer() const { return normalizer_; }
    void stateNormalizer(const StateNormalizer::Ptr &f) { normalizer_ = f; }
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

protected:
    InstructionSemantics2::BaseSemantics::StatePtr initialState() const;
    std::string normalizeState(const InstructionSemantics2::BaseSemantics::StatePtr&) const;
};

} // namespace
} // namespace

#endif
