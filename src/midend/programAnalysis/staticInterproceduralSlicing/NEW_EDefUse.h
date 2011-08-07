#ifndef EDEFUSE_CLASS
#define EDEFUSE_CLASS
// #include "rose.h"
#include "DFAnalysis.h"
#include "DefUseAnalysis.h"
#include <vector>
class EDefUse
{
public:
  EDefUse(SgProject * proj);
protected:
  DefUseAnalysis * internalDefUse;
public:
      int run(bool debug);
      // get the vector of defining and usage nodes for a specific node and a initializedName
      std::vector < SgNode* > getDefFor(SgNode* node, SgInitializedName* initName);
      std::vector < SgNode* > getUseFor(SgNode* node, SgInitializedName* initName);
      std::vector < std::pair < SgInitializedName* , SgNode* > >  getDefMultiMapFor(SgNode* node);
      // return whether a node is a global node
      bool isNodeGlobalVariable(SgInitializedName* node);


      void printDefUse();
      //
};
#endif
