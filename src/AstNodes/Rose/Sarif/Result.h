#include <featureTests.h>
#ifdef ROSE_ENABLE_SARIF
#include <Rose/Sarif/Node.h>

#include <Sawyer/Optional.h>

#ifdef ROSE_IMPL
#include <Rose/Sarif/CodeFlow.h>
#include <Rose/Sarif/Location.h>
#endif

namespace Rose {
namespace Sarif {

/** A single result from an analysis.
 *
 *  A result is the child of an @ref Analysis in a SARIF @ref Log. A result has a @ref Kind such as @c PASS or @c FAIL. A failure
 *  result has a @ref Severity such as @c WARNING or @c ERROR. Other result properties are a @ref message, an @ref id "ID", a
 *  reference to a particular @ref rule, a reference to the @ref analysisTarget "analysis target artifact", etc. A result also has
 *  a list of @ ref locations.
 *
 *  Example:
 *
 *  @snippet{trimleft} sarifUnitTests.C sarif_result */
[[Rosebud::no_constructors]]
class Result: public Node {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Properties
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Property: Kind of result.
     *
     *  The kind is set by the constructor and is read-only. */
    [[Rosebud::mutators()]]
    Kind kind = Kind::FAIL;

    /** Property: Severity of result.
     *
     *  The severity is set by the constructor and is read-only. */
    [[Rosebud::mutators()]]
    Severity severity = Severity::ERROR;

    /** Property: Message.
     *
     *  An optional multi-line text message. */
    [[Rosebud::property]]
    std::string message;

    /** Property: Stable ID.
     *
     *  A result may have a stable ID. */
    [[Rosebud::property]]
    std::string id;

    /** Property: Associated rule.
     *
     *  Pointer to an optional rule associated with this result. If this result points to a rule, then the rule must be attached to
     *  the same @ref Analysis as this result before this result can be emitted. Attempting to emit a result pointing to a detached
     *  rule will result in an exception. */
    [[Rosebud::property]]
    RulePtr rule;

    /** Property: Analysis target.
     *
     *  Pointer to an optional artifact associated with this result. If this result points to an analysis target, then that target
     *  must be attached to the same @ref Analysis as one of its artifacts before this result can be emitted. Attempting to emit a
     *  result pointing to a detached artifact will result in an exception. */
    [[Rosebud::property]]
    ArtifactPtr analysisTarget;

public:
    /** Locations associated with this result. */
    [[Rosebud::not_null]]
    EdgeVector<Location> locations;

    /** Code flows associated with this result. */
    [[Rosebud::not_null]]
    EdgeVector<CodeFlow> codeFlows;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Public functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:

    // Find index, if any, of the specified rule within the parent analysis.
    Sawyer::Optional<size_t> findRuleIndex(const RulePtr&);

    // Find index, if any, of the specified artifact within the parent analysis.
    Sawyer::Optional<size_t> findArtifactIndex(const ArtifactPtr&);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Allocating constructor.
     *
     *  If a @ref Kind is specified then it must not be @ref Kind::FAIL. If a @ref Severity is specified then is must not be @ref
     *  Severity::NONE.
     *
     *  If a location is specified, then it becomes the first location. Locations can be added/removed/modified later by adjusting
     *  the @ref locations property.
     *
     * @{ */
    static Ptr instance(Kind, const std::string &mesg);
    static Ptr instance(Kind, const std::string &mesg, const LocationPtr &location);
    static Ptr instance(Severity, const std::string &mesg);
    static Ptr instance(Severity, const std::string &mesg, const LocationPtr &location);
    /** @} */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Overrides
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    bool emit(std::ostream&) override;
};

} // namespace
} // namespace
#endif
