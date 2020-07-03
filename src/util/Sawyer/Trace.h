// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




#ifndef Sawyer_Trace_H
#define Sawyer_Trace_H

#include <Sawyer/Interval.h>
#include <Sawyer/Map.h>
#include <Sawyer/Optional.h>
#include <Sawyer/Sawyer.h>
#include <Sawyer/Set.h>
#include <boost/foreach.hpp>
#include <boost/iterator.hpp>
#include <set>
#include <vector>

namespace Sawyer {
namespace Container {

/** Tag for a map-based @ref Trace label index. */
class TraceMapIndexTag {};

/** Tag for a vector-based @ref Trace label index. */
class TraceVectorIndexTag {};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Implementation Details
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace TraceDetail {

// Internal: An index where labels are keys for a tree-based map.
template<class K, class V>
class MapIndex {
    typedef K Label;
    typedef V Value;

    Sawyer::Container::Map<Label, Value> map_;

public:
    // Reset index to initial state
    void clear() {
        map_.clear();
    }

    // Return all labels, and possibly more (although this implementation is exact).  If any extra labels are returned, their
    // values are the default constructed.
    boost::iterator_range<typename Sawyer::Container::Map<Label, Value>::ConstKeyIterator> labels() const {
        return map_.keys();
    }

    // Return the value stored for a label. If the label has no value then return a default constructed value and do not modify
    // the index.
    const Value& operator[](const Label &label) const {
        return map_.getOrDefault(label);
    }

    // Return the value stored for a label. If the label has no value then first insert a default constructed value.
    Value& operator[](const Label &label) {
        return map_.insertMaybeDefault(label);
    }

    // If the index supports it, reserve space for the specified number of labels.
    void reserve(size_t /*n*/) {}
};

// Internal: An index where labels are indexes into a vector.  For member documentation see TraceMapIndex
template<class K, class V>
class VectorIndex {
    typedef K Label;
    typedef V Value;

    const Value dflt_;
    std::vector<Value> vector_;

public:
    VectorIndex() {}

    void clear() {
        vector_.clear(0);
    }

    boost::iterator_range<typename Sawyer::Container::Interval<Label>::ConstIterator> labels() const {
        return Sawyer::Container::Interval<Label>::baseSize(0, vector_.size()).values();
    }

    const Value& operator[](Label label) const {
        return (size_t)label < vector_.size() ? vector_[label] : dflt_;
    }

    Value& operator[](Label label) {
        if ((size_t)label >= vector_.size())
            vector_.resize(label+1, dflt_);
        return vector_[label];
    }

    void reserve(size_t n) {
        vector_.reserve(n);
    }
};

} // namespace


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Traits
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Traits for a Trace label index.
 *
 *  The @p IndexTypeTag argument is used to select the index implementation used to associate some data with each
 *  label. %Sawyer defines two kinds of indexes: the @ref TraceMapIndexTag selects an index based on @c std::map while the @ref
 *  TraceVectorIndexTag selects an index based on @c std::vector. The vector-based index is suitable for labels that can be
 *  used as vector indexes. The map-based index should be used for all other cases, including integer labels that would result
 *  in very sparse vectors.
 *
 *  Users are welcome to define their own index class. Their class should have the following interface:
 *
 *  @li The index must be able to store labels of type @p Label. Each label will have a default constructor, a copy
 *      constructor, an assignment operator, and be comparable as equal or not-equal. Additional requirements can be imposed by
 *      the index.
 *  @li The index needs to be able to store arbitrary data of type @p Value per label. These values will always have a default
 *      constructor, a copy constructor, and an assignment operator. The index cannot impose additional requirements.
 *  @li The index must have a @c clear method that takes no arguments and resets the index to its default constructed state.
 *  @li The index must have two array operators whose argument is a @p Label and whose signature differes only in whether the
 *      @p this pointer is const. Both return a reference to either the @p Value stored for that label or a default-constructed
 *      @p Value. The operator that takes a const @p this pointer should return a const reference, and the value that takes a
 *      non-const @p this pointer must return a non-const reference. Furthermore, the non-const version must first insert a
 *      default-constructed value if no value was previously stored for the specified label.
 *  @li The index must be able to return a @c boost::iterator_range pair of iterators that will index over its labels.  These
 *      range is permitted to iterate over labels that are not part of a trace, as long as the iteration also visits each
 *      defined label exactly once.  Dereferencing an non-end iterator must result in a valid @c Label object (or const
 *      reference to such).
 *  @li The index must define a @c reserve method that takes a @c size_t argument. Its purpose is to give the index a chance
 *      to pre-allocate space. The argument is only a hint for the number of distinct labels that the trace might eventually
 *      contain.
 *
 *  To use a user-defined index, the user should define their index class, define a tag for their class, and specialize @c
 *  TraceIndexTraits.  Here's a simple example where the user has a custom index for a specific user-defined label type:
 *
 *  @code
 *  class MyLabel {...};
 *
 *  template<class Value>
 *  class MyLabelIndex; // see requirements above
 *
 *  class MyLabelIndexTag {};
 *
 *  namespace Sawyer { namespace Container {
 *      template<class Value>
 *      struct TraceIndexTraits<MyLabel, Value, MyLabelIndexTag> {
 *          typedef MyIndex<Value> Index;
 *      };
 *  }}
 *  @endcode
 *
 *  Once you've defined your index, you can start to use it like this:
 *
 *  @code
 *  Sawyer::Container::Trace<MyLabel, MyLabelIndexTag> trace;
 *  @endcode
 */
template<class Label, class Value, class IndexTypeTag>
struct TraceIndexTraits {
    typedef TraceDetail::MapIndex<Label, Value> Index;
};

template<class Label, class Value>
struct TraceIndexTraits<Label, Value, TraceVectorIndexTag> {
    typedef TraceDetail::VectorIndex<Label, Value> Index;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Trace
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Records and replays traces.
 *
 *  This class is able to record a trace and replay it later. A trace is an ordered list of labels of type @p T, where a label
 *  can be any type whose members have an ordering relationship (integers, strings, pointers, certain kinds of iterators,
 *  etc.). Some examples where a @ref Trace could be used:
 *
 *  @li To record a traversal through a tree-like data structure where the nodes of the tree have string labels.
 *  @li To record a traversal through a @ref Sawyer::Container::Graph whose vertices are numbered.
 *  @li To record the states executed by a DFA
 *
 *  Of course, a container traversal or DFA execution could also be replayed by re-traversing or re-executing, but that's not
 *  always convenient due to data modification during the traversal, the size of the associated data, or the runtime
 *  expense. Furthermore, a @ref Trace can also answer queries like how often a label was visited, what transitions are most
 *  common, and so on.
 *
 *  Although a trace can be stored simply as an ordered list of labels, doing so is not always the most efficient. A @ref
 *  Trace compresses a trace by observing that many kinds of traces tend to be "bursty". That is, the successor of each
 *  visit to label @em A is likely to be the same as the successor of the previous visit to @em A. See @ref burstiness.  The
 *  higher the burstiness the more compression is possible.
 *
 *  In order to compress a trace and to later obtain information on a per-label basis, the @ref Trace needs to be able to form
 *  an index of the labels. The second template argument, @p IndexTag, is a tag that selects an index type from the @ref
 *  TraceIndexTraits.  %Sawyer provides the following tags, but users can create additional tags and their own index
 *  types. @ref TraceMapIndexTag is the default and uses an @c std::map to store labels; @ref TraceVectorIndexTag causes an @c
 *  std::vector to be used instead and is suitable for a dense set of low-numbered labels.
 *
 *  @section Sawyer_Trace_Example1 Example: recording a program execution trace for a control flow graph
 *
 *  This example records a program execution trace (ET) over a control flow graph (CFG). An ET is the ordered list of machine
 *  instruction addresses executed by a program when it runs, and a CFG is a graph whose vertices are all known instruction
 *  addresses and whose edges represent possible execution flow from one instruction to another.  Non-branching instructions
 *  have only one outgoing edge, and most branches have two outgoing edges (indirect branches have more). For simplicity, we
 *  assume that the CFG is complete and is computed prior to obtaining the trace.  The important thing to know is that
 *  execution traces tend to be bursty: non-branching instructions always have the same successor, and branching instructions
 *  (especially those controlling loops) tend to branch one way more often than the other.
 *
 *  @code
 *  using namespace Sawyer;
 *  using namespace Sawyer::Container;
 *
 *  class Instruction { ... } // some user-defined type
 *  class CfgEdge { ... } // some user-defined type
 *  class Program { ... } // some user-define type
 *  Program program("a.out"); // user-defined: starts a program in a debugger
 *
 *  typedef Graph<Instruction, CfgEdge, unsigned> Cfg; // instructions indexed by their unsigned address
 *  typedef Trace<size_t, TraceVectorIndexTag> Trace; // labels are sequential CFG vertex IDs, so a vector is appropriate
 *
 *  Cfg cfg;
 *  Trace trace;
 *  while (program.isRunning()) {
 *      unsigned address = program.executionAddress();
 *      Cfg::ConstVertexIterator vertex = cfg.findVertexKey(address); // O(log) or O(1) depending on index type
 *      trace.append(vertex->id()); // O(1)
 *      program.singleStep();
 *  }
 *  @endcode
 *
 *  Later, we can replay the trace without the expense of running the program again:
 *
 *  @code
 *  trace.traverse([&cfg](size_t vertexId) {
 *      Cfg::ConstVertexIterator vertex = cfg.findVertex(vertexId); // O(1)
 *      const Instruction &insn = vertex->value();
 *      std::cout <<"executed " <<insn.toString() <<"\n";
 *      return true; // continue traversal
 *  });
 *  @endcode
 *
 *  If we know the address of a branch instruction (such as for an "if" statement in a higher level language), we can query
 *  some properties such as burstiness to decide whether speculative execution would be beneficial here:
 *
 *  @code
 *  unsigned branchAddress = ...;
 *  if (trace.exists(branchAddress)) { // branch was executed at least once?
 *      if (trace.burstiness(branchAddress) >= 2.0) {
 *          // loosely speaking, the branch predicate is not random; 2.0 is arbitrary
 *      }
 *  }
 *  @endcode */
template<class T, class IndexTag = TraceMapIndexTag>
class Trace {
private:
    struct Decompression {
        size_t n;                                       // label visitation sequence number (starts at zero for each label)
        size_t succIdx;                                 // index into a Successors list;

