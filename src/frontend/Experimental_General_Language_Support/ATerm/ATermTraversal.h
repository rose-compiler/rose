#ifndef ATERM_TRAVERSAL_H
#define ATERM_TRAVERSAL_H

#include <aterm2.h>

namespace Rose {
  namespace builder {
    struct SourcePosition;
  }
}

namespace rb = Rose::builder;

namespace ATermSupport {

class PosInfo
{
 public:
   PosInfo() : startLine_(0), startCol_(0), endLine_(0), endCol_(0)
    {
    }

   PosInfo(int strtLine, int strtCol, int endLine, int endCol)
     {
        startLine_ = strtLine;
        startCol_  = strtCol;
        endLine_   = endLine;
        endCol_    = endCol;
     }

   PosInfo(SgLocatedNode* fromNode)
     {
        ROSE_ASSERT(fromNode != NULL);
        ROSE_ASSERT(fromNode->get_startOfConstruct() != NULL);
        ROSE_ASSERT(fromNode->get_endOfConstruct()   != NULL);

        startLine_ = fromNode->get_startOfConstruct()->get_line();
        startCol_  = fromNode->get_startOfConstruct()->get_col();
        endLine_   = fromNode->get_endOfConstruct()->get_line();
        endCol_    = fromNode->get_endOfConstruct()->get_col();
     }

   int  getStartLine() const       { return startLine_; }
   int  getStartCol()  const       { return startCol_;  }
   int  getEndLine()   const       { return endLine_;   }
   int  getEndCol()    const       { return endCol_;    }

   void setStartLine ( int line )  { startLine_ = line; }
   void setStartCol  ( int col  )  { startCol_  = col;  }
   void setEndLine   ( int line )  { endLine_   = line; }
   void setEndCol    ( int col  )  { endCol_    = col;  }

 protected:
   int startLine_, startCol_;  // location (line,col) of first character ( 1 based)
   int endLine_,   endCol_;    // location (line,col) of last  character (+1 col)
};

class ATermTraversal
{
 public:
   ATermTraversal() = delete;
   ATermTraversal(SgSourceFile* source);

   std::string getCurrentFilename()
     {
       return pSourceFile->get_sourceFileNameWithPath();
     }

 protected:
   SgSourceFile*  pSourceFile;

   static void    fixupLocation(PosInfo & loc);
   static PosInfo getLocation(ATerm term);

   void setSourcePositions(ATerm term, rb::SourcePosition &start, rb::SourcePosition &end);

   void setSourcePosition              ( SgLocatedNode* node, ATerm term );
   void setSourcePosition              ( SgLocatedNode* node, PosInfo & pos );
   void setSourcePositionFrom          ( SgLocatedNode* node, SgLocatedNode* fromNode );
   void setSourcePositionExcludingTerm ( SgLocatedNode* node, ATerm startTerm, ATerm endTerm );
   void setSourcePositionIncludingTerm ( SgLocatedNode* node, ATerm startTerm, ATerm endTerm );
   void setSourcePositionIncludingNode ( SgLocatedNode* node, ATerm startTerm, SgLocatedNode* endNode );
   void setSourcePositionFromEndOnly   ( SgLocatedNode* node, SgLocatedNode* fromNode );

}; // class ATermTraversal
}  // namespace ATermSupport

#endif
