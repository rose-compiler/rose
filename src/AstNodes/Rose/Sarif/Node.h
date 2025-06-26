#include <featureTests.h>
#ifdef ROSE_ENABLE_SARIF
#include <Rose/Sarif/BasicTypes.h>
#include <Rose/Sarif/NodeDeclarations.h>

#include <Sawyer/Tree.h>

#include <nlohmann/json.h>

namespace Rose {
namespace Sarif {

/** Base class for Sarif nodes. */
[[Rosebud::abstract]]
class Node {
public:
    /** Emit this node and its children as JSON.
     *
     *  Returns true if this node produced (or tried to produce) any output, false if not. */
    virtual bool emit(std::ostream&);

private:
    /** Emit this node's properties bag if present.
     *
     *  Returns true if this node produced (or tried to produce) any output, false if not. */
    bool emitProperties(std::ostream &);

public:

    /** Property: property bag for holding JSON metadata.
     *
     *
     *  SARIF nodes can hold arbitrary metadata in this field, as described in SARIF §3.8.1.
     *  This field is an accessor for the metadata as a key-value map -- for adding new entries, see @ref addProperty.
     *
     *  */
    [[using Rosebud: large, not_null]] 
    nlohmann::json properties;

public:
    /** Add a property to this object's property bag.
     *
     *  Property names must be hierarchical strings (must not contain forward slash characters, see SARIF §3.5.4)
     *  If the property name is valid, inserts the input into properties and returns true, otherwise returns false.
     **/
    bool addProperty(const std::string &, const nlohmann::json &);
};

} // namespace
} // namespace
#endif
