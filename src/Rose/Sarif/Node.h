#ifndef ROSE_Sarif_Node_H
#define ROSE_Sarif_Node_H
#include <Rose/Sarif/BasicTypes.h>

#include <Rose/Sarif/Exception.h>
#include <Rose/StringUtility/Escape.h>
#include <Sawyer/Tree.h>

namespace Rose {
namespace Sarif {

/** Base class for SARIF nodes. */
class Node: public Sawyer::Tree::Vertex<Node> {
    size_t yamlIndentAmount_ = 2;
    bool isFrozen_ = false;

public:
    virtual ~Node();
protected:
    Node();

public:
    /** Shared-ownership pointer to a @ref Node object.
     *
     *  @{ */
    using Ptr = NodePtr;
    using ConstPtr = NodeConstPtr;
    /** @} */

    /** True when log is in incremental mode.
     *
     *  Returns true if this object is attached to a log and the log is in incremental mode. */
    virtual bool isIncremental();

    /** The stream for incremental output.
     *
     *  If @ref isIncremental returns true, then this function returns a reference to the stream to which incremental output should
     *  be emitted. Otherwise it returns a reference to a default constructed output stream. */
    virtual std::ostream& incrementalStream();

    /** Whether an object is frozen.
     *
     *  In incremental mode, if a parent object emits information after one of its children, it should mark that child as frozen
     *  to indicate that it's now too late to change anything in that child. For instance, when a second @ref Result object is
     *  added to an @ref Analysis object and the Analysis object is attached to a @ref Log that's in incremental mode, then the
     *  Analysis must mark the first Result as frozen -- since we've started emitting information about the second Result, it's
     *  no longer possible to go back and modify the first Result.
     *
     * @{ */
    bool isFrozen() const;
    void freeze();
    void thaw();
    /** @} */

    /** Emit output for this node. */
    virtual void emitYaml(std::ostream&, const std::string &prefix) = 0;

    /** Base YAML prefix for this object. */
    virtual std::string emissionPrefix();

protected:
    // Given a YAML line prefix for the first item in an object, create the prefix for the rest of the items.
    std::string makeNextPrefix(const std::string&);

    // Given a YAML line prefix, make a new prefix for an indented object
    std::string makeObjectPrefix(const std::string&);

    // Given a YAML line prefix, make a new prefix for an indented list item.
    std::string makeListPrefix(const std::string&);

    template<class T>
    static void lock(Edge<T> &edge, const char *dataMemberName) {
        const std::string mesg = std::string(dataMemberName) + " cannot be reassigned";
        edge.beforeChange([mesg](const Node::Ptr&, const Node::Ptr&) {
            throw IncrementalError(mesg);
        });
    }
};

} // namespace
} // namespace

#endif
