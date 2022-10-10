#include "NormalizationInliner.h"
#include "inliner.h"

namespace CodeThorn {

  InlinerBase::~InlinerBase() {
  }

  int InlinerBase::getNumInlinedFunctions() {
    return _numInlinedFunctions;
  }

  void NormalizationInliner::inlineFunctions(SgNode* root) {
    _numInlinedFunctions=0;
  }

  NormalizationInliner::~NormalizationInliner() {
  }

  void RoseInliner::inlineFunctions(SgNode* root) {
    // Inline one call at a time until all have been inlined.  Loops on recursive code.
    //SgProject* project=isSgProject(root);
    //ROSE_ASSERT(project);
    size_t nInlined = 0;
    for (int count=0; count<inlineDepth; ++count) {
      bool changed = false;
      auto functionCalls=SageInterface::querySubTree<SgFunctionCallExp>(root);
      BOOST_FOREACH(SgFunctionCallExp *call, functionCalls) {
        if (doInline(call)) {
          ASSERT_always_forbid2(isAstContaining(root, call),
                                "Inliner says it inlined, but the call expression is still present in the AST.");
          ++nInlined;
          changed = true;
          break;
        }
      }
      if (!changed)
        break;
    }
    _numInlinedFunctions=nInlined;
  }
  
  bool RoseInliner::isAstContaining(SgNode *haystack, SgNode *needle) {
    struct T1: AstSimpleProcessing {
      SgNode *needle;
      T1(SgNode *needle): needle(needle) {}
      void visit(SgNode *node) {
        if (node == needle)
          throw this;
      }
    } t1(needle);
    try {
      t1.traverse(haystack, preorder);
      return false;
    } catch (const T1*) {
      return true;
    }
  }

} // end of namespace CodeThorn