        Decompression(): n(0), succIdx(0) {}
    };

public:
    /** Label type.
     *
     *  Labels must have an ordering defined by a less-than operator. */
    typedef T Label;

    /** Compressed next-label list.
     *
     *  See @ref successors. */
    struct Successor {
        size_t end;                                     /**< Label visitation sequence number. Starts at zero for each label. */
        Label next;                                     /**< Next label. */

        Successor(): end(0) {}
        Successor(size_t end, const Label &next): end(end), next(next) {}
    };

    /** Successors for a label. */
    typedef std::vector<Successor> Successors;

    /** Forward iterator.
     *
     *  This iterator traverses the elements of the trace in the order they were inserted, returning a label at each step. */
    class ConstIterator: public boost::iterator_facade<ConstIterator, const Label, boost::forward_traversal_tag, Label> {
        friend class boost::iterator_core_access;
        const Trace *trace_;
        Sawyer::Optional<Label> label_;
        size_t position_;
        typename TraceIndexTraits<Label, Trace::Decompression, IndexTag>::Index decompressionState_;

    public:
        /** Construct iterator point to end of any trace. */
        ConstIterator()
            : trace_(NULL), position_(0) {}

        /** Construct iterator pointing to first element of the trace, if any. */
        explicit ConstIterator(const Trace &trace)
            : trace_(&trace), position_(0) {
            if (!trace_->isEmpty())
                label_ = trace_->front();
        }

