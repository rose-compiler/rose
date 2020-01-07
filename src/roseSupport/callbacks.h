#ifndef ROSE_Callbacks_H
#define ROSE_Callbacks_H

#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>
#include <list>

namespace Rose {

/** User callbacks. See the @ref Rose::Callbacks::List class for details. */
namespace Callbacks {

/** Direction of callback list traversal. */
enum Direction {
    FORWARD,                /**< Traverse the list from front to back. */
    BACKWARD                /**< Traverse the list from back to front. */
};

/** List of callback functors.
 *
 *  This template defines ordered containers of callback functors and thread-safe functions for accessing the containers.
 *
 *  @section Rose_Callbacks_List_Example1 Example Usage
 *
 *  The first step is to create a functor class having one or more callbacks (operator() methods), each of which takes a
 *  struct reference containing the data to which the callback is applied.
 *
 *  @code
 *  class MyCallback: public MyCallbackBase { //base class optional
 *  public:
 *      MyCallback(): ncalls(0) {}
 *
 *      // Callback argument structure.  Data members can be anything
 *      // you want, and you can even have more than one of these
 *      // structs--one per callback signature.  The constructor
 *      // should generally just assign arguments to data members.
 *      struct Args {
 *          Args(SgAsmInstruction *insn, int callno, char *name):
 *              insn(insn), callno(callno), name(name) {}
 *          SgAsmInstruction *insn;
 *          int callno;
 *          char *name;
 *      };
 *
 *      // The callback.  You can overload these, but they'll all
 *      // take two arguments where the first is a Boolean and the
 *      // second is one of the argument types you defined above.
 *      bool operator()(bool b, const Args &args) {
 *          ncalls++;
 *          printf(stderr,"%s: %d\n", args.name, args.callno);
 *          insns.push_back(args.insn);
 *          return b;
 *      }
 *
 *      // Some data members accessible by the callback(s)
 *      size_t ncalls;
 *      static std::vector<SgAsmInstruction*> insns;
 *  };
 *  @endcode
 *
 *  The second step is to create a list to hold these callbacks. Our example callback was derived from a base class, so
 *  we'll use that base class as the list element type.
 *
 *  @code
 *  typedef Rose::Callbacks::List<MyCallbackBase> MyCallbackList;
 *  MyCallbackList cbl;
 *  @endcode
 *
 *  Add some callbacks to the list. We'll just add one for this example.
 *
 *  @code
 *  cbl.append(new MyCallback);
 *  @endcode
 *
 *  Finally, invoke all the callbacks in the list, supplying some arguments that will be used for each callback.
 *
 *  @code
 *  bool result = cbl.apply(true, MyCallback::Args(insn, 5, "foo"));
 *  @endcode
 */
template<class T>
class List {
public:
    typedef T CallbackType;                                 /**< Functor class. */
    typedef std::list<CallbackType*> CBList;                /**< Standard vector of functor pointers. */

private:
    mutable boost::mutex mutex_;
    CBList list;

public:
    List() {}

    List(const List &other) {
        boost::lock_guard<boost::mutex> lock(other.mutex_);
        list = other.list;
    }

    explicit List(CallbackType *callback) {
        append(callback);
    }

    /** Returns the number of callbacks in the list.
     *
     *  Thread safety: This method is thread safe. */
    size_t size() const {
        boost::lock_guard<boost::mutex> lock(mutex_);
        return list.size();
    }

    /** Predicate to test whether the list is empty.
     *
     *  Returns true if the list is empty, false otherwise.
     *
     *  Thread safety: This method is thread safe. */
    bool empty() const {
        boost::lock_guard<boost::mutex> lock(mutex_);
        return list.empty();
    }

    /** Append a functor to the end of the list without copying it.
     *
     *  Functors can be inserted more than once into a list, and each occurrence will be called by the apply() method.
     *
     *  Thread safety: This method is thread safe.  In fact, it is safe to modify the list while apply() is calling the
     *  functors on that list. */
    List& append(CallbackType *cb) {
        assert(cb!=NULL);
        boost::lock_guard<boost::mutex> lock(mutex_);
        list.push_back(cb);
        return *this;
    }

    /** Prepend callback to beginning of list without copying it.
     *
     *  Functors can be inserted more than once into a list, and each occurrence will be called by the apply() method.
     *
     *  Thread safety: This method is thread safe.  In fact, it is safe to modify the list while apply() is calling the
     *  functors on that list. */
    List& prepend(CallbackType *cb) {
        assert(cb!=NULL);
        boost::lock_guard<boost::mutex> lock(mutex_);
        list.push_front(cb);
        return *this;
    }

    /** Insert a callback after another.
     *
     *  The callback is inserted after each occurrence of the @p relative_to callback. Up to @p nreplacement insertions are
     *  made beginning at the front of the list.
     *
     *  Thread safety: This method is thread safe.  In fact, it is safe to modify the list while apply() is calling the
     *  functors on that list. */
    List& after(CallbackType *relative_to, CallbackType *cb, size_t nreplacements = UNLIMITED) {
        assert(cb!=NULL);
        boost::lock_guard<boost::mutex> lock(mutex_);
        for (typename CBList::iterator li=list.begin(); li!=list.end() && nreplacements>0; ++li) {
            if (*li==relative_to) {
                li = list.insert(++li, cb);
                --nreplacements;
            }
        }
        return *this;
    }

