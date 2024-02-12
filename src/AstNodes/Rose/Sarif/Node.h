#include <featureTests.h>
#ifdef ROSE_ENABLE_SARIF
#include <Sawyer/Tree.h>
#include <Rose/Sarif/BasicTypes.h>
#include <Rose/Sarif/NodeDeclarations.h>

namespace Rose {
namespace Sarif {

/** Base class for Sarif nodes. */
[[Rosebud::abstract]]
class Node {
public:
    /** Emit this node and its children as JSON.
     *
     *  Returns true if this node produced (or tried to produce) any output, false if not. */
    virtual bool emit(std::ostream&) = 0;
};

} // namespace
} // namespace
#endif
