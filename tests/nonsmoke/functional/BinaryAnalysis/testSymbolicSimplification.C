#include "conditionalDisable.h"
#ifdef ROSE_BINARY_TEST_DISABLED
#include <iostream>
int main() { std::cout <<"disabled for " <<ROSE_BINARY_TEST_DISABLED <<"\n"; return 1; }
#else

#include <rose.h>
#include <BinarySymbolicExpr.h>
#include <BinaryYicesSolver.h>
#include <BinaryZ3Solver.h>
#include <SymbolicSemantics2.h>

using namespace Rose;
using namespace Rose::BinaryAnalysis;

#if 0 // [Robb P. Matzke 2015-06-25]: cannot be tested automatically since Jenkins might not have an SMT solver
static void
test_yices_linkage() {
    if (unsigned avail = YicesSolver::availableLinkage()) {
        std::cout <<"Yices is available as";
        if (avail & YicesSolver::LM_EXECUTABLE)
            std::cout <<" an executable";
        if (avail & YicesSolver::LM_LIBRARY)
            std::cout <<" a library";
        std::cout <<"\n";
    } else {
        std::cout <<"Yices is not available";
    }
    
    YicesSolver yices1;
    switch (yices1.linkage()) {
        case YicesSolver::LM_NONE:
            std::cout <<"Yices is not available.\n";
            break;
        case YicesSolver::LM_LIBRARY:
            std::cout <<"Yices is linked as a library.\n";
            break;
        case YicesSolver::LM_EXECUTABLE:
            std::cout <<"Yices is used as a separate executable.\n";
            break;
        default:
            ASSERT_not_reachable("invalid yices linkage");
    }

    if ((YicesSolver::availableLinkage() & YicesSolver::LM_LIBRARY) != 0) {
        YicesSolver yices2;
        yices2.linkage(YicesSolver::LM_LIBRARY);
        switch (yices2.linkage()) {
            case YicesSolver::LM_NONE:
                std::cout <<"Yices is not available.\n";
                break;
            case YicesSolver::LM_LIBRARY:
                std::cout <<"Yices is linked as a library.\n";
                break;
            case YicesSolver::LM_EXECUTABLE:
                std::cout <<"Yices is used as a separate executable.\n";
                break;
            default:
                ASSERT_not_reachable("invalid yices linkage");
        }
    }
}
#endif

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
    using namespace InstructionSemantics2;

    BaseSemantics::RiscOperatorsPtr ops = SymbolicSemantics::RiscOperators::instance(SymbolicSemantics::SValue::instance(),
                                                                                     SmtSolverPtr());

    BaseSemantics::SValuePtr a1 = ops->undefined_(32);                 // v1
    BaseSemantics::SValuePtr a2 = ops->add(a1, ops->number_(32, 4));   // (add v1 4)
    BaseSemantics::SValuePtr a3 = ops->add(a2, ops->number_(32, 8));   // (add v1 12)
    BaseSemantics::SValuePtr a4 = ops->subtract(a3, a2);               // (add v1 12 (negate (add v1 4)))
    std::cout <<"(add v1 4 8 (negate (add v1 4))) = " <<*a4 <<"\n";
}

int
main() {
#if 0 // [Robb P. Matzke 2015-06-25]: cannot be tested automatically since Jenkins might not have Yices
    test_yices_linkage();
#endif
    test_add_simplifications();
    test_svalues();
}

#endif
