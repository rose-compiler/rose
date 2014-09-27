#ifndef ROSE_Partitioner2_Utility_H
#define ROSE_Partitioner2_Utility_H

#include <Partitioner2/AddressUsageMap.h>
#include <Partitioner2/BasicBlock.h>
#include <Partitioner2/ControlFlowGraph.h>
#include <Partitioner2/DataBlock.h>
#include <Partitioner2/Function.h>

#include "Diagnostics.h"

#include <algorithm>
#include <ostream>

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

extern Sawyer::Message::Facility mlog;
void initDiagnostics();

bool sortBasicBlocksByAddress(const BasicBlock::Ptr&, const BasicBlock::Ptr&);
bool sortDataBlocks(const DataBlock::Ptr&, const DataBlock::Ptr&);
bool sortFunctionsByAddress(const Function::Ptr&, const Function::Ptr&);
bool sortByExpression(const BasicBlock::Successor&, const BasicBlock::Successor&);
bool sortVerticesByAddress(const ControlFlowGraph::ConstVertexNodeIterator&, const ControlFlowGraph::ConstVertexNodeIterator&);
bool sortEdgesBySrc(const ControlFlowGraph::ConstEdgeNodeIterator&, const ControlFlowGraph::ConstEdgeNodeIterator&);
bool sortEdgesByDst(const ControlFlowGraph::ConstEdgeNodeIterator&, const ControlFlowGraph::ConstEdgeNodeIterator&);
bool sortBlocksForAst(SgAsmBlock*, SgAsmBlock*);
bool sortInstructionsByAddress(SgAsmInstruction*, SgAsmInstruction*);

template<class Container, class Comparator>
static bool
isSorted(const Container &container, Comparator sorted, bool distinct=true) {
    typename Container::const_iterator current = container.begin();
    if (current!=container.end()) {
        typename Container::const_iterator next = current;
        while (++next != container.end()) {
            if ((distinct && !sorted(*current, *next)) || sorted(*next, *current))
                return false;
            ++current;
        }
    }
    return true;
}

template<class Container, class Value, class Comparator>
typename Container::const_iterator
lowerBound(const Container &container, const Value &item, Comparator cmp) {
    return std::lower_bound(container.begin(), container.end(), item, cmp);
}
template<class Container, class Value, class Comparator>
typename Container::iterator
lowerBound(Container &container, const Value &item, Comparator cmp) {
    return std::lower_bound(container.begin(), container.end(), item, cmp);
}

// Return true if two container elements are equal according to the sorting comparator
template<class Value, class Comparator>
bool
equalUnique(const Value &a, const Value &b, Comparator cmp) {
    return !cmp(a, b) && !cmp(b, a);                    // equal iff neither one is less than the other
}

// Insert an item into a sorted container if it doesn't exist yet in the container.  Returns true iff inserted.
template<class Container, class Value, class Comparator>
bool
insertUnique(Container &container, const Value &item, Comparator cmp) {
    ASSERT_not_null(item);
    ASSERT_require(isSorted(container, cmp, true));     // unique, sorted items
    typename Container::iterator lb = lowerBound(container, item, cmp);
    if (lb==container.end() || !equalUnique(*lb, item, cmp)) {
        container.insert(lb, item);
        ASSERT_require(isSorted(container, cmp, true));
        return true;
    }
    return false;
}

// Erase an item from a sorted container if it doesn't exist yet in the container.  Returns true iff erased.
template<class Container, class Value, class Comparator>
bool
eraseUnique(Container &container, const Value &item, Comparator cmp) {
    ASSERT_not_null(item);
    ASSERT_require(isSorted(container, cmp, true));     // unique, sorted items
    typename Container::iterator lb = lowerBound(container, item, cmp);
    if (lb!=container.end() && equalUnique(*lb, item, cmp)) {
        container.erase(lb);
        return true;
    }
    return false;
}

// Check existence of an item in a sorted container. Returns true iff the item exists.
template<class Container, class Value, class Comparator>
bool
existsUnique(const Container &container, const Value &item, Comparator cmp) {
    if (item==NULL)
        return false;
    ASSERT_require(isSorted(container, cmp, true));     // unique, sorted items
    typename Container::const_iterator lb = lowerBound(container, item, cmp);
    if (lb==container.end() || cmp(*lb, item) || cmp(item, *lb))
        return false;
    return true;
}

std::ostream& operator<<(std::ostream&, const AddressUser&);
std::ostream& operator<<(std::ostream&, const AddressUsers&);
std::ostream& operator<<(std::ostream&, const AddressUsageMap&);
std::ostream& operator<<(std::ostream&, const ControlFlowGraph::VertexNode&);
std::ostream& operator<<(std::ostream&, const ControlFlowGraph::EdgeNode&);


/** Trigger based on number of times called. */
class Trigger {
public:
    typedef Sawyer::Container::Interval<size_t> SizeInterval;
private:
    size_t nCalls_;                                     // number of times called
    SizeInterval when_;                                 // when to trigger based on nCalls_
public:
    /** Trigger not armed. */
    Trigger(): nCalls_(0) {}

    /** Armed for triggering when number of calls falls within @p when. */
    explicit Trigger(const SizeInterval &when): nCalls_(0), when_(when) {}

    /** Armed for triggering after @p nSkip calls but not more than @p nTimes times. */
    Trigger(size_t nSkip, size_t nTimes): nCalls_(0), when_(nTimes?SizeInterval::baseSize(nSkip, nTimes):SizeInterval()) {}

    /** Armed for one call. */
    static Trigger once() { return Trigger(0, 1); }

    /** Armed to always trigger. */
    static Trigger always() { return Trigger(0, size_t(-1)); }

    /** Armed to never trigger. */
    static Trigger never() { return Trigger(); }

    /** True if trigger is armed. */
    bool isArmed() const { return !when_.isEmpty() && nCalls_<=when_.greatest(); }

    /** Increment calls and return true if triggering. */
    bool shouldTrigger() { return when_.isContaining(nCalls_++); }

    /** Number of times called. */
    size_t nCalls() const { return nCalls_; }

    /** Reset number of calls to zero. */
    void reset() { nCalls_ = 0; }
};

/** Return the next serial number. */
size_t serialNumber();



} // namespace
} // namespace
} // namespace

#endif
