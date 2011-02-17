#ifndef EXPRESSIONTREEEQUAL_H
#define EXPRESSIONTREEEQUAL_H



// DQ (3/21/2006): Added namespace to separate the 
// PRE specific work (and it's use of the type named 
// ControlFlowGraph which conflicts with the OTHER
// ControlFlowGraph in:
//    src/midend/programAnalysis/dominatorTreesAndDominanceFrontiers
// namespace PRE {

//! Are two expressions equal (using a deep comparison)?
bool expressionTreeEqual(SgExpression*, SgExpression*);

//! Are corresponding expressions in two lists equal (using a deep comparison)?
bool expressionTreeEqualStar(const SgExpressionPtrList&,
                             const SgExpressionPtrList&);

// closing scope of namespace: PRE
// }

#endif // EXPRESSIONTREEEQUAL_H
