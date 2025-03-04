#ifndef ROSE_BinaryAnalysis_ModelChecker_FailureUnit_H
#define ROSE_BinaryAnalysis_ModelChecker_FailureUnit_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_MODEL_CHECKER

#include <Rose/BinaryAnalysis/ModelChecker/ExecutionUnit.h>

namespace Rose {
namespace BinaryAnalysis {
namespace ModelChecker {

/** An execution unit that always fails.
 *
 *  This is a model checker execution unit that always represents an execution failure. */
class FailureUnit: public ExecutionUnit {
public:
    using Ptr = FailureUnitPtr;

private:
    const Sawyer::Optional<Address> va_;
    const std::string description_;
    TagPtr tag_;

protected:
    FailureUnit() = delete;
    FailureUnit(const Sawyer::Optional<Address>&, const SourceLocation&, const std::string &description,
                const TagPtr&);
public:
    ~FailureUnit();

public:
    /** Allocating constructor.
     *
     *  Constructs a new execution unit to represent an execution failure. The descriptive string must not be empty and
     *  should be a single line of text without vertical characters or leading/trailing white space.
     *
     *  If a tag is provided, then it is thrown when this unit is executed.
     *
     *  Thread safety: This constructor is thread safe.
     *
     *  @{ */
    static Ptr instance(const Sawyer::Optional<Address>&, const SourceLocation&, const std::string &description);
    static Ptr instance(const Sawyer::Optional<Address>&, const SourceLocation&, const std::string &description, const TagPtr&);
    /** @} */

public:
    /** Property: Description.
     *
     *  Description of the failure as a single line of text with no vertical characters or leading/trailing white space.
     *  The description can only be set in the constructor.
     *
     *  Thread safety: This method is thread safe. */
    const std::string& description() const;

public:
    virtual std::string printableName() const override;
    virtual void printSteps(const SettingsPtr&, std::ostream&, const std::string &prefix,
                            size_t stepOrigin, size_t maxSteps) const override;
    virtual void toYamlHeader(const SettingsPtr&, std::ostream&, const std::string &prefix) const override;
    virtual void toYamlSteps(const SettingsPtr&, std::ostream&, const std::string &prefix,
                             size_t stepOrigin, size_t maxSteps) const override;
    virtual std::vector<Sarif::LocationPtr> toSarif(size_t maxSteps) const override;
    virtual size_t nSteps() const override;
    virtual Sawyer::Optional<Address> address() const override;

    virtual std::vector<TagPtr>
    execute(const SettingsPtr&, const SemanticCallbacksPtr&, const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&) override;
};

} // namespace
} // namespace
} // namespace

#endif
#endif
