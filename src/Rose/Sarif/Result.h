#ifndef ROSE_Sarif_Result_H
#define ROSE_Sarif_Result_H
#include <Rose/Sarif/Node.h>

namespace Rose {
namespace Sarif {

/** A single result from an analysis.
 *
 *  A result is the child of an @ref Analysis in a SARIF @ref Log. Each result has a severity level, a message, and zero or
 *  more locations.  */
class Result: public Node {
public:
    /** Shared-ownership pointer to a @ref Result object.
     *
     *  @{ */
    using Ptr = ResultPtr;
    using ConstPtr = ResultConstPtr;
    /** @} */

private:
    Kind kind_ = Kind::FAIL;
    Severity severity_ = Severity::ERROR;
    std::string message_;
    std::string id_;                                    // optional stable ID for this result
    RulePtr rule_;                                      // optional rule pointer

public:
    /** Locations associated with this result. */
    EdgeVector<Location> locations;

public:
    ~Result();
protected:
    Result(Kind, Severity, const std::string &mesg);          // use `instance` instead
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

    /** Property: Kind of result.
     *
     *  The kind is set by the constructor and is read-only. */
    Kind kind() const;

    /** Property: Severity of result.
     *
     *  The severity is set by the constructor and is read-only. */
    Severity severity() const;

    /** Property: Stable ID.
     *
     *  A result may have a stable ID.
     *
     * @{ */
    const std::string& id() const;
    void id(const std::string&);
    /** @} */

    /** Property: Associated rule.
     *
     *  Pointer to an optional rule associated with this result. If this result points to a rule, then the rule must be attached to
     *  the same @ref Analysis as this result before this result can be emitted. Attempting to emit a result pointing to a detached
     *  rule will result in an exception.
     *
     *  @{ */
    RulePtr rule() const;
    void rule(const RulePtr&);
    /** @} */

public:
    void emitYaml(std::ostream&, const std::string &prefix) override;
    std::string emissionPrefix() override;

private:
    void emitId(std::ostream&, const std::string &prefix);
    void emitRule(std::ostream&, const std::string &prefix);
    void checkLocationsResize(int delta, const LocationPtr&);
    void handleLocationsResize(int delta, const LocationPtr&);
    Sawyer::Optional<size_t> findRuleIndex(const RulePtr&);
};

} // namespace
} // namespace
#endif
