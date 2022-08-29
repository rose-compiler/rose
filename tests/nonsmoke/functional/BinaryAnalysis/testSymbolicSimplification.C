#include "conditionalDisable.h"
#ifdef ROSE_BINARY_TEST_DISABLED
#include <iostream>
int main() { std::cout <<"disabled for " <<ROSE_BINARY_TEST_DISABLED <<"\n"; return 1; }
#else

#include <rose.h>
#include <Rose/BinaryAnalysis/BasicTypes.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/SymbolicSemantics.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>
#include <Rose/BinaryAnalysis/SymbolicExpr.h>
#include <Rose/BinaryAnalysis/Z3Solver.h>

using namespace Rose;
using namespace Rose::BinaryAnalysis;

static void
test_add_simplifications() {
    SymbolicExpr::Ptr reg = SymbolicExpr::makeIntegerVariable(32, "esp_0");
    SymbolicExpr::Ptr nreg = SymbolicExpr::Interior::instance(SymbolicExpr::OP_NEGATE, reg);
    SymbolicExpr::Ptr number = SymbolicExpr::makeIntegerConstant(32, 0xfffffffc);
    SymbolicExpr::Ptr t1 = SymbolicExpr::Interior::instance(SymbolicExpr::OP_ADD, reg, nreg, number);
    std::cout <<"(add esp_0 (negate esp_0) 0xfffffffc) = " <<*t1 <<"\n";

    SymbolicExpr::Ptr n1 = SymbolicExpr::makeIntegerVariable(32, "esp_0");
    SymbolicExpr::Ptr n2 = SymbolicExpr::makeIntegerConstant(32, 4);
    SymbolicExpr::Ptr n3 = SymbolicExpr::makeIntegerConstant(32, 8);
    SymbolicExpr::Ptr n4 = SymbolicExpr::Interior::instance(SymbolicExpr::OP_ADD, n1, n2);
    SymbolicExpr::Ptr n5 = SymbolicExpr::Interior::instance(SymbolicExpr::OP_ADD, n4, n3);
    SymbolicExpr::Ptr n6 = SymbolicExpr::Interior::instance(SymbolicExpr::OP_NEGATE, n1);
    SymbolicExpr::Ptr n7 = SymbolicExpr::Interior::instance(SymbolicExpr::OP_ADD, n5, n6);
    std::cout <<"(add esp_0 4 8 (negate esp_0)) = " <<*n7 <<"\n";
}

static void
test_svalues() {
    using namespace InstructionSemantics;

    const SymbolicSemantics::SValue::Ptr protoval = SymbolicSemantics::SValue::instance();

    BaseSemantics::RiscOperators::Ptr ops =
        SymbolicSemantics::RiscOperators::instanceFromProtoval(protoval, SmtSolver::Ptr());
    ASSERT_always_not_null(ops);

    BaseSemantics::SValuePtr a1 = ops->undefined_(32);                 // v1
    BaseSemantics::SValuePtr a2 = ops->add(a1, ops->number_(32, 4));   // (add v1 4)
    BaseSemantics::SValuePtr a3 = ops->add(a2, ops->number_(32, 8));   // (add v1 12)
    BaseSemantics::SValuePtr a4 = ops->subtract(a3, a2);               // (add v1 12 (negate (add v1 4)))
    std::cout <<"(add v1 4 8 (negate (add v1 4))) = " <<*a4 <<"\n";
}

int
main() {
    test_add_simplifications();
    test_svalues();
}
#endif
