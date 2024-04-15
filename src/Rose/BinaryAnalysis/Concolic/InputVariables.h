#ifndef ROSE_BinaryAnalysis_Concolic_InputVariables_H
#define ROSE_BinaryAnalysis_Concolic_InputVariables_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <Rose/BinaryAnalysis/Concolic/BasicTypes.h>

#include <Rose/BinaryAnalysis/SymbolicExpression.h>
#include <Rose/Constants.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

/** Manages variables.
 *
 *  This class maintains two lists:
 *
 *  @li A list of all known variables and their current bindings to values. These normally come from execution events that have both
 *  a non-null @ref ExecutionEvent::variable "variable" and a non-null @ref ExecutionEvent::value "value" property, but it's possible
 *  to add other bindings to this object as well.
 *
 *  @li A list of executin events that have variables that are being treated as test case inputs. This list is indexed by variable.
 *
 *  This class also coordinates the lists by adjusting lists and execution events in tandem. */
class InputVariables: public Sawyer::SharedObject {
public:
    /** Shared ownership pointer. */
    using Ptr = InputVariablesPtr;

private:
    using Variables = Sawyer::Container::Map<uint64_t, ExecutionEventPtr>; // map symbolic variable ID to program input
    Variables variables_;
    SymbolicExpression::ExprExprHashMap bindings_;      // used for substitutions

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors, etc.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
protected:
    InputVariables();

public:
    /** Allocating constructor. */
    static Ptr instance();

    ~InputVariables();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Properties and queries
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:

    /** Property: Variable value bindings.
     *
     *  This is the set of all variable-value bindings. It is a list of variables and their corresponding values. This set
     *  is adjusted indirectly through other functions in this API. */
    const SymbolicExpression::ExprExprHashMap& bindings() const;

    /** Find an execution event for an input variable.
     *
     *  Returns the event that has the specified input variable. Returns null if there is no such event. The variable must
     *  be defined by having called @ref define or equivalent.
     *
     * @{ */
    ExecutionEventPtr event(const std::string &variableName) const;
    ExecutionEventPtr event(const SymbolicExpressionPtr &variable) const;
    /** @} */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // High-level functions.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /** Makes an execution event an input variable.
     *
     *  The specified @ref ExecutionEvent::variable, which must be non-null and, is made into an
     *  input variable.  This is a multi step process that:
     *
     *  @li Deactivates the variable if activated (see @ref deactivate).
     *
     *  @li Gives the variable an input type and indexes in the event (see @ref define).
     *
     *  @li Adds the variable to the list of known input variables in this object (see @ref define).
     *
     *  @li Binds the variable to a value if the event has a value (see @ref bind). */
    void activate(const ExecutionEventPtr&, InputType, size_t idx1 = INVALID_INDEX, size_t idx2 = INVALID_INDEX);

    /** Deactivates an execution event input variable.
     *
     *  The specified @ref ExecutionEvent::variable is turned into a non-input variable with the following steps:
     *
     *  @li The variable is unbound from any value (see @ref unbind).
     *
     *  @li The variable is removed from the list of known input variables in this object (see @ref undefine).
     *
     *  @li The @ref ExecutionEvent::inputType is set to @c NONE and the indices are cleared (see @ref undefine). */
    void deactivate(const ExecutionEventPtr&);

    /** Play an event.
     *
     *  If the event has a variable and a value, then a new binding is added to this object. If the event's variable is marked
     *  as an input variable, then it's added to the list of input variables. This function is normally called after reading
     *  events from a database when initializing the concolic state. The event is not modified. */
    void playback(const ExecutionEventPtr&);

    /** Un-play an event.
     *
     *  This is basically the inverse of playback. It removes the event's information from this object without changing the
     *  event. */
    void unplayback(const ExecutionEventPtr&);

    /** Add variable bindings to an SMT solver. */
    void addBindingsToSolver(const SmtSolverPtr&) const;

    /** Print all defined variables. */
    void print(std::ostream&, const std::string &prefix = "") const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Mid-level functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /** Define an input variable.
     *
     *  The @ref ExecutionEvent::variable is made into an input variable by giving the event an input type and adding the
     *  variable to the list of known variables in this object. The optional indices are used for things like "argv" and "envp"
     *  that are lists of strings in order specify the exact character the variable references.
     *
     *  Binding the variable to a value is a separate step accomblished by the @ref bind function.
     *
     *  See also, @ref undefine. */
    void define(const ExecutionEventPtr&, InputType, size_t idx1 = INVALID_INDEX, size_t idx2 = INVALID_INDEX);

    /** Undefine an input variable.
     *
     *  This is roughly the inverse of @ref define. Namely, the @ref ExecutionEvent::inputType is reset to @c NONE, and the
     *  variable is removed from the list of known input variables in this object. These steps are no-ops if they're not
     *  applicable.
     *
     *  The variable + value binding (if any) is not removed since it was not added by @ref define. If you want to remove it, then
     *  either use @ref deactivate or @ref unbind.
     *
     *  See also, @ref define. */
    void undefine(const ExecutionEventPtr&);

    /** Bind a variable to a value.
     *
     *  Binds an event's @ref ExecutionEvent::variable "variable" to the event's @ref ExecutionEvent::value "value". This is
     *  a no-op if the variable or value is null. The execution event is not affected in any way.
     *
     *  See also, @ref unbind. */
    void bind(const ExecutionEventPtr&);

    /** Unbind a variable from its value.
     *
     *  If the event has a @ref ExecutionEvent::variable "variable", then it's unbound from any value in this object's
     *  @ref bindings property. The execution event is not affected in any way.
     *
     *  The specified variable is unbound from any value it might have.  This is a no-op if the variable is null or is not
     *  bound.
     *
     *  See also, @ref bind. */
    void unbind(const ExecutionEventPtr&);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Low-level functions.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /** Binds or rebinds a variable to a value.
     *
     *  The variable and value must both be non-null. The association between the variable and the value is added to
     *  the @ref bindings property. */
    void bindVariableValue(const SymbolicExpressionPtr &variable, const SymbolicExpressionPtr &value);

    /** Unbinds a value from a variable.
     *
     *  Removes the variable and its binding from the @ref bindings property if present. If the variable is null then this
     *  function does nothing. */
    void unbindVariableValue(const SymbolicExpressionPtr &variable);
};

} // namespace
} // namespace
} // namespace

#endif
#endif
