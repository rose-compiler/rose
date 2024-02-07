#ifndef __CXX_SDGLIBRARY_EXTENDER
#define __CXX_SDGLIBRARY_EXTENDER
// #include "rose.h"
#include "DependenceGraph.h"
class SystemDependenceGraph;
class SDGLibraryExtender
{
  public:
  virtual bool isKnownLibraryFunction(SgFunctionDeclaration*) {
      return false;
  }
  virtual void createConnectionsForLibaryFunction(SgFunctionDeclaration*, SystemDependenceGraph*) {}
  virtual ~SDGLibraryExtender() {}
};
#endif
