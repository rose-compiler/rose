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
 *  The @ref SharedObject class is used in conjunction with @ref SharedPointer to declare that objects of this type are
 *  allocated on the heap (only), are reference counted, and invoke "delete" on themselves when the reference count decreases to
 *  zero.  Any object that is intended to be referenced by a @ref SharedPointer must inherit directly or indirectly from
 *  @ref SharedObject.
 *
 *  Here's an example that demonstrates some of the best practices:
 *
 *  @li Provide a declaration for the pointer by appending "Ptr" to the class name.
 *
 *  @li Inherit from @ref SharedObject just once in the entire class hierarchy, normally in the base class.
 *
 *  @li Provide a public "Ptr" member type that's equivalent to the forward-declared pointer type.
 *
 *  @li Make all constructors "protected" so that users are less likely to construct objects without allocating them in the
 *  heap.
 *
 *  @li Provide static member functions named "instance", one per constructor, that allocate the object on the heap and return
 *  its first reference-counting pointer.
 *
 * @code
 *  // Forward declarations if necessary
 *  class MyBaseClass;
 *  typedef Sawyer::SharedPointer<MyBaseClass> MyBaseClassPtr;
 *
 *  // Class definition
 *  class MyBaseClass: public SharedObject {
 *  public:
 *      typedef Sawyer::SharedPointer<MyBaseClass> Ptr;
 *
 *  protected:
 *      // Hide the constructors so users don't accidentally create objects on the stack
 *      MyBaseClass();
 *      MyBaseClass(const MyBaseClass&);
 *
 *  public:
 *      // Provide allocating wrappers around the constructors
 *      static Ptr instance() {
 *          return Ptr(new MyBaseClass);
 *      }
 *      static Ptr instance(const MyBaseClass &other) {
 *          return Ptr(new MyBaseClass(other));
 *      }
 *  };
 * @endcode
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
    SharedObject(const SharedObject&): nrefs_(0) {}

    /** Virtual destructor. Verifies that the reference count is zero. */
    virtual ~SharedObject() {
        ASSERT_require(nrefs_==0);
    }

    /** Assignment.
     *
     *  Assigning one object to another doesn't change the reference count or mutex of either object. */
    SharedObject& operator=(const SharedObject&) {
        return *this;
    }
};

} // namespace

#endif