        /** Copy constructor. */
        ConstIterator(const ConstIterator &other)
            : trace_(other.trace_), label_(other.label_), position_(other.position_),
              decompressionState_(other.decompressionState_) {}

        /** Test whether iterator is at the end. */
        bool isEnd() const {
            return !label_;
        }

        /** Position of iterator within trace.
         *
         *  The position starts at zero and is incremented each time this iterator is incremented. */
        size_t position() const {
            return position_;
        }

    private:
        // core operation from iterator_facade needed for readable iterator concept
        Label dereference() const {
            ASSERT_forbid(isEnd());
            return *label_;
        }

        // core operation from iterator_facade needed for single pass iterator concept
        bool equal(const ConstIterator &other) const {
            if (isEnd() || other.isEnd())
                return isEnd() == other.isEnd();
            return trace_ == other.trace_ && position() == other.position();
        }
            
        // core operation from iterator_facade needed for incrementable iterator concept
        void increment() {
            ASSERT_forbid(isEnd());
            const Successors &successors = trace_->index_[*label_];
            Decompression &dcomp = decompressionState_[*label_];
            if (dcomp.succIdx >= successors.size()) {
                label_ = Nothing();
            } else {
                const Successor &successor = successors[dcomp.succIdx];
                if (dcomp.n < successor.end) {
                    label_ = successor.next;
                    ++dcomp.n;
                } else {
                    ++dcomp.succIdx;
                    if (dcomp.succIdx >= successors.size()) {
                        label_ = Nothing();
                    } else {
                        label_ = successors[dcomp.succIdx].next;
                        ++dcomp.n;
                    }
                }
            }
            ++position_;
        }
    };
    
private:
    typedef typename TraceIndexTraits<Label, Successors, IndexTag>::Index Index;
    Index index_;                                       // encoded sequence of labels
    size_t size_;                                       // total length of sequence
    size_t nLabels_;                                    // number of distinct labels in sequence
    Optional<Label> front_, back_;                      // first and last labels in the sequence if size_ > 0

public:
    /** Default constructor. */
    Trace(): size_(0), nLabels_(0) {}

