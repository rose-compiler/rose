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

public:
    void emitYaml(std::ostream&, const std::string &prefix) override;
    std::string emissionPrefix() override;

private:
    void emitId(std::ostream&, const std::string &prefix);
    void checkLocationsResize(int delta, const LocationPtr&);
    void handleLocationsResize(int delta, const LocationPtr&);


};

} // namespace
} // namespace
#endif
