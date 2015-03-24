
#ifndef ASTTERM_H
#define ASTTERM_H

/*********************************
 * Author: Markus Schordan, 2012 *
 *********************************/

#include "RoseAst.h"
#include <typeinfo>

namespace SPRAY {

  class AstTerm {
  public:
    static std::string astTermToMultiLineString(SgNode* node,int sep=1, int pos=0);
    static std::string astTermWithNullValuesToString(SgNode* node);
    static std::string astTermWithNullValuesToDot(SgNode* root);
    static std::string astTermWithoutNullValuesToDot(SgNode* root);
    static std::string astTermWithNullValuesToDotFragment(SgNode* root) ;
    static std::string astTermToDot(RoseAst::iterator start, RoseAst::iterator end);
    static std::string functionAstTermsWithNullValuesToDot(SgNode* root);
    static std::string pointerExprToString(SgNode* node);
  private:
    static std::string nodeTypeName(SgNode* node);
    static std::string dotFragmentToDot(std::string fragment);
  };
}

#endif