    /** Insert a callback before another.  The callback is inserted before each occurrence of the @p relative_to
     *  callback. Up to @p nreplacement insertions are made begin$ning at the front of the list.
     *
     *  Thread safety: This method is thread safe.  In fact, it is safe to modify the list while apply() is calling the
     *  functors on that list. */
    List& before(CallbackType *relative_to, CallbackType *cb, size_t nreplacements = UNLIMITED) {
        assert(cb!=NULL);
        boost::lock_guard<boost::mutex> lock(mutex_);
        for (typename CBList::iterator li=list.begin(); li!=list.end() && nreplacements>0; ++li) {
            if (*li==relative_to) {
                li = list.insert(li, cb);
                ++li;
                --nreplacements;
            }
        }
        return *this;
    }

    /** Replace one callback with another.
     *
     *  The replaced callback is removed from the list as if by erase() without deleting it, and its replacement is inserted at
     *  the same position without copying it.  At most @p nreplacements are performed (the default is unlimited).  Replacements
     *  are performed in the direction specified.
     *
     *  Thread safety: This method is thread safe. In fact, it is safe to modify the list while apply() is calling the
     *  functors on that list. */
    List& replace(CallbackType *old_cb, CallbackType *new_cb, size_t nreplacements = UNLIMITED, Direction dir = FORWARD) {
        assert(new_cb!=NULL);
        boost::lock_guard<boost::mutex> lock(mutex_);
        if (FORWARD==dir) {
            for (typename CBList::iterator li=list.begin(); li!=list.end() && nreplacements>0; ++li) {
                if (*li==old_cb) {
                    *li = new_cb;
                    --nreplacements;
                }
            }
        } else {
            for (typename CBList::reverse_iterator li=list.rbegin(); li!=list.rend() && nreplacements>0; ++li) {
                if (*li==old_cb) {
                    *li = new_cb;
                    --nreplacements;
                }
            }
        }
        return *this;
    }

    /** Remove a callback from a list without destroying it.
     *
     *  The list is traversed in the specified direction and the first functor that matches (by pointer comparison) the
     *  specified callback is removed from the list.  Returns true if a functor was removed, false otherwise.
     *
     *  Thread safety: This method is thread safe.  In fact, it is safe to modify the list while apply() is calling the
     *  functors on that list. */
    bool erase(CallbackType *cb, Direction dir=FORWARD) {
        boost::lock_guard<boost::mutex> lock(mutex_);
        if (FORWARD==dir) {
            for (typename CBList::iterator li=list.begin(); li!=list.end(); ++li) {
                if (*li==cb) {
                    list.erase(li);
                    return true;
                }
            }
        } else {
            for (typename CBList::reverse_iterator li=list.rbegin(); li!=list.rend(); ++li) {
                if (*li==cb) {
                    list.erase((++li).base());
                    return true;
                }
            }
        }
        return false;
    }

    /** Remove all callbacks from list without destroying them.
     *
     *  Thread safety: This method is thread safe.  In fact, it is safe to modify the list while apply() is calling the
     *  functors on that list. */
    List& clear() {
        boost::lock_guard<boost::mutex> lock(mutex_);
        list.clear();
        return *this;
    }

    /** Returns a copy of the underlying STL vector of functors.
     *
     *  Thread safety: This method is thread safe. */
    std::list<CallbackType*> callbacks() const {
        boost::lock_guard<boost::mutex> lock(mutex_);
        return list;
    }

    /** Invokes all functors in the callback list.
     *
     *  The functors are invoked sequentially in the order specified by calling its operator() method.  Two arguments are
     *  provided: a boolean value, and an additional argument with parameterized type.  The boolean argument for the first
     *  callback is the @p b argument of this method; the boolean argument of the subsequent callbacks is the return value of
     *  the previous callback; the return value of this method is the return value of the last callback (or the initial value
     *  of @p b if no callbacks were made).
     *
     *  Thread safety:  This method is thread safe.  If this list is modified by one or more of the functors on this list
     *  or by another thread, those changes do not affect which callbacks are made by this invocation of apply().  The
     *  callbacks should not assume that any particular mutexes or other thread synchronization resources are held. It is
     *  possible for a single callback to be invoked concurrently if two or more threads invoke apply() concurrently. */
    template<class ArgumentType>
    bool apply(bool b, const ArgumentType &args, Direction dir=FORWARD) const {
        CBList list = callbacks(); /* copy, so callbacks can safely modify this object's list */
        if (FORWARD==dir) {
            for (typename CBList::iterator li=list.begin(); li!=list.end(); ++li) {
                b = (**li)(b, args);
            }
        } else {
            for (typename CBList::reverse_iterator li=list.rbegin(); li!=list.rend(); ++li) {
                b = (**li)(b, args);
            }
        }
        return b;
    }
};

} // namespace
} // namespace

#endif /* ROSE_callbacks_H */
