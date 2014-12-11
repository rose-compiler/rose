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
bool sortFunctionNodesByAddress(const SgAsmFunction*, const SgAsmFunction*);
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

/** Parse an address interval.
 *
 *  An interval is specified in one of the following forms, where N and M are non-negative integers:
 *
 * @li "N" is a singleton interval.
 * @li "N,M" is the min/max form specifying the two inclusive end points. M must not be less than N.
 * @li "N-M" is the begin/end form where M is greater than N and not included in the interval.
 * @li "N+M" is the base/size form where N is the minimum value and M is the number of items.
 *
 * The integers can be specified in decimal, octal, hexadecimal, or binary using the usual C/C++ syntax. */
class AddressIntervalParser: public Sawyer::CommandLine::ValueParser {
protected:
    AddressIntervalParser() {}
    AddressIntervalParser(const Sawyer::CommandLine::ValueSaver::Ptr &valueSaver)
        : Sawyer::CommandLine::ValueParser(valueSaver) {}
public:
    typedef Sawyer::SharedPointer<AddressIntervalParser> Ptr;
    static Ptr instance() {
        return Ptr(new AddressIntervalParser);
    }
    static Ptr instance(const Sawyer::CommandLine::ValueSaver::Ptr &valueSaver) {
        return Ptr(new AddressIntervalParser(valueSaver));
    }
    static std::string docString();
private:
    virtual Sawyer::CommandLine::ParsedValue operator()(const char *input, const char **rest,
                                                        const Sawyer::CommandLine::Location &loc) ROSE_OVERRIDE;
};

AddressIntervalParser::Ptr addressIntervalParser(AddressInterval &storage);
AddressIntervalParser::Ptr addressIntervalParser();

/** Trigger based on number of times called. */
class Trigger {
public:
    typedef AddressInterval SizeInterval;               // okay to use 64-bit integers for the counters
    struct Settings {
        SizeInterval when;                              // when to trigger based on nCalls_
        Settings(): when(0) {}
    };
private:
    size_t nCalls_;                                     // number of times called
    Settings settings_;
public:
    /** Trigger armed for single call. */
    Trigger(): nCalls_(0) {}

    /** Armed for triggering when number of calls falls within @p when. */
    explicit Trigger(const Settings &settings): nCalls_(0), settings_(settings) {}

    /** Armed for triggering after @p nSkip calls but not more than @p nTimes times. */
    Trigger(size_t nSkip, size_t nTimes): nCalls_(0) {
        settings_.when = nTimes ? SizeInterval::baseSize(nSkip, nTimes) : SizeInterval();
    }

    /** Armed for one call. */
    static Trigger once() { return Trigger(0, 1); }

    /** Armed to always trigger. */
    static Trigger always() { return Trigger(0, size_t(-1)); }

    /** Armed to never trigger. */
    static Trigger never() { return Trigger(); }

    /** True if trigger is armed. */
    bool isArmed() const { return !settings_.when.isEmpty() && nCalls_<=settings_.when.greatest(); }

    /** Increment calls and return true if triggering. */
    bool shouldTrigger() { return settings_.when.isContaining(nCalls_++); }

    /** Number of times called. */
    size_t nCalls() const { return nCalls_; }

    /** Reset number of calls to zero. */
    void reset() { nCalls_ = 0; }

    /** Command-line switches to initialize settings. */
    static Sawyer::CommandLine::SwitchGroup switches(Settings&);

    /** Documentation for command-line switches. */
    static std::string docString();
};

/** Return the next serial number. */
size_t serialNumber();



} // namespace
} // namespace
} // namespace

#endif
