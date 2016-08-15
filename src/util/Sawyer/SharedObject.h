// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




#ifndef Sawyer_SharedObject_H
#define Sawyer_SharedObject_H

#include <Sawyer/Sawyer.h>
#include <Sawyer/Synchronization.h>

namespace Sawyer {

/** Base class for reference counted objects.
 *
 *  Any reference counted object should inherit from this class, which provides a default constructor, virtual destructor, and
 *  a private reference count data member.
 *
 *  @sa SharedPointer, @ref SharedFromThis */
class SAWYER_EXPORT SharedObject {
    template<class U> friend class SharedPointer;
    mutable SAWYER_THREAD_TRAITS::Mutex mutex_;
    mutable size_t nrefs_;
public:
    /** Default constructor.  Initializes the reference count to zero. */
    SharedObject(): nrefs_(0) {}

    /** Copy constructor.
     *
     *  Shared objects are not typically copy-constructed, but we must support it anyway in case the user wants to
     *  copy-construct some shared object.  The new object has a ref-count of zero. */
    SharedObject(const SharedObject &other): nrefs_(0) {}

    /** Virtual destructor. Verifies that the reference count is zero. */
    virtual ~SharedObject() {
        ASSERT_require(nrefs_==0);
    }

    /** Assignment.
     *
     *  Assigning one object to another doesn't change the reference count or mutex of either object. */
    SharedObject& operator=(const SharedObject &other) {
        return *this;
    }
};

} // namespace

#endif
