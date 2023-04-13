#include <boost/format.hpp>

/** Base class for all binary analysis IR nodes. */
[[Rosebud::abstract]]
class SgAsmNode: public SgNode {
protected:
    // Rosebud generates call to destructorHelper in every destructor implementation. This gives the user a chance to do something
    // special. Any implementations should be prepared to handle multiple invocations on the same object, and should not access any
    // data members in any derived class. Standard C++ rules about calling virtual functions in destructors apply here.
    virtual void destructorHelper() {}
};
