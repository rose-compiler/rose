#ifndef DFABSTRACTSOLVER_H
#define DFABSTRACTSOLVER_H

#include "Labeler.h"

namespace CodeThorn 
{
  
struct Lattice;  

/// Defines the abstract solver interface 
struct DFAbstractSolver 
{
  /// runs the solver
  virtual void runSolver() = 0;
  
  /// computes the preinfo at @ref lab / @ref inInfo  
  virtual void computeCombinedPreInfo(Label lab, Lattice& inInfo) = 0;
  
  /// currently only internally used, so not a member of the public interface
  //~ virtual void computePostInfo(Label lab, Lattice& inInfo) = 0;

  /// optional debugging/tracing support
  virtual void setTrace(bool trace) = 0; 
};

} // namespace CodeThorn

#endif /* DFABSTRACTSOLVER_H */
