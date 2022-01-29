#ifndef ATERM_TRAVERSAL_H
#define ATERM_TRAVERSAL_H

#include "PosInfo.h"
#include <aterm2.h>

namespace Rose {
  namespace builder {
    struct SourcePosition;
  }
}

namespace rb = Rose::builder;
using PosInfo = rb::PosInfo;

namespace ATermSupport {

class ATermTraversal
{
 public:
   ATermTraversal() = delete;
   ATermTraversal(SgSourceFile* source);

   std::string getCurrentFilename()
     {
       return source_->get_sourceFileNameWithPath();
     }

 protected:
   SgSourceFile* source_;

   static void    fixupLocation(PosInfo & loc);
   static PosInfo getLocation(ATerm term);

   void setSourcePositions(ATerm term, rb::SourcePosition &start, rb::SourcePosition &end);

   void setSourcePosition(SgLocatedNode* node, PosInfo &pos);
   void setSourcePosition(SgLocatedNode* node, const std::string &filename, PosInfo &pos);

   void setSourcePositionFrom        ( SgLocatedNode* node, SgLocatedNode* fromNode );
   void setSourcePositionFromEndOnly ( SgLocatedNode* node, SgLocatedNode* fromNode );

   void setSourcePosition              ( SgLocatedNode* node, ATerm term );
   void setSourcePositionExcludingTerm ( SgLocatedNode* node, ATerm startTerm, ATerm endTerm );
   void setSourcePositionIncludingTerm ( SgLocatedNode* node, ATerm startTerm, ATerm endTerm );
   void setSourcePositionIncludingNode ( SgLocatedNode* node, ATerm startTerm, SgLocatedNode* endNode );

}; // class ATermTraversal
}  // namespace ATermSupport

#endif
