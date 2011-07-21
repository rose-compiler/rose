#ifndef STMT_DEP_ANAL_H
#define STMT_DEP_ANAL_H

#include <SymbolicVal.h>

class DepRel;
class DepInfo;
class DomainCond;

template <class Result>
void ComputeLoopDep( const AstNodePtr& exp, 
                     const DepRel& rel, Result& r);

template <class VarVec, class CoeffVec>
SymbolicVal DecomposeAffineExpression( const SymbolicVal& exp, 
                 const VarVec& vars, CoeffVec& vec, int size);

template <class Collect>
int SetDepDirection( DepInfo &edd, int commLevel, Collect &result);

template <class CoeffVec, class BoundVec,class BoundOp>
bool SplitEquation( CoeffVec& cur, 
                      const SymbolicVal& cut, const BoundVec& bounds, 
                      BoundOp& boundop, CoeffVec& split);

template <class Mat>
bool NormalizeMatrix( Mat& analMatrix, int rows, int cols);

template <class CoeffVec, class BoundVec, class BoundOp, class Dep>
bool AnalyzeEquation(const CoeffVec& vec, const BoundVec& bounds,
                        BoundOp& boundop, Dep& result, const DepRel& rel) ;

#include <StmtDepAnal.C>
#endif
