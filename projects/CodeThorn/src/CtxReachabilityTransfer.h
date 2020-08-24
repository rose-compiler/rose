
#ifndef _CTXREACHABILITYTRANSFER_H
#define _CTXREACHABILITYTRANSFER_H 1

#include "DFTransferFunctions.h"
#include "CtxReachabilityLattice.h"

namespace CodeThorn
{

struct CtxReachabilityTransfer : DFTransferFunctions 
{
  void transfer(Edge, Lattice& element) ROSE_OVERRIDE
  {
    dynamic_cast<CtxReachabilityLattice&>(element).setReachable();
  }
  
  void initializeExtremalValue(Lattice& element)
  {
    dynamic_cast<CtxReachabilityLattice&>(element).setReachable();
  }
};

} // namespace CodeThorn

#endif /* _CTXREACHABILITYTRANSFER_H */

