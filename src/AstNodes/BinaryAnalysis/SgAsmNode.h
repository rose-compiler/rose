#include <Sawyer/Attribute.h>
#include <boost/format.hpp>

/** Base class for all binary analysis IR nodes. */
[[Rosebud::abstract]]
class SgAsmNode: public SgNode {
    /** Attribute storage.
     *
     *  An attribute is extra user-defined data stored in an AST node.
     *
     *  Once ROSETTA is out of the picture, this interface will become cleaner because the typical way to add attribute storage to a
     *  type is to have the type be derived from @c Sawyer::Attribute::Storage. But as things stand now, ROSETTA doesn't support
     *  multiple inheritance and we must therefore use an awkward interface that reads like @c node->attributes().setAttribute(....)
     *  instead of the more streamlined @c node->setAttribute(....). */
    [[using Rosebud: large, mutators(), no_serialize]]
    Sawyer::Attribute::Storage<> attributes;

protected:
    // Rosebud generates call to destructorHelper in every destructor implementation. This gives the user a chance to do something
    // special. Any implementations should be prepared to handle multiple invocations on the same object, and should not access any
    // data members in any derived class. Standard C++ rules about calling virtual functions in destructors apply here.
    virtual void destructorHelper() {}
};