    /** Returns a forward iterator pointing to first element of this trace. */
    ConstIterator begin() const {
        return ConstIterator(*this);
    }

    /** Returns a forward iterator pointing past the end of this trace. */
    ConstIterator end() const {
        return ConstIterator();
    }

    /** Returns the first item in the trace.
     *
     *  The trace must not be empty.  Returns a copy of the item. */
    Label front() const {
        return *front_;
    }

    /** Returns the last item in the trace.
     *
     *  The trace must not be empty. Returns a copy of the item. */
    Label back() const {
        return *back_;
    }
    
    /** Clears the recorded trace.
     *
     *  This @ref Trace is reset to its default-constructed state. */
    void clear() {
        index_.clear();
        size_ = nLabels_ = 0;
        front_ = back_ = Nothing();
    }

    /** Reserve space in the label index.
     *
     *  This is a hint to reserve space for @p n distinct labels in the label index. The index need not honor this request. */
    void reserve(size_t n) {
        index_.reserve(n);
    }

    /** Determines if a trace is empty.
     *
     *  Returns true if and only if a trace contains no labels.
     *
     *  Time complexity: constant. */
    bool isEmpty() const {
        return front_ ? false : true;
    }

    /** Determines if a label is present in a trace.
     *
     *  Returns true if and only if the specified label occurs in the trace.
     *
     *  Time complexity: depends on the label indexing scheme, but probably constant for labels that can be used as array
     *  indexes and logorithmic for labels that are map lookup keys. */
    bool exists(const Label &label) const {
        return isEmpty() ? false : (*front_ == label || *back_ == label || !index_[label].empty());
    }

    /** Total length of a trace, or the number of times a label appears in a trace.
     *
     *  Time complexity: constant.
     *
     * @{ */
    size_t size() const {
        return size_;
    }
    size_t size(const Label &label) const {
        const Successors &successors = index_[label];
        return successors.empty() ? 0 : successors.back().end;
    }
    /** @} */

