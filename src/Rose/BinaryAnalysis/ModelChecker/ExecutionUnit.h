#ifndef ROSE_BinaryAnalysis_ModelChecker_ExecutionUnit_H
#define ROSE_BinaryAnalysis_ModelChecker_ExecutionUnit_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_MODEL_CHECKER

#include <Rose/BinaryAnalysis/Address.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/BasicTypes.h>
#include <Rose/BinaryAnalysis/ModelChecker/BasicTypes.h>
#include <Rose/SourceLocation.h>

class SgAsmInstruction;

namespace Rose {
namespace BinaryAnalysis {
namespace ModelChecker {

/** One unit of execution.
 *
 *  A unit of execution can be a basic block, a single instruction, a function summary, or any other user-defined
 *  item. Users are expected to subclass this as necessary. */
class ExecutionUnit {
public:
    /** Shared ownership pointer for an execution unit. */
    using Ptr = ExecutionUnitPtr;

private:
    const SourceLocation sourceLocation_;

public:
    ExecutionUnit() = delete;
    explicit ExecutionUnit(const SourceLocation&);
    virtual ~ExecutionUnit();

public:
    /** Property: Printable name.
     *
     *  This returns the name of this unit as a single line that is suitable for terminal output.
     *
     *  Thread safety: The implementation must be thread safe. */
    virtual std::string printableName() const = 0;

    /** Print the steps for this execution unit.
     *
     *  The output should show each step starting on its own line indented by the specified prefix. The steps should be
     *  numbered starting from @p stepOrigin. Additional details about the step should be on subsequent lines and indented in
     *  units of two spaces at a time.
     *
     *  Thread safety: The implementation must be thread safe but may allow other threads to output to the same stream
     *  concurrently. */
    virtual void printSteps(const SettingsPtr&, std::ostream&, const std::string &prefix,
                            size_t stepOrigin, size_t maxSteps) const = 0;

    /** Print the header information in YAML format. */
    virtual void toYamlHeader(const SettingsPtr&, std::ostream&, const std::string &prefix) const = 0;

    /** Print the steps for this execution unit in YAML format.
     *
     *  The output should show each step starting on its own line indented by the specified prefix. The steps should be
     *  numbered starting from @p stepOrigin. Additional details about the steps should be indented further.
     *
     *  Thread safety: The implementation must be thread safe but may allow other threads to output to the same stream
     *  concurrently. */
    virtual void toYamlSteps(const SettingsPtr&, std::ostream&, const std::string &prefix,
                             size_t stepOrigin, size_t maxSteps) const = 0;

    /** Return the steps for this execution unit as SARIF locations.
     *
     *  Thread safety: The implementation must be thread safe. */
    virtual std::vector<Sarif::LocationPtr> toSarif(size_t maxSteps) const = 0;

    /** List source location.
     *
     *  List some source lines if we can find them. If a source location is specified, use it, otherwise use the source
     *  location associated with this execution unit.  If the source location is empty, then nothing is printed.
     *
     *  Thread safety: This method is thread safe.
     *
     * @{ */
    void printSource(const SettingsPtr&, std::ostream&, const std::string &prefix) const;
    void printSource(const SettingsPtr&, std::ostream&, const std::string &prefix, const SourceLocation&) const;
    /** @} */

    /** Property: Number of steps required to execute this unit.
     *
     *  This is a user-defined measure of the approximate difficulty of executing this unit. For instance, it could be the
     *  number of instructions that are executed. The number of steps returned by this method should generally agree with
     *  the number of steps printed when displaying this unit.
     *
     *  Thread safety: The implementation must be thread safe. The easiest way to achieve this is to ensure that the information
     *  doesn't change during model checking. */
    virtual size_t nSteps() const = 0;

    /** Property: Address if it has one.
     *
     *  Returns the address of this execution unit if it has one.
     *
     *  The default implementation always returns nothing.
     *
     *  Thread safety: The implementation must be thread safe. */
    virtual Sawyer::Optional<Address> address() const;

    /** Property: Location in source code.
     *
     *  This property holds an optional source location record.
     *
     *  Thread safety: This property accessor is thread safe. */
    SourceLocation sourceLocation() const;

    /** Predicate to test for unknown instructions.
     *
     *  Returns true if the execution unit contains any instructions whose machine bytes exist but could not be decoded.
     *  This typically indicates that we're trying to execute data or we've chosen the wrong instruction set architecture.
     *
     *  The default implementation always returns false.
     *
     *  Thread safety: The implementation must be thread safe. */
    virtual bool containsUnknownInsn() const;

    /** Execute the unit to create a new state.
     *
     *  The supplied @p riscOperators argument has an attached state that should be modified in place according to the
     *  semantics of the execution unit. It returns zero or more tags to be added to the path node.
     *
     *  This method is not responsible for adding new paths to the work queue--it only updates the state.
     *
     *  Thread safety: The implementation need not be thread safe. */
    virtual std::vector<TagPtr>
    execute(const SettingsPtr&, const SemanticCallbacksPtr&,
            const InstructionSemantics::BaseSemantics::RiscOperatorsPtr &riscOperators) = 0;

    /** Execute a single instruction semantically.
     *
     *  This is a support function for executing a single instruction semantically. Doing this consistently is a little hard
     *  because of the different kinds of exceptions that can be thrown by the semantics layers, and the need to indicate
     *  to the caller whether the execution failed or succeeded independently of returning tags and without any particular
     *  specialized support in the semantics layers.
     *
     *  The low level semantic execution can indicate a failure in different ways:
     *
     *  @li It can set the current state of the RISC operators to null. Since this doesn't involve exceptions, it's sometimes
     *  the fastest way to indicate the failure, but it doesn't say why the failure occurred.
     *
     *  @li It can throw a @ref Rose::Exception which will be caught by this function and emitted in the diagnostic output.
     *  This function will then make sure the current state of the RISC operators is null before returning.
     *
     *  @li It can throw a @ref Tag wrapped in a @ref ThrownTag object, which this function catches and returns. The tag
     *  is printed in the diagnostic output and the current state of the RISC operators is set to null before returning. Do not
     *  throw a @ref ModelChecker::Tag::Ptr directly--these are @c std::shared_ptr that don't interact well with C++ try/catch.
     *
     *  Limitations of this design:
     *
     *  @li It is currently possible to return only one tag by throwing it. If this is a problem, the natural solution is to
     *  extend the @ref ThrownTag to hold more than one pointer.
     *
     *  @li It is not currently possible to return a tag and have the execution succeed. This is because
     *  Dispatcher::processInstruction doesn't catch exceptions and therefore does not perform any final operations on the
     *  semantic state.
     *
     *  Thread safety: This method is NOT thread safe. */
    TagPtr executeInstruction(const SettingsPtr&, SgAsmInstruction*, const InstructionSemantics::BaseSemantics::DispatcherPtr&,
                              size_t nodeStep);
};

} // namespace
} // namespace
} // namespace

#endif
#endif
