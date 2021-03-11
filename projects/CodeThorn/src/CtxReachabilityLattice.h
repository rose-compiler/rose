

#ifndef _CTXREACHABILITYLATTICE_H
#define _CTXREACHABILITYLATTICE_H 1

#include "PropertyState.h"

namespace CodeThorn
{

// \note if the reachability lattice is used in conjunction with CtxLattice,
//       its design could be simplified. A non-existing context is not reachable,
//       an existing context has a lattice (so the flag reachable is actually not needed).
struct CtxReachabilityLattice : Lattice 
{
    /// tests if @ref other over-approximates this lattice
    bool approximatedBy(Lattice& other) const ROSE_OVERRIDE;
    
    /// merges @ref other into this 
    void combine(Lattice& other) ROSE_OVERRIDE;
    
    /// tests if this lattice represents bot
    bool isBot() const ROSE_OVERRIDE;
    
    /// prints a representation of this lattice on stream @ref os
    /// \param os  the output stream
    /// \param vim a variable ID mapping that gives names to Variable-ids
    void toStream(std::ostream& os, VariableIdMapping* vim) ROSE_OVERRIDE;
        
    //
    // setters and getters
    
    void setReachable()      { reachable = true; }
    bool isReachable() const { return reachable; }

  private:
    bool reachable = false;
};

} // namespace CodeThorn

#endif /* _CTXREACHABILITYLATTICE_H */