    /** Number of distinct labels.
     *
     *  This is the number of unique labels that appear in the trace.
     *
     *  Time complexity: constant. */
    size_t nLabels() const {
        return nLabels_;
    }

    /** Set of all labels in the trace.
     *
     *  This is the set of labels in no particular order. */
    Sawyer::Container::Set<Label> labels() const {
        Sawyer::Container::Set<Label> retval;
        BOOST_FOREACH (const Label &label, index_.labels())
            retval.insert(label);
        return retval;
    }

    /** Append a label to a trace.
     *
     *  Time complexity: amortized constant if labels are array indexes, amortized logirithmic if labels are map keys. */
    void append(const Label &label) {
        if (isEmpty()) {
            front_ = label;
            ++nLabels_;
        } else {
            ASSERT_require(front_);
            if (!exists(label))
                ++nLabels_;
            Successors &successors = index_[*back_];
            if (successors.empty()) {
                successors.push_back(Successor(1, label));
            } else if (successors.back().next == label) {
                ++successors.back().end;
            } else {
                successors.push_back(Successor(successors.back().end+1, label));
            }
        }
        back_ = label;
        ++size_;
    }

    /** Traversal of the trace labels.
     *
     *  The @p visitor functor takes one argument: the label being visited, and should return true if the traversal should
     *  continue or false if it should be terminated.
     *
     *  Time complexity: initialization time is proportional to the number of distinct labels in the trace. Advancing from one
     *  label to the next is constant time. */
    template<class Visitor>
    void traverse(Visitor &visitor) const {
        if (isEmpty())
            return;
        Label label = *front_;
        typename TraceIndexTraits<Label, Decompression, IndexTag>::Index decompressionState;
        while (1) {
            if (!visitor(label))
                return;
            const Successors &successors = index_[label];
            Decompression &dcomp = decompressionState[label];
            if (dcomp.succIdx >= successors.size()) {     // FIXME[Robb Matzke 2016-12-08]: simplify these nested "if"s
                return;
            } else {
                const Successor &successor = successors[dcomp.succIdx];
                if (dcomp.n < successor.end) {
                    label = successor.next;
                    ++dcomp.n;
                } else {
                    ++dcomp.succIdx;
                    if (dcomp.succIdx >= successors.size())
                        return;
                    label = successors[dcomp.succIdx].next;
                    ++dcomp.n;
                }
            }
        }
    }

private:
    // helper for the "print" method
    struct PrintHelper {
        std::ostream &out;
        const std::string &separator;
        size_t nPrinted;

        PrintHelper(std::ostream &out, const std::string &separator)
            : out(out), separator(separator), nPrinted(0) {}

        bool operator()(const Label &label) {
            if (1 != ++nPrinted)
                out <<separator;
            out <<label;
            return true;
        }
    };

public:
    /** Print as sequence.
     *
     *  Emits the trace to the specified output stream with each element separated by the @p separator. */
    void print(std::ostream &out, const std::string &separator = ", ") const {
        PrintHelper visitor(out, separator);
        traverse(visitor);
    }

    /** Low-level debugging information.
     *
     *  This method is intended for debugging. It prints the storage representation of this trace. The output format is not
     *  defined. */
    void dump(std::ostream &out) const {
        if (isEmpty()) {
            out <<"Trace(empty)";
        } else {
            out <<"Trace(size=" <<size_ <<", unique=" <<nLabels_ <<", front=" <<*front_ <<", back=" <<*back_;
            BOOST_FOREACH (const Label &label, index_.labels()) {
                const Successors &successors = index_[label];
                if (!successors.empty()) {
                    out <<"\n  " <<label <<" => [";
                    BOOST_FOREACH (const Successor &successor, successors)
                        out <<"\n    end index=" <<successor.end <<", next label=" <<successor.next;
                    out <<"]\n";
                }
            }
            out <<")";
        }
    }

private:
    // helper for the "toVector" method
    struct ToVector {
        std::vector<Label> vector;
        bool operator()(const Label &label) {
            vector.push_back(label);
            return true;
        }
    };

public:
    /** Convert a trace into a vector.
     *
     *  Converts a trace into a vector of labels. Consider using @ref traverse instead since it's more efficient. */
    std::vector<Label> toVector() const {
        ToVector visitor;
        traverse(visitor);
        return visitor.vector;
    }

