
#include "CtxReachabilityLattice.h"

namespace CodeThorn
{

bool CtxReachabilityLattice::approximatedBy(Lattice& other) const 
{
  CtxReachabilityLattice& that = dynamic_cast<CtxReachabilityLattice&>(other);
  
  return !isReachable() || that.isReachable(); 
}

void CtxReachabilityLattice::combine(Lattice& other) 
{
  CtxReachabilityLattice& that = dynamic_cast<CtxReachabilityLattice&>(other);
  
  reachable = isReachable() || that.isReachable();
}
    
bool CtxReachabilityLattice::isBot() const 
{
  return isReachable();
}
    
void CtxReachabilityLattice::toStream(std::ostream& os, VariableIdMapping* vim) 
{
  if (!isReachable()) os << '!';
  os << "reachable";
}

}
