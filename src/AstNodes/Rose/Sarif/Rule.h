#include <featureTests.h>
#ifdef ROSE_ENABLE_SARIF
#include <Rose/Sarif/Node.h>

#include <boost/uuid/uuid.hpp>
#include <rose_serialize_uuid.h>

namespace Rose {
namespace Sarif {

/** Information that describes a tool analysis result.
 *
 *  This object represents meta data about an analysis result. An analysis result may refer to a particular rule in order to impart
 *  additional meta information to the result.
 *
 *  In the SARIF standard, this object is called a "reportingDescriptor" object and is more general than how we use it here.  The
 *  standard says "A 'reportingDescriptor' object contains information that describes a 'reporting item' generated by a tool. A
 *  reporting item is either a result produced by the tool's analysis, or a notification of a condition encountered by the tool. We
 *  refer to this descriptive information as 'reporting item metadata'. When referring to the metadata that describes a result, we
 *  use the more specific term 'rule metadata'. Some of the properties of the reportingDescriptor object are interpreted differently
 *  depending on whether the object represents a rule or a notification. The description of each property will specify any such
 *  differences."
 *
 *  A rule has properties such as @ref id "ID", @ref name, @ref description, @ref helpUri "help URI", @ref uuid "UUID", etc.
 *
 *  Example:
 *
 *  @snippet{trimleft} sarifUnitTests.C sarif_rule */
class Rule: public Node {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Properties
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Property: Identification string.
     *
     *  The identification string should be a short, stable string that identifies the rule. This string is set by the constructor
     *  and not modifiable thereafter. Identifiers may be opaque to the user. */
    [[Rosebud::ctor_arg, Rosebud::mutators()]]
    std::string id;

    /** Property: Optional name.
     *
     *  A rule may contain a non-null name that is understandable to the user. If the name of a rule contains implementation details
     *  that change over time, a tool author might alter a rule's name while leaving the stable ID property unchanged. A rule name
     *  is suitable in contexts where a readable identifier is preferable and where the lack of stability is not a concern. */
    [[Rosebud::property]]
    std::string name;

    /** Property: Full description.
     *
     *  The full description is a multi-line, multi-paragraph string describing the rule in detail. It is initially set during
     *  construction but can be changed later. */
    [[Rosebud::ctor_arg]]
    std::string description;

    /** Property: Help URI.
     *
     *  A URI pointing to the primary documentation for the reporting item. The documentation might include examples, author contact
     *  information, and links to additional information. */
    [[Rosebud::property]]
    std::string helpUri;

    /** Property: Universally unique identifier.
     *
     *  The universally unique identification. This is assigned during construction, but can be changed later. */
    [[Rosebud::property]]
    boost::uuids::uuid uuid;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Overrides
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    bool emit(std::ostream&) override;
};

} // namespace
} // namespace
#endif
