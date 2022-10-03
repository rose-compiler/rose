#ifndef ROSE_BinaryAnalysis_ModelChecker_Tag_H
#define ROSE_BinaryAnalysis_ModelChecker_Tag_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/ModelChecker/Types.h>

namespace Rose {
namespace BinaryAnalysis {
namespace ModelChecker {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Base class
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Information tagged to a path node.
 *
 *  Tags can be attached to path nodes to mark interesting things about a certain location within a path. Each node
 *  can have zero or more tags associated with it, and the same tag can be attached to more than one node. Tags have
 *  some information which is common to all (or most) types of tags, but users may create their own subclasses for
 *  special things. */
class Tag: public std::enable_shared_from_this<Tag> {
public:
    using Ptr = TagPtr;

private:
    const size_t nodeStep_;                             // index of the step of the node to which this tag applies

protected:
    Tag(size_t nodeStep);
public:
    virtual ~Tag();

public:
    /** Property: Node step.
     *
     *  This is the index of the step within this node to which this tag applies. The index is specified when the
     *  tag is created.
     *
     *  Thread safety: This property accessor is thread safe. */
    size_t nodeStep() const;

    /** Property: Generic name of tag.
     *
     *  For instance, the name of a null pointer dereference tag might be the words "null pointer dereference" (see @ref
     *  NullDereferenceTag for the actual value).
     *
     *  Thread safety: The implementation must be thread safe. */
    virtual std::string name() const = 0;

    /** String to identify this tag.
     *
     *  Returns a single line string (no line feed) that is suitable for printing on a terminal (no special characters).
     *  This is used mostly in diagnostic messages.
     *
     *  Thread safety: The implementation must be thread safe. */
    virtual std::string printableName() const = 0;

    /** Print multi-line information about the tag.
     *
     *  First line should be the full name of the tag. Next lines are indented by at least two spaces and contain additional
     *  information about the tag. All lines start with the @p prefix.
     *
     *  Thread safety: The implementation must be thread safe for gathering the information but need not concern itself
     *  with ensuring that no other threads are sending output to the same stream. */
    virtual void print(std::ostream&, const std::string &prefix) const = 0;

    /** Print multi-line information about the tag in YAML format.
     *
     *  The first line starts with the @p prefix, and the following lines start with that number of spaces.
     *
     *  Thread safety: The implementation must be thread safe for gathering the information but need not concern itself
     *  with ensuring that no other threads are sending output to the same stream. */
    virtual void toYaml(std::ostream&, const std::string &prefix) const = 0;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ThrowableTag
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Exception wrapper for tags. */
struct ThrownTag {
    Tag::Ptr tag;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// NameTag
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Base class for tags that have a name.
 *
 *  This is a base class for tags that store a string as their name. */
class NameTag: public Tag {
public:
    using Ptr = NameTagPtr;

protected:
    const std::string name_;

protected:
    explicit NameTag(size_t nodeStep, const std::string &name);

public:
    /** Allocating constructor.
     *
     *  Thread safety: This constructor is thread safe. */
    static Ptr instance(size_t nodeStep, const std::string &name);

public:
    virtual std::string name() const override;
    virtual std::string printableName() const override;
    virtual void print(std::ostream&, const std::string &prefix) const override;
    virtual void toYaml(std::ostream&, const std::string &prefix) const override;
};

} // namespace
} // namespace
} // namespace

#endif
#endif
