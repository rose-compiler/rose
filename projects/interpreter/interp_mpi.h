/* This interpreter overrides certain MPI functions.  It will be modified to use symbolic
   values. */

#ifndef _INTERP_MPI_H
#define _INTERP_MPI_H

#include <interp_core.h>

namespace Interp {
namespace mpi {

class MPI_InitValue : public BuiltinFunctionValue
   {
     public:
     MPI_InitValue(Position pos, StackFrameP owner) : BuiltinFunctionValue(pos, owner) {}

     std::string functionName() const;
     ValueP call(SgFunctionType *fnType, const std::vector<ValueP> &args) const;

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
