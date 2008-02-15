#ifndef STMT_DEP_ANAL_H
#define STMT_DEP_ANAL_H

#include <SymbolicVal.h>

class LoopTransformInterface;
class AstNodePtr;
class DepRel;
class DepInfo;
class DomainCond;

template <class Result>
void ComputeLoopDep( LoopTransformInterface &fa, const AstNodePtr& exp, 
                     const DepRel& rel, Result& r);

template <class VarVec, class CoeffVec>
SymbolicVal DecomposeAffineExpression(LoopTransformInterface& la, 
            const SymbolicVal& exp, const VarVec& vars, CoeffVec& vec, int size);

template <class Collect>
int SetDepDirection( DepInfo &edd, int commLevel, Collect &result);

template <class CoeffVec, class BoundVec,class BoundOp>
// Boolean SplitEquation( LoopTransformInterface& la, CoeffVec& cur, 
int SplitEquation( LoopTransformInterface& la, CoeffVec& cur, 
                      const SymbolicVal& cut, const BoundVec& bounds, 
                      BoundOp& boundop, CoeffVec& split);

template <class Mat>
// Boolean NormalizeMatrix( Mat& analMatrix, int rows, int cols);
int NormalizeMatrix( Mat& analMatrix, int rows, int cols);

template <class CoeffVec, class BoundVec, class BoundOp, class Dep>
// Boolean AnalyzeEquation(const CoeffVec& vec, const BoundVec& bounds,
int AnalyzeEquation(const CoeffVec& vec, const BoundVec& bounds,
                        BoundOp& boundop, Dep& result, const DepRel& rel) ;

#include <StmtDepAnal.C>
#endif
