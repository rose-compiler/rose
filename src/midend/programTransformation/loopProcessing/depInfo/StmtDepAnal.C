#include <vector>
#include <iostream>
#include <stdlib.h>
#include <FunctionObject.h>
#include <DepRel.h>
#include <SymbolicVal.h>
#include <DomainInfo.h>

#include <LoopTransformInterface.h>
#include "RoseAsserts.h" /* JFR: Added 17Jun2020 */

extern bool DebugDep();

// 2*i+1 input exp,  loop variable i, size 1 loop nest
// the decomposed coeffecient vector is 2
// return 1 as the left term.
template <class VarVec, class CoeffVec>
SymbolicVal DecomposeAffineExpression(
            const SymbolicVal& exp, const VarVec& vars, CoeffVec& vec, int size)
  {
    // AstInterface& fa = la;
    SymbolicVal val = exp;
    // int coeff;
    for (int i = 0; i < size; ++i) {
       SymbolicVar ivar = vars[i];
       SymbolicBound ivarbound;
       SymbolicVal coeff = UnwrapVarCond( SymbolicCond( REL_LE, val, 0), ivar, ivarbound);
       if (coeff.IsNIL())
          return SymbolicVal();
       if (!(coeff == 0)) {
          if (!ivarbound.ub.IsNIL())
             val = -ivarbound.ub;
          else {
             val = ivarbound.lb;
             coeff = -coeff;
          }
       }
       vec.push_back(coeff);
    }
    return val;
  }
// Coefficient vector, current coefficient, bounds vector, GetBoundOperator, Split coefficient vector
template <class CoeffVec, class BoundVec, class BoundOp>
bool SplitEquation( CoeffVec& cur,
                      const SymbolicVal& cut, const BoundVec& bounds,
                      BoundOp& boundop, CoeffVec& split)
 {
     int dim = cur.size()-1;
     SymbolicVal leftval = cur[dim]; // obtain the last coefficient, which is right side terms without using loop index variable
     if (leftval != 0) {
       CompareRel r1 = CompareVal(leftval,-cut, &boundop);
       CompareRel r2 = CompareVal(leftval,cut, &boundop);
       bool lt = ((r1 & REL_GT) && (r2 & REL_LT)) || ((r1 & REL_LT) && (r2 & REL_GT));
       if (!lt) { // relation of r1 and r2 must be reversed pair, or error
         if (DebugDep())
           std::cerr << "unable to split because " << leftval.toString() << " ? " << cut.toString() << std::endl;
         return false;
       }
     }

     bool succ = false;
     split.clear();
     int j =0;
     for (; j < dim; ++j) {
        SymbolicVal left = cur[j] / cut;
        if (HasFraction(left))
            split.push_back(0);
        else {
            split.push_back(left);
            succ = true;
        }
     }
     split.push_back(0); // right-hand side value
     if (succ) {
        SymbolicVal left = 0;
        for (j = 0; j < dim; ++j) {
           if (split[j]== 0)
             switch (CompareVal(cur[j],0,&boundop)) {
              case REL_LE:
                 left = left + cur[j] * bounds[j].lb; break;
              case REL_GE:
                 left = left + cur[j] * bounds[j].ub; break;
              default: break;
             }
        }
        if (j == dim && (left == 0 || (CompareVal(left,cut) & REL_LT)))  {
          for (j = 0; j < dim; ++j) {
             if (split[j] != 0)
                cur[j] = 0; // clear some coefficency values
          }

          return true;
        }
        else if (DebugDep()) {
            if (j == dim)
               std::cerr << "unable to decide left " << left.toString() << " ? " << cut.toString() << std::endl;
            else
               std::cerr << "unable to decide cur[" << j << "] ? 0\n";
        }
     }
     split.clear();
     return false;
}

template <class Mat>
bool NormalizeMatrix( Mat& analMatrix, int rows, int cols)
  {
    int k;
    for ( k = 0 ; k < rows && k < cols; k++) {
       int pti, pt = -1, ptnum = -1;
       for ( pti = k ; pti < rows; ++pti) {
         if (analMatrix[pti][k] != 0) {
            int ptnum1 = 0;
            for (int j = k; j < cols; ++j) {
               if (analMatrix[pti][j].GetValType() != VAL_CONST)
                 ++ptnum1;
            }
            if (ptnum < 0 || ptnum > ptnum1) {
              pt = pti;
              ptnum = ptnum1;
            }
         }
       }
       if (pt < 0)
         continue;
       if (pt != k) {
         for (int i = k; i < cols; i++) {
           SymbolicVal t = analMatrix[k][i];
           analMatrix[k][i] = analMatrix[pt][i];
           analMatrix[pt][i] = t;
         }
       }
       for (int i = k+1; i < rows; i++) {
         SymbolicVal f1 = analMatrix[i][k], f2 = analMatrix[k][k];
         if (f1 == 0) continue;
         for (int j = k; j < cols; j++) {
            analMatrix[i][j] = analMatrix[i][j] * f2 - analMatrix[k][j] * f1;
         }
         assert(analMatrix[i][k] == 0);
       }
    }
    for ( k-- ; k >= 0; k--) {
      if (analMatrix[k][k] == 0)
        continue;
      for (int i = k-1; i >= 0; i--) {
        SymbolicVal f1 = analMatrix[i][k], f2 = analMatrix[k][k];
        if (f1 == 0) continue;
        for (int j = i; j < cols; j++) {
          analMatrix[i][j] = analMatrix[i][j] * f2 - analMatrix[k][j] * f1;
        }
        assert(analMatrix[i][k] == 0);
      }
    }
    if (rows >= cols && analMatrix[cols-1][cols-1] != 0)
        return false;
    return true;
  }

