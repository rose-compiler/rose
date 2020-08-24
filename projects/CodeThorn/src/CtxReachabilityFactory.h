
#ifndef _CTXREACHABILITYFACTORY_H
#define _CTXREACHABILITYFACTORY_H 1

#include "PropertyStateFactory.h"
#include "CtxReachabilityLattice.h"

namespace CodeThorn
{

struct CtxReachabilityFactory : PropertyStateFactory {
  CtxReachabilityLattice*
  create() ROSE_OVERRIDE
  {
    return new CtxReachabilityLattice;
  }
};

} // namespace CodeThorn

#endif /* _CTXREACHABILITYFACTORY_H */

