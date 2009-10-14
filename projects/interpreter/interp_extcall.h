/* The external calling interpreter uses the libffi library to call real implementations
   of undefined functions, so long as they are in shared libraries. */

#ifndef _INTERP_EXTERN_H
#define _INTERP_EXTERN_H

#include <interp_core.h>

namespace Interp {
namespace extcall {

class ExternalCallingFunctionValue : public BuiltinFunctionValue
   {
     std::string fnName;
     void (*fnPtr)();

     public:
     ExternalCallingFunctionValue(const std::string &fnName, void (*fnPtr)(), Position pos, StackFrameP owner) : BuiltinFunctionValue(pos, owner), fnName(fnName), fnPtr(fnPtr) {}

     std::string functionName() const;
     ValueP call(SgFunctionType *fnType, const std::vector<ValueP> &args) const;

   };

class ExternalCallingStackFrame : public StackFrame
   {
     protected:
          ValueP externEvalFunctionRefExp(SgFunctionSymbol *sym);

     public:
          const std::vector<void *> &sharedLibraries;

          ExternalCallingStackFrame(const std::vector<void *> &sharedLibraries, Interpretation *currentInterp, SgFunctionSymbol *funSym, ValueP thisBinding = ValueP()) : StackFrame(currentInterp, funSym, thisBinding), sharedLibraries(sharedLibraries) {}

          StackFrameP newStackFrame(SgFunctionSymbol *funSym, ValueP thisBinding);
          ValueP evalFunctionRefExp(SgFunctionSymbol *sym);
   };

std::vector<void *> buildLibraryList(SgProject *prj);

}
}

#endif
