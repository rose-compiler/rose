
#ifndef _CTXREACHABILITYTRANSFER_H
#define _CTXREACHABILITYTRANSFER_H 1

#include "DFTransferFunctions.h"

namespace CodeThorn
{

struct CtxReachabilityTransfer : DFTransferFunctions 
{
  void transfer(Edge, Lattice&) ROSE_OVERRIDE
  {
    // empty - reachability is computed by propagation
  }
};

} // namespace CodeThorn

#endif /* _CTXREACHABILITYTRANSFER_H */

