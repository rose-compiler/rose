#ifndef ROSE_Partitioner2_BasicTypes_H
#define ROSE_Partitioner2_BasicTypes_H

// Define this if you want extra invariant checks that are quite expensive. This only makes a difference if NDEBUG and
// SAWYER_NDEBUG are both undefined--if either one of them are defined then no expensive (or inexpensive) checks are
// performed.
//#define ROSE_PARTITIONER_EXPENSIVE_CHECKS

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

/** Partitioner control flow vertex types. */
enum VertexType {
    V_BASIC_BLOCK,                                      /**< A basic block or placeholder for a basic block. */
    V_UNDISCOVERED,                                     /**< The special "undiscovered" vertex. */
    V_INDETERMINATE,                                    /**< Special vertex destination for indeterminate edges. */
    V_NONEXISTING,                                      /**< Special vertex destination for non-existing basic blocks. */
};

/** Partitioner control flow edge types. */
enum EdgeType {
    E_NORMAL,                                           /**< Normal control flow edge, nothing special. */
    E_FUNCTION_CALL,                                    /**< Edge is a function call. */
    E_FUNCTION_RETURN,                                  /**< Edge is a function return. Such edges represent the actual
                                                         *   return-to-caller and usually originate from a return instruction
                                                         *   (e.g., x86 @c RET, m68k @c RTS, etc.). */
    E_CALL_RETURN,                                      /**< Edge is a function return from the call site. Such edges are from
                                                         *   a caller basic block to (probably) the fall-through address of the
                                                         *   call and don't actually exist directly in the specimen.  The
                                                         *   represent the fact that the called function eventually returns
                                                         *   even if the instructions for the called function are not available
                                                         *   to analyze. */
    E_FUNCTION_XFER,                                    /**< Edge is a function call transfer. A function call transfer is
                                                         *   similar to @ref E_FUNCTION_CALL except the entire call frame is
                                                         *   transferred to the target function and this function is no longer
                                                         *   considered part of the call stack; a return from the target
                                                         *   function will skip over this function. Function call transfers
                                                         *   most often occur as the edge leaving a thunk. */
};

class Partitioner;

/** Stack of distinct items.
 *
 *  Last-in-first-out list, or stack, of items where each item is not equal to any other item in the stack.  The item type, @p
 *  T, must satisfy requirements for being a member of an STL @p set. */
template<class T>
class DistinctStack {
public:
    typedef T Value;                                    /**< Type of value stored by this container. */
private:
    typedef std::list<Value> Stack;
    typedef typename Stack::iterator Position;
    typedef std::map<Value, Position> Presence;
    Stack stack_;
    Presence presence_;
public:
    /** Determins if this container is empty. */
    bool isEmpty() const {
        return stack_.empty();
    }

    /** Returns number of items in list. */
    size_t size() const {
        return presence_.size();                        // map.size is O(1), but list.size is O(n)
    }

    /** Returns true if item is present. */
    bool exists(const Value &item) const {
        return presence_.find(item) != presence_.end();
    }

    /** Push item onto stack if it isn't present. */
    void pushMaybe(const Value &item) {
        typename Presence::iterator found = presence_.find(item);
        if (found == presence_.end()) {
            stack_.push_back(item);
            presence_[item] = --stack_.end();
        } else {
            stack_.erase(found->second);
            stack_.push_back(item);
            found->second = --stack_.end();
        }
    }

    /** Pop and return most recent item added. */
    Value pop() {
        ASSERT_forbid(isEmpty());
        Value item = stack_.back();
        stack_.pop_back();
        presence_.erase(item);
        return item;
    }

    /** Erase item if it exists. */
    void erase(const Value &item) {
        typename Presence::iterator found = presence_.find(item);
        if (found != presence_.end()) {
            stack_.erase(found->second);
            presence_.erase(found);
        }
    }
};

} // namespace
} // namespace
} // namespace

#endif
