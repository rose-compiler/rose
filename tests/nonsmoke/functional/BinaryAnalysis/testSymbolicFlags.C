// Demonstrates and tests user-defined flag bits for symbolic expressions.
//
// Note: We always provide a comment for variables so that the test output is stable.  Internally, variables are always named
// with a letter ("v" or "m") followed by an automatically generated unique number. If a new variable is created in the middle
// of the test then all subsequent variables will have a different number and it would be more difficult to verify that the
// remaining output is correct.  The default expression printer will print a non-empty comment in lieu of the variable name.
#include "conditionalDisable.h"
#ifdef ROSE_BINARY_TEST_DISABLED
#include <iostream>
int main() { std::cout <<"disabled for " <<ROSE_BINARY_TEST_DISABLED <<"\n"; return 1; }
#else

#include <rose.h>
#include <BinarySymbolicExpr.h>

using namespace Rose::BinaryAnalysis;

// Bit flags (low-order 16 bits are reserved for ROSE, so don't use them)
static const unsigned UNDEFINED = 0x00010000;
static const unsigned INVALID   = 0x00020000;

// Make sure that the bits can be set and stick.
static void
testSetting() {
    std::cout <<"test setting of flags:\n";

    // The default is that no flags are set
    SymbolicExpr::Ptr e1 = SymbolicExpr::makeIntegerVariable(32, "e1");
    ASSERT_always_require(e1->flags() == 0);

    // Flags are always specified after a comment string
    SymbolicExpr::Ptr e2 = SymbolicExpr::makeIntegerVariable(32, "e2", UNDEFINED);
    ASSERT_always_require(e2->flags() == UNDEFINED);

    // Flags can be specified for things other than variables
    SymbolicExpr::Ptr e3 = SymbolicExpr::makeIntegerConstant(32, 0, "e3", INVALID);
    ASSERT_always_require(e3->flags() == INVALID);
    SymbolicExpr::Ptr e4 = SymbolicExpr::makeIntegerConstant(Sawyer::Container::BitVector(128), "e4", UNDEFINED|INVALID);
    ASSERT_always_require(e4->flags() == (UNDEFINED|INVALID));
    SymbolicExpr::Ptr e5 = SymbolicExpr::makeBooleanConstant(true, "e5", UNDEFINED);
    ASSERT_always_require(e5->flags() == UNDEFINED);
    SymbolicExpr::Ptr e6 = SymbolicExpr::makeMemoryVariable(32, 32, "e6", INVALID);
    ASSERT_always_require(e6->flags() == INVALID);
}

// Flags for internal nodes are the union of flags of subtrees
static void
testInternal() {
    std::cout <<"test internal internal nodes:\n";
    SymbolicExpr::Ptr e1 = SymbolicExpr::makeIntegerVariable(32, "e1", UNDEFINED);
    SymbolicExpr::Ptr e2 = SymbolicExpr::makeIntegerVariable(32, "e2", INVALID);
    SymbolicExpr::Ptr e3 = SymbolicExpr::Interior::instance(SymbolicExpr::OP_ADD, e1, e2);
    ASSERT_always_require(e3->flags() == (UNDEFINED|INVALID));
}

// Make sure we can print flags
static void
testPrinting() {
    std::cout <<"test printing flags:\n";
    SymbolicExpr::Ptr b = SymbolicExpr::makeIntegerVariable(32, "b", INVALID);
    SymbolicExpr::Ptr a = SymbolicExpr::makeIntegerVariable(32, "a", UNDEFINED);
    SymbolicExpr::Ptr e1 = SymbolicExpr::Interior::instance(SymbolicExpr::OP_ADD, a, b);
    std::cout <<"  e1 = " <<*e1 <<"\n";
}

// When simplifying, if subtrees are discarded (e.g., cancellation) then they don't contribute their flags to the result.
static void
testDiscardRule() {
    std::cout <<"test simplification discard rule:\n";
    SymbolicExpr::Ptr e1 = SymbolicExpr::makeIntegerVariable(32, "e1", UNDEFINED);
    SymbolicExpr::Ptr e2 = SymbolicExpr::Interior::instance(SymbolicExpr::OP_NEGATE, e1);
    SymbolicExpr::Ptr e3 = SymbolicExpr::makeIntegerVariable(32, "e3", INVALID);
    SymbolicExpr::Ptr e4 = SymbolicExpr::Interior::instance(SymbolicExpr::OP_ADD, e1, e2, e3);
    std::cout <<"  e4 = " <<*e4 <<"\n";
    ASSERT_always_require(e4->flags() == INVALID);

}

