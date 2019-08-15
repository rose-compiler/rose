#ifndef NORMALIZATION_INLINER_H
#define NORMALIZATION_INLINER_H

#include "sage3basic.h"

#include "RoseAst.h"
#include "SgNodeHelper.h"
#include "inliner.h"
#include "CFAnalysis.h"
#include <list>

using namespace std;
using namespace Rose;

namespace CodeThorn {
  class InlinerBase {
  public:
    virtual void inlineFunctions(SgNode* root)=0;
    int getNumInlinedFunctions();
    virtual ~InlinerBase();
  protected:
    int _numInlinedFunctions=0;
  };

  class NormalizationInliner : public InlinerBase {
  public:
    void inlineFunctions(SgNode* root) override;
    ~NormalizationInliner();
  };

  class RoseInliner : public InlinerBase {
  public:
    void inlineFunctions(SgNode* root) override;
    // default value, can be overwritten
    int inlineDepth=10; 
  private:
    // Finds needle in haystack and returns true if found.  Needle is a single node (possibly an invalid pointer and will not be
    // dereferenced) and haystack is the root of an abstract syntax (sub)tree.
    static bool isAstContaining(SgNode *haystack, SgNode *needle);
  };

}

#endif