    /** %Set of labels which are successors for the specified label.
     *
     *  Time complexity: The set must be constructed by traversing successor sequence. The length of the successor sequence is
     *  the number of maximal subsequences where the members of a subsequence are all the same. Insertion of each label into
     *  the set has logarithmic time. */
    std::set<Label> successorSet(const Label &label) const {
        std::set<Label> unique;
        BOOST_FOREACH (const Successor &successor, index_[label])
            unique.insert(successor.next);
        return unique;
    }

    /** The burstiness of a label.
     *
     *  Loosely speaking, the "burstiness" of a label is a measure of how often successive occurrences of the label transition
     *  to the same successor and is a floating point value between zero and one, inclusive. Specifically, the burstiness of a
     *  label is the number of distinct labels divided by the number of maximal subsequences of the successor sequence where a
     *  subsequence's members are all the same label.
     *
     *  For example, if the the entire trace is [5, 5, 5, 4, 5, 5] then the successors of label 5 are the sequence [5, 5, 4, 5]
     *  and the maximal subsequences of one label are [5, 5], [4], and [5].  The burstiness of 5 is therefore 2/3 since there
     *  are two distinct labels, {4, 5}, and three maximal successor subsequences.
     *
     *  Some properties: A label with only one distinct successor, will have a burstiness of 1.0 no matter how many
     *  times the label appears in the trace.  A label with N distinct successors will have a burstiness of 1.0 if it visits
     *  the first distinct successor, then the second, then the third (in any order) without any interleaving. A label that
     *  interleaves its successors as much as possible will have a burstiness of approximately 1/N where N is the number of
     *  distinct successors.
     *
     *  If label has no successors then zero is returned. This is distinguishable from other cases since normal return values
     *  will always be positive.
     *
     *  Time complexity: O(n) where n is the number of maximal subsequences of the successors where all members of a
     *  subsequence are equal. Time to look up the label depends on the label index type.
     *
     *  @{ */
    double burstiness(const Label &label) const {
        if (size_t nUniqueLabels = successorSet(label).size())
            return (double)nUniqueLabels / index_[label].size();
        return 0.0;
    }
    /** @} */

    /** The burstiness of a trace.
     *
     *  The burstiness of a trace is the average burstiness of its labels. */
    double burstiness() const {
        size_t size = 0;
        double sum = 0.0;
        BOOST_FOREACH (const Label &label, index_.labels()) {
            if (double x = burstiness(label)) {
                sum += x;
                ++size;
            }
        }
        return size ? sum / size : 0.0;
    }

    /** Ordered successors for a label.
     *
     *  Given a label, return a vector that describes the ordered successors for this label. The return value is a list whose
     *  members are pairs consisting of an integer visit sequence number and a label.  For instance, if label A was visited
     *  10 times and the successors were, in order, [B, B, B, B, B, C, C, B, B, D] then the return value will be the list of
     *  pairs [(5,B), (7,C), (9,B), (10,D)].
     *
     *  Time complexity: Depends on the label index type. If the @ref TraceVectorIndexTag was specified then this method
     *  executes in constant time. */
    const Successors& successors(const Label &label) const {
        return index_[label];
    }
};

/** Emit the ordered labels for a trace.
 *
 *  Prints a trace by emitting a comma-separated list of the trace's labels using @ref Trace::print. */
template<class T, class IndexTag>
inline std::ostream&
operator<<(std::ostream &out, const Trace<T, IndexTag> &trace) {
    trace.print(out);
    return out;
}

} // namespace
} // namespace

#endif