// Any new tree created by the simplifier will have zero flags if that new tree doesn't depend on any original trees.
static void
testNewExprRule() {
    std::cout <<"test simplification new expression rule:\n";
    SymbolicExpr::Ptr e1 = SymbolicExpr::makeIntegerVariable(32, "e1", UNDEFINED);
    SymbolicExpr::Ptr e2 = SymbolicExpr::Interior::instance(SymbolicExpr::OP_ADD, e1,
                                                            SymbolicExpr::Interior::instance(SymbolicExpr::OP_NEGATE, e1,
                                                                                             SmtSolverPtr()),
                                                            SmtSolverPtr());
    std::cout <<"  e2 = " <<*e2 <<"\n";
    ASSERT_always_require(e2->flags() == 0);

    SymbolicExpr::Ptr e3 = SymbolicExpr::Interior::instance(SymbolicExpr::OP_ADD, e1,
                                                            SymbolicExpr::Interior::instance(SymbolicExpr::OP_INVERT, e1,
                                                                                             SmtSolverPtr()),
                                                            SmtSolverPtr());
    std::cout <<"  e3 = " <<*e3 <<"\n";
    ASSERT_always_require(e3->flags() == 0);

    SymbolicExpr::Ptr e4 = SymbolicExpr::Interior::instance(SymbolicExpr::OP_XOR, e1, e1);
    std::cout <<"  e4 = " <<*e4 <<"\n";
    ASSERT_always_require(e4->flags() == 0);
}

// Any new subtree created by a simplification somehow combining input subtrees will have flags that are the union of the flags
// forom the input subtrees on which it depends.
static void
testFoldingRule() {
    std::cout <<"test simplification folding rule:\n";
    SymbolicExpr::Ptr e1 = SymbolicExpr::makeIntegerConstant(32, 7, "e1", UNDEFINED);
    SymbolicExpr::Ptr e2 = SymbolicExpr::makeIntegerConstant(32, 8, "e2", INVALID);
    SymbolicExpr::Ptr e3 = SymbolicExpr::Interior::instance(SymbolicExpr::OP_ADD, e1, e2);
    std::cout <<"  e3 = " <<*e3 <<"\n";
    ASSERT_always_require(e3->flags() == (UNDEFINED|INVALID));
}

// User-defined flags are significant for hashing.
static void
testHashing() {
    std::cout <<"test hashing:\n";
    SymbolicExpr::Ptr e1 = SymbolicExpr::makeIntegerConstant(32, 7, "", 0);
    SymbolicExpr::Ptr e2 = SymbolicExpr::makeIntegerConstant(32, 7, "", UNDEFINED);
    ASSERT_always_require(e1->hash() != e2->hash());
}

// Simplification of relational operators is treated like folding. I.e., the flags of the result are the union of the flags of
// the operands on which the result depends.
static void
testRelationalFolding() {
    std::cout <<"test relational folding:\n";
    SymbolicExpr::Ptr e1 = SymbolicExpr::makeIntegerConstant(32, 7, "e1", UNDEFINED);
    SymbolicExpr::Ptr e2 = SymbolicExpr::makeIntegerConstant(32, 8, "e2", INVALID);

    SymbolicExpr::Ptr e3 = SymbolicExpr::Interior::instance(SymbolicExpr::OP_ULT, e1, e2);
    std::cout <<"  e3 = " <<*e3 <<"\n";
    ASSERT_always_require(e3->flags() == (UNDEFINED|INVALID));

    SymbolicExpr::Ptr e4 = SymbolicExpr::Interior::instance(SymbolicExpr::OP_ULT, e2, e1);
    std::cout <<"  e4 = " <<*e4 <<"\n";
    ASSERT_always_require(e4->flags() == (UNDEFINED|INVALID));

    SymbolicExpr::Ptr e5 = SymbolicExpr::makeIntegerVariable(32, "e5", UNDEFINED);
    SymbolicExpr::Ptr e6 = SymbolicExpr::Interior::instance(SymbolicExpr::OP_EQ, e5, e5);
    std::cout <<"  e6 = " <<*e6 <<"\n";
    ASSERT_always_require(e6->flags() == UNDEFINED);
}

int
main() {
    testSetting();
    testInternal();
    testPrinting();
    testDiscardRule();
    testNewExprRule();
    testFoldingRule();
    testHashing();
    testRelationalFolding();
}

#endif
