// This test demonstrated a bug (it is now fixed) which
// forces the setting of the scope of a class declaration 
// within the astPostProcessing phase (I could not do it 
// earlier!).  Normally and where possible we fixup the scopes
// in the EDG/SageIII translation directly. However in the
// case below, we don't see the namespace "MyCFG" until
// after a number of referenced STL types are processed.
// Internally we keep a flag for if the scope was purposefully
// left unset and we use this flag so that we can track errors
// (distinguish when the scope is NULL because we could not 
// set it vs. if it was mistakenly not set.

#include<string>
#include<vector>

namespace MyCFG {

  class CFGEdge {};

  class CFGPath {
    std::vector<CFGEdge> edges;
    public:
#if 1
 // Either of these line cause the error: STL type names are in the
 // scope of MyCFG and referenced before we visit the MyCFG namespace.
 // CFGPath(CFGEdge e): edges(1, e) {}
    CFGPath(): edges() {}
#else
    CFGPath(CFGEdge e) {}
#endif
  };

}

