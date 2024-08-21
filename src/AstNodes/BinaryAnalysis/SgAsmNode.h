#include <Sawyer/Attribute.h>
#include <boost/format.hpp>

/** Base class for all binary analysis IR nodes. */
[[Rosebud::abstract]]
class SgAsmNode: public SgNode {
public:
    /** Attribute storage.
     *
     *  An attribute is extra user-defined data stored in an AST node.
     *
     *  Once ROSETTA is out of the picture, this interface will become cleaner because the typical way to add attribute storage to a
     *  type is to have the type be derived from @c Sawyer::Attribute::Storage. But as things stand now, ROSETTA doesn't support
     *  multiple inheritance and we must therefore use an awkward interface that reads like @c node->attributes().setAttribute(....)
     *  instead of the more streamlined @c node->setAttribute(....). */
    [[using Rosebud: large, mutators(), serialize()]]
    Sawyer::Attribute::Storage<> attributes;
};
