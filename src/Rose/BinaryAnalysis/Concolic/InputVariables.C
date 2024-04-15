#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <Rose/BinaryAnalysis/Concolic/InputVariables.h>

#include <Rose/BinaryAnalysis/Concolic/ExecutionEvent.h>
#include <Rose/BinaryAnalysis/SmtSolver.h>
#include <Rose/BinaryAnalysis/SymbolicExpression.h>

#include <boost/lexical_cast.hpp>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Input Variables
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

InputVariables::InputVariables() {}

InputVariables::~InputVariables() {}

InputVariables::Ptr
InputVariables::instance() {
    return Ptr(new InputVariables);
}

const SymbolicExpression::ExprExprHashMap&
InputVariables::bindings() const {
    return bindings_;
}

void
InputVariables::activate(const ExecutionEvent::Ptr &event, InputType inputType, size_t idx1, size_t idx2) {
    ASSERT_not_null(event);
    ASSERT_require(event->variable());
    ASSERT_forbid(InputType::NONE == inputType);

    deactivate(event);
    define(event, inputType, idx1, idx2);
    if (event->value())
        bindVariableValue(event->variable(), event->value());
}

void
InputVariables::deactivate(const ExecutionEvent::Ptr &event) {
    if (event && event->variable())
        unbindVariableValue(event->variable());
    undefine(event);
}

void
InputVariables::playback(const ExecutionEvent::Ptr &event) {
    if (event && event->variable()) {
        if (event->value())
            bindVariableValue(event->variable(), event->value());
        if (InputType::NONE != event->inputType())
            variables_.insert(event->variable()->variableId().get(), event);
    }
}

void
InputVariables::unplayback(const ExecutionEvent::Ptr &event) {
    if (event && event->variable()) {
        unbindVariableValue(event->variable());
        variables_.erase(*event->variable()->variableId());
    }
}

void
InputVariables::print(std::ostream &out, const std::string &prefix) const {
    for (const Variables::Node &node: variables_.nodes()) {
        uint64_t variableNumber = node.key();
        ExecutionEvent::Ptr event = node.value();
        out <<prefix <<event->name() <<" = v" <<variableNumber <<"\n";
    }
}

void
InputVariables::define(const ExecutionEvent::Ptr &event, InputType inputType, size_t idx1, size_t idx2) {
    ASSERT_not_null(event);
    ASSERT_not_null(event->variable());
    ASSERT_require2(InputType::NONE == event->inputType(), "already defined");
    ASSERT_forbid(InputType::NONE == inputType);
    ASSERT_require(INVALID_INDEX == idx2 || INVALID_INDEX != idx1);

    undefine(event);
    event->inputType(inputType, idx1, idx2);
    variables_.insert(event->variable()->variableId().get(), event);
}

void
InputVariables::undefine(const ExecutionEvent::Ptr &event) {
    if (event) {
        if (event->variable())
            variables_.erase(event->variable()->variableId().get());
        event->inputType(InputType::NONE, INVALID_INDEX, INVALID_INDEX);
    }
}

void
InputVariables::bind(const ExecutionEvent::Ptr &event) {
    ASSERT_not_null(event);
    if (event->variable() && event->value())
        bindVariableValue(event->variable(), event->value());
}

void
InputVariables::unbind(const ExecutionEvent::Ptr &event) {
    ASSERT_not_null(event);
    unbindVariableValue(event->variable());
}

void
InputVariables::bindVariableValue(const SymbolicExpression::Ptr &variable, const SymbolicExpression::Ptr &value) {
    ASSERT_not_null(variable);
    ASSERT_not_null(value);
    bindings_.insert(std::make_pair(variable, value));
}

void
InputVariables::unbindVariableValue(const SymbolicExpression::Ptr &variable) {
    if (variable)
        bindings_.erase(variable);
}

ExecutionEvent::Ptr
InputVariables::event(const std::string &variableName) const {
    ASSERT_require(variableName.size() >= 2);
    ASSERT_require(variableName[0] == 'v');
    uint64_t varId = boost::lexical_cast<uint64_t>(variableName.substr(1));
    return variables_.getOrDefault(varId);
}

ExecutionEvent::Ptr
InputVariables::event(const SymbolicExpression::Ptr &variable) const {
    ASSERT_not_null(variable);
    ASSERT_require(variable->isScalarVariable());
    uint64_t varId = *variable->variableId();
    return variables_.getOrDefault(varId);
}

void
InputVariables::addBindingsToSolver(const SmtSolver::Ptr &solver) const {
    ASSERT_not_null(solver);
    for (const auto &node: bindings_) {
        SymbolicExpression::Ptr eq = SymbolicExpression::makeEq(node.first, node.second);
        solver->insert(eq);
    }
}

} // namespace
} // namespace
} // namespace

#endif
