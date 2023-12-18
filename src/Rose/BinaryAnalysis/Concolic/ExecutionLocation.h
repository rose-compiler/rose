#ifndef ROSE_BinaryAnalysis_Concolic_ExecutionLocation_H
#define ROSE_BinaryAnalysis_Concolic_ExecutionLocation_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <Rose/BinaryAnalysis/Concolic/BasicTypes.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

/** Location of an event.
 *
 *  An event location consists of a primary value and a secondary value, both of which are unsigned integers. Depending on the
 *  architecture, the primary value might be the number of instructions executed (i.e, the length of the current execution
 *  path). The secondary value is usually just a sequence number for ordering events that all occur at the same primary
 *  value. */
class ExecutionLocation {
private:
    uint64_t primary_ = 0;
    uint64_t secondary_ = 0;
    When when_ = When::POST;

public:
    ExecutionLocation() {}

    ExecutionLocation(uint64_t primary, uint64_t secondary, When when = When::PRE)
        : primary_(primary), secondary_(secondary), when_(when) {}

public:
    /** Property: Primary sequence number.
     *
     *  The primary sequence number is incremented per instruction executed. */
    uint64_t primary() const {
        return primary_;
    }

    /** Property: Secondary sequence number.
     *
     *  The secondary sequence number is reset to zero each time the primary sequence number is incremented. The secondary
     *  sequence number is used to order events within the execution of a single instruction, subject to the @ref when
     *  property. */
    uint64_t secondary() const {
        return secondary_;
    }

    /** Property: When the event is executed.
     *
     *  Specifies whether the event is to be replayed before or after the instruction that generated it. Those events that
     *  occur before are sorted by their @ref secondary property, and those that occur after are sorted by their @ref secondary
     *  property as well. See the @ref isSorted definition for details. */
    When when() const {
        return when_;
    }

    /** Execution event for next instruction. */
    ExecutionLocation nextPrimary() const;

    /** Next execution event for same instruction.
     *
     *  The event will be replayed before or after the instruction. */
    ExecutionLocation nextSecondary(When) const;

    /** String representation for printing. */
    std::string toString() const;

    /** Emit string representation to a stream. */
    void print(std::ostream&) const;

    /** Are two events sorted?
     *
     *  Returns true if and only if this location occurs before the @p other location in the sort order. */
    bool operator<(const ExecutionLocation &other) const;

    /** Are two events sorted?
     *
     *  Returns true if and only if location @p a occurs before @p b in sorted order. */
    static bool isSorted(const ExecutionLocation &a, const ExecutionLocation &b);
};

std::ostream& operator<<(std::ostream&, const ExecutionLocation&);

} // namespace
} // namespace
} // namespace

#endif
#endif
