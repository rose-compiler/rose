//using namespace std;

#ifndef __DFAFilter_HXX_LOADED__
#define __DFAFilter_HXX_LOADED__

using namespace VirtualCFG;


struct IsDFAFilter
{
  bool operator() (CFGNode cfgn) const
  {
    SgNode *n = cfgn.getNode();
    //    return true;
    // get rid of all beginning nodes
    if (!cfgn.isInteresting() && !(isSgFunctionCallExp(cfgn.getNode()) && cfgn.getIndex() >= 2))
      return false;
    if (isSgInitializedName(n) && cfgn.getIndex()>0)
      //if (isSgInitializedName(n) && cfgn==n->cfgForEnd())
      return false;
    //    if (cfgn.getIndex()>0)
    //  return false;

    /*
    SgNode *n = cfgn.getNode();
    if (isSgVarRefExp(n))
      return true;
    if (isSgInitializedName(n) && cfgn==n->cfgForEnd())
      //if (isSgInitializedName(n))
      return true;
    */
    return true;
  }
};

#endif
