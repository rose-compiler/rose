#include "conditionalDisable.h"
#ifdef ROSE_BINARY_TEST_DISABLED
#include <iostream>
int main() { std::cout <<"disabled for " <<ROSE_BINARY_TEST_DISABLED <<"\n"; return 1; }
#else

#include <rose.h>
#include <Rose/BinaryAnalysis/BasicTypes.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/SymbolicSemantics.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>
#include <Rose/BinaryAnalysis/SymbolicExpression.h>
#include <Rose/BinaryAnalysis/Z3Solver.h>

using namespace Rose;
using namespace Rose::BinaryAnalysis;

static void
test_add_simplifications() {
    SymbolicExpression::Ptr reg = SymbolicExpression::makeIntegerVariable(32, "esp_0");
    SymbolicExpression::Ptr nreg = SymbolicExpression::Interior::instance(SymbolicExpression::OP_NEGATE, reg);
    SymbolicExpression::Ptr number = SymbolicExpression::makeIntegerConstant(32, 0xfffffffc);
    SymbolicExpression::Ptr t1 = SymbolicExpression::Interior::instance(SymbolicExpression::OP_ADD, reg, nreg, number);
    std::cout <<"(add esp_0 (negate esp_0) 0xfffffffc) = " <<*t1 <<"\n";

    SymbolicExpression::Ptr n1 = SymbolicExpression::makeIntegerVariable(32, "esp_0");
    SymbolicExpression::Ptr n2 = SymbolicExpression::makeIntegerConstant(32, 4);
    SymbolicExpression::Ptr n3 = SymbolicExpression::makeIntegerConstant(32, 8);
    SymbolicExpression::Ptr n4 = SymbolicExpression::Interior::instance(SymbolicExpression::OP_ADD, n1, n2);
    SymbolicExpression::Ptr n5 = SymbolicExpression::Interior::instance(SymbolicExpression::OP_ADD, n4, n3);
    SymbolicExpression::Ptr n6 = SymbolicExpression::Interior::instance(SymbolicExpression::OP_NEGATE, n1);
    SymbolicExpression::Ptr n7 = SymbolicExpression::Interior::instance(SymbolicExpression::OP_ADD, n5, n6);
    std::cout <<"(add esp_0 4 8 (negate esp_0)) = " <<*n7 <<"\n";
}

static void
test_svalues() {
    using namespace InstructionSemantics;

    const SymbolicSemantics::SValue::Ptr protoval = SymbolicSemantics::SValue::instance();

    BaseSemantics::RiscOperators::Ptr ops =
        SymbolicSemantics::RiscOperators::instanceFromProtoval(protoval, SmtSolver::Ptr());
    ASSERT_always_not_null(ops);

    BaseSemantics::SValue::Ptr a1 = ops->undefined_(32);                 // v1
    BaseSemantics::SValue::Ptr a2 = ops->add(a1, ops->number_(32, 4));   // (add v1 4)
    BaseSemantics::SValue::Ptr a3 = ops->add(a2, ops->number_(32, 8));   // (add v1 12)
    BaseSemantics::SValue::Ptr a4 = ops->subtract(a3, a2);               // (add v1 12 (negate (add v1 4)))
    std::cout <<"(add v1 4 8 (negate (add v1 4))) = " <<*a4 <<"\n";
}

int
main() {
    test_add_simplifications();
    test_svalues();
}
#endif