template <class Collect>
int SetDepDirection( DepInfo &edd, int commLevel, Collect &result)
    {
      if (commLevel < 0)
          return commLevel;
      DepRel eq(DEPDIR_EQ, 0), lt(DEPDIR_LE, -1);
      int i;
      for ( i = 0; i < commLevel; i++) {
        DepRel e1 = edd.Entry(i,i) & lt;
        if (DebugDep()) {
           std::cerr << "at common loop level " << i << ":" << e1.toString() << "\n";
        }
        if (!e1.IsTop()) {
          DepInfo edd1( edd);
          edd1.Entry( i,i) = e1;
          assert(!edd1.IsTop());
          result(edd1);
        }
        DepRel &e2 = edd.Entry(i,i);
        e2  &= eq;
        if (e2.IsTop())
          return i;
      }
      return i+1;
    }

//extern DepTestStatistics DepStats;

template <class CoeffVec, class BoundVec, class BoundOp, class Dep>
bool AnalyzeEquation(const CoeffVec& vec, const BoundVec& bounds,
                        BoundOp& boundop,
                        Dep& result, const DepRel& rel)
{
  int dim = vec.size()- 1;
  std::vector<int> signs;
  for (int index = 0; index < dim; ++index) {
    if  (vec[index]==0) {
        signs.push_back(0);
        continue;
    }
    SymbolicBound cb = GetValBound(vec[index], boundop);
    assert(!cb.lb.IsNIL() && !cb.ub.IsNIL());
    const SymbolicBound& b = bounds[index];
    assert(!b.lb.IsNIL() && !b.ub.IsNIL());
    if (b.lb >= 0) {
       if (cb.lb >= 0)
          signs.push_back(1);
       else if (cb.ub <= 0)
          signs.push_back(-1);
       else {
         if (DebugDep())
           std::cerr << "unable to decide sign of coeff when lb >=0 for ivar[" << index << "]\n";
         //return false;
         signs.push_back(2);
       }
    }
    else if (b.ub <= 0) {
      if (cb.lb >= 0)
        signs.push_back(-1);
      else if (cb.ub <= 0)
        signs.push_back(1);
      else {
         if (DebugDep())
           std::cerr << "unable to decide sign of coeff when ub <=0 for ivar[" << index << "]\n";
        //return false;
        signs.push_back(2);
      }
    }
    else {
         if (DebugDep())
           std::cerr << "unable to decide sign of ivar[" << index << "]\n";
        //return false;
        signs.push_back(2);
    }
  }
  if (vec[dim] == 0)
      signs.push_back(0);
  else {
     SymbolicVal leftval = vec[dim];
     if (leftval.IsNIL()) {
        if (DebugDep())
           std::cerr << "unable to decide sign of leftval\n";
        return false;
     }
     SymbolicBound lb = GetValBound(vec[dim], boundop);
     if (lb.ub <= 0)
        signs.push_back(-1);
     else if (lb.lb >= 0)
        signs.push_back(1);
     else {
        if (DebugDep())
           std::cerr << "unable to decide sign of leftval\n";
        return false;
        //signs.push_back(2);
     }
  }
  for (int i = 0; i < dim ; ++i) {
    if (signs[i] == 0)
       continue;
    SymbolicVal coeff = vec[i];
    assert(!coeff.IsNIL());
    int j = 0;
    for ( j = i+1; j < dim; ++j) {
      if (signs[j] == 0 || coeff + vec[j] != 0)
        continue;
      int left = 0, k;
      for (k = 0; k < dim ; ++k) {
        if (k == i || k == j)
           continue;
        if (left == 0)
            left = signs[k];
        else if (signs[k] == 2 || signs[k] * left < 0)
           break;
      }
      if ( k < dim || left == 2 || left * signs[dim] < 0)
         continue;
      int diff = 0, c = 1;
      bool hasdiff = false;
      if (left == 0 && vec[dim].isConstInt(diff) &&
           (diff == 0 || coeff.isConstInt(c)))  {
        if (diff != 0 && c != 1) {
           int odiff = diff;
           diff = diff / c;
           if (odiff != diff * c)
                          {
                                        //DepStats.AddAdhocDV(DepStats.RoseToPlatoDV(DepRel(DEPDIR_NONE)));
                                        result[i][j] = DepRel(DEPDIR_NONE);
                                        return true;
           }
        }
        hasdiff = true;
      }
      if (hasdiff) {
                //DepStats.AddAdhocDV(DepStats.RoseToPlatoDV(DepRel(DEPDIR_EQ, diff)));
                result[i][j] = rel * DepRel(DEPDIR_EQ, diff);
                return true; // precise dependence
      }
      else if (signs[i] != 2) {
                if (signs[dim]* signs[i] > 0) {
                        //DepStats.AddAdhocDV(DepStats.RoseToPlatoDV(DepRel(DEPDIR_GE, diff)));
                        result[i][j] = rel * DepRel(DEPDIR_GE, diff);
                }
                else {
                        //DepStats.AddAdhocDV(DepStats.RoseToPlatoDV(DepRel(DEPDIR_LE, diff)));
                        result[i][j] = rel * DepRel(DEPDIR_LE, diff);
                }
      }
    }
  }
  return false;
}

