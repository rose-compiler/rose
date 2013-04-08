#ifndef PRE_H
#define PRE_H

#include <stdint.h>
#include <queue>
#include <fstream>
#include "replaceExpressionWithStatement.h"
#include "inlinerSupport.h"
#include "expressionTreeEqual.h"
#include "controlFlowGraph.h"

// DQ (3/21/2006): Moved this outside of PRE namespace since it is also used in 
// the finiteDifferencing transformation.
//! Are any variables in syms modified anywhere within n, or is n a declaration
//! of one of them?
bool anyOfListPotentiallyModifiedIn(const std::vector<SgVariableSymbol*>& syms, SgNode* n);

//! Is the expression expr computed within root?  This test is simple, and does
//! not account for changes to the variables in expr.
bool expressionComputedIn(SgExpression* expr, SgNode* root);

//! Count how many computations of expr are within root.  Same caveats as
//! expressionComputedIn.
int countComputationsOfExpressionIn(SgExpression* expr, SgNode* root);

// DQ (3/21/2006): Added namespace to separate the 
// PRE specific work (and it's use of the type named 
// ControlFlowGraph which conflicts with the OTHER
// ControlFlowGraph in:
//    src/midend/programAnalysis/dominatorTreesAndDominanceFrontiers
namespace PRE {

//! Get the function definition containing a given node
SgFunctionDefinition* getFunctionDefinition(SgNode* n);

//! Do partial redundancy elimination, looking for copies of one expression expr
//! within the basic block root.  A control flow graph for root must be provided
//! in cfg, with a map from nodes to their statements in node_statements, a map
//! from edges to their CFG edge types in edge_type, and a map from edges to
//! their insertion points in edge_insertion_point.  The algorithm used is that
//! of Paleri, Srikant, and Shankar ("Partial redundancy elimination: a simple,
//! pragmatic, and provably correct algorithm", Science of Computer Programming
//! 48 (2003) 1--20).
void partialRedundancyEliminationOne(
    SgExpression* expr, 
    SgBasicBlock* root,
    const myControlFlowGraph& cfg);

//! Do partial redundancy for all expressions within a given function, whose
//! definition is given in n.
void partialRedundancyEliminationFunction(SgFunctionDefinition* n);

//! Do partial redundancy elimination on all functions within the scope n.
void ROSE_DLL_API partialRedundancyElimination(SgNode* n);

// closing scope of namespace: PRE
}

#endif // PRE_H
