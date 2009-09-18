/* This interpreter overrides certain MPI functions.  It will be modified to use symbolic
   values. */

#ifndef _INTERP_MPI_H
#define _INTERP_MPI_H

#include <interp_core.h>

namespace Interp {
namespace mpi {

class MPI_InitValue : public Value
   {
     public:
     MPI_InitValue(Position pos, StackFrameP owner) : Value(pos, owner, true) {}

     std::string show() const;
     ValueP call(SgFunctionType *fnType, const std::vector<ValueP> &args) const;
     ValueP primAssign(const_ValueP rhs, SgType *lhsApt, SgType *rhsApt);
     size_t forwardValidity() const;

   };

class MPIStackFrame : public StackFrame
   {
     public:

          MPIStackFrame(Interpretation *currentInterp, SgFunctionSymbol *funSym, ValueP thisBinding = ValueP()) : StackFrame(currentInterp, funSym, thisBinding) {}

          StackFrameP newStackFrame(SgFunctionSymbol *funSym, ValueP thisBinding);
          ValueP evalFunctionRefExp(SgFunctionSymbol *sym);

   };

}
}

#endif
