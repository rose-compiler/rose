#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Concolic/InputVariables.h>

#include <Rose/BinaryAnalysis/Concolic/ExecutionEvent.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Input Variables
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
InputVariables::insertProgramArgumentCount(const ExecutionEvent::Ptr &event, const SymbolicExpr::Ptr &variable) {
    ASSERT_not_null(event);
    ASSERT_require(event->inputType() == InputType::NONE);
    ASSERT_not_null(variable);
    ASSERT_require(variable->isVariable2());

    event->inputType(InputType::PROGRAM_ARGUMENT_COUNT);
    event->inputVariable(variable);
    event->name("argc");

    variables_.insert(*variable->variableId(), event);
}

void
InputVariables::insertProgramArgument(const ExecutionEvent::Ptr &event, size_t i, size_t j,
                                      const SymbolicExpr::Ptr &variable) {
    ASSERT_not_null(event);
    ASSERT_require(event->inputType() == InputType::NONE);
    ASSERT_not_null(variable);
    ASSERT_require(variable->isVariable2());

    event->inputType(InputType::PROGRAM_ARGUMENT);
    event->inputVariable(variable);
    event->inputI1(i);
    event->inputI2(j);
    event->name((boost::format("argv.%d.%d") %i %j).str()); // because "." is easier to read (not escaped like []

    variables_.insert(*variable->variableId(), event);
}

void
InputVariables::insertEnvironmentVariable(const ExecutionEvent::Ptr &event, size_t i, size_t j,
                                          const SymbolicExpr::Ptr &variable) {
    ASSERT_not_null(event);
    ASSERT_require(event->inputType() == InputType::NONE);
    ASSERT_not_null(variable);
    ASSERT_require(variable->isVariable2());

    event->inputType(InputType::ENVIRONMENT);
    event->inputVariable(variable);
    event->inputI1(i);
    event->inputI2(j);
    event->name((boost::format("envp.%d.%d") %i %j).str()); // because "." is easier to read (not escaped like []

    variables_.insert(*variable->variableId(), event);
}

void
InputVariables::insertSystemCallReturn(const ExecutionEventPtr &event, const SymbolicExpr::Ptr &variable) {
    ASSERT_not_null(event);
    ASSERT_require(event->inputType() == InputType::NONE);
    ASSERT_not_null(variable);
    ASSERT_require(variable->isVariable2());

    event->inputType(InputType::SYSTEM_CALL_RETVAL);
    event->inputVariable(variable);
    if (variable->comment().empty()) {
        variable->comment(event->name());
    } else if (event->name().empty()) {
        event->name(variable->comment());
    }

    variables_.insert(*variable->variableId(), event);
}

void
InputVariables::insertEvent(const ExecutionEventPtr &event) {
    ASSERT_not_null(event);
    ASSERT_forbid(event->inputType() == InputType::NONE);
    SymbolicExpr::Ptr variable = event->inputVariable();
    ASSERT_not_null(variable);
    ASSERT_require(variable->isVariable2());
    ASSERT_forbid(event->name().empty());

    variables_.insert(*variable->variableId(), event);
}

ExecutionEvent::Ptr
InputVariables::get(const std::string &symbolicVariableName) const {
    ASSERT_require(symbolicVariableName.size() >= 2);
    ASSERT_require(symbolicVariableName[0] == 'v');
    uint64_t varId = rose_strtoull(symbolicVariableName.c_str()+1, NULL, 10);
    return variables_.getOrDefault(varId);
}

void
InputVariables::print(std::ostream &out, const std::string &prefix) const {
    BOOST_FOREACH (const Variables::Node &node, variables_.nodes()) {
        uint64_t variableNumber = node.key();
        ExecutionEvent::Ptr event = node.value();
        out <<prefix <<event->name() <<" = v" <<variableNumber <<"\n";
    }
}

} // namespace
} // namespace
} // namespace

#endif
