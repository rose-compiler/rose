// Author: Markus Schordan
// $Id: AstNodePtrs.h,v 1.3 2008/01/08 02:56:38 dquinlan Exp $

#ifndef ASTNODEPTRS_H
#define ASTNODEPTRS_H

#include <string>
#include <iomanip>
#include "AstProcessing.h"

// DQ (8/10/2004):
// This class appears to return a list of all traversed AST nodes.  It is equivalent to the
// AstQuery operator with the input variant V_SgNode.  Not sure how important this mechanism 
// is (appears to be used in ASTConsistencyTests.[Ch]).

struct AstNodePtrSynAttr 
   {
     SgNode* node;
     AstNodePtrSynAttr() { node = NULL; }
   };

class AstNodePtrs : public SgBottomUpProcessing<AstNodePtrSynAttr>
   {
     public:
          AstNodePtrs();
         ~AstNodePtrs();
     protected:
          typedef std::vector<SgNode*> AstNodePointersList;
          virtual AstNodePtrSynAttr evaluateSynthesizedAttribute(SgNode* node, SynthesizedAttributesList );
          virtual void visitWithAstNodePointersList(SgNode*, AstNodePointersList) {}
     private:
   };

#endif
