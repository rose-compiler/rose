#include <rose.h>
#include <Rose/BinaryAnalysis/SymbolicExpression.h>
#include <Rose/BinaryAnalysis/Z3Solver.h>
#include <Sawyer/Stopwatch.h>

using namespace Rose::BinaryAnalysis;

// Build an expression with lots of common subexpressions. This is a tiny
// bit tricky because we need to build something that ROSE cannot simplify.
SymbolicExpression::Ptr highSharing(size_t depth) {
    SymbolicExpression::Ptr retval = SymbolicExpression::makeIntegerVariable(32);
    for (size_t i=0; i<depth; ++i) {
        if (i % 2) {
            retval = SymbolicExpression::makeAdd(retval, retval);
        } else {
            retval = SymbolicExpression::makeRol(retval, retval);
        }
    }
    return retval;
}

int main() {
    // Build an initial large expression
    std::cerr <<"building expression with high degree of common subexpressions...\n";
    Sawyer::Stopwatch timer;
    SymbolicExpression::Ptr e1 = highSharing(30);
    std::cerr <<"  time to build expression:    " <<timer <<"\n";
    std::cerr <<"  logical size of expression:  " <<e1->nNodes() <<" nodes\n";
    std::cerr <<"  physical size of expression: " <<e1->nNodesUnique() <<" nodes\n";
    std::cerr <<"  number of variables:         " <<e1->getVariables().size() <<"\n";
    std::cerr <<"  expression hash:             " <<e1->hash() <<"\n";

    // Rename the variable everywhere it occurs
    std::cerr <<"renaming variables...\n";
    SymbolicExpression::ExprExprHashMap index;
    size_t nextId = 100;
    timer.restart();
    SymbolicExpression::Ptr e2 = e1->renameVariables(index /*in,out*/, nextId /*in,out*/);
    std::cerr <<"  time to rename variables:    " <<timer <<"\n";
    std::cerr <<"  logical size of expression:  " <<e2->nNodes() <<" nodes\n";
    std::cerr <<"  physical size of expression: " <<e2->nNodesUnique() <<" nodes\n";
    std::cerr <<"  number of variables:         " <<e2->getVariables().size() <<"\n";
    std::cerr <<"  expression hash:             " <<e2->hash() <<"\n";
    
    // Substitute one variable for another
    std::cerr <<"performing variable substitution...\n";
    SymbolicExpression::Ptr oldVar = *e1->getVariables().begin();
    SymbolicExpression::Ptr newVar = SymbolicExpression::makeIntegerVariable(oldVar->nBits());
    timer.restart();
    SymbolicExpression::Ptr e3 = e1->substitute(oldVar, newVar);
    std::cerr <<"  time to substitute:          " <<timer <<"\n";
    std::cerr <<"  old variable:                " <<*oldVar <<"\n";
    std::cerr <<"  new variable:                " <<*newVar <<"\n";
    std::cerr <<"  logical size of expression:  " <<e3->nNodes() <<" nodes\n";
    std::cerr <<"  physical size of expression: " <<e3->nNodesUnique() <<" nodes\n";
    std::cerr <<"  number of variables:         " <<e3->getVariables().size() <<"\n";
    std::cerr <<"  expression hash:             " <<e3->hash() <<"\n";

    // Perform multiple substitutions at once (although we just provide one)
    std::cerr <<"performing multi-substitution...\n";
    SymbolicExpression::ExprExprHashMap substitutions;
    substitutions.insert(std::make_pair(oldVar, newVar)); 
    timer.restart();
    SymbolicExpression::Ptr e4 = e1->substituteMultiple(substitutions);
    std::cerr <<"  time to substitute:          " <<timer <<"\n";
    std::cerr <<"  old variable:                " <<*oldVar <<"\n";
    std::cerr <<"  new variable:                " <<*newVar <<"\n";
    std::cerr <<"  logical size of expression:  " <<e4->nNodes() <<" nodes\n";
    std::cerr <<"  physical size of expression: " <<e4->nNodesUnique() <<" nodes\n";
    std::cerr <<"  number of variables:         " <<e4->getVariables().size() <<"\n";
    std::cerr <<"  expression hash:             " <<e4->hash() <<"\n";
}
