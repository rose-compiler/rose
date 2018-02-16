#ifndef ATERM_TO_UNTYPED_TRAVERSAL_H
#define ATERM_TO_UNTYPED_TRAVERSAL_H

#include <aterm2.h>

namespace ATermSupport {

class PosInfo
{
 public:
   PosInfo() : pStartLine(0), pStartCol(0), pEndLine(0), pEndCol(0)
    {
    }

   PosInfo(int strtLine, int strtCol, int endLine, int endCol)
     {
        pStartLine = strtLine;
        pStartCol  = strtCol;
        pEndLine   = endLine;
        pEndCol    = endCol;
     }

   int  getStartLine()             { return pStartLine; }
   int  getStartCol()              { return pStartCol;  }
   int  getEndLine()               { return pEndLine;   }
   int  getEndCol()                { return pEndCol;    }

   void setStartLine ( int line )  { pStartLine = line; }
   void setStartCol  ( int col  )  { pStartCol  = col;  }
   void setEndLine   ( int line )  { pEndLine   = line; }
   void setEndCol    ( int col  )  { pEndCol    = col;  }

 protected:
   int pStartLine, pStartCol;  // location (line,col) of first character ( 1 based)
   int pEndLine,   pEndCol;    // location (line,col) of last  character (+1 col)
};

class ATermToUntypedTraversal
{
 public:
   ATermToUntypedTraversal(SgSourceFile* source);
   virtual ~ATermToUntypedTraversal();

   SgUntypedFile*        get_file()  { return pUntypedFile; }
   SgUntypedGlobalScope* get_scope() { return pUntypedFile->get_scope(); }

   std::string getCurrentFilename()
      {
         return pSourceFile->get_sourceFileNameWithPath();
      }

 protected:
   SgUntypedFile* pUntypedFile;
   SgSourceFile*  pSourceFile;

 protected:

   static void    fixupLocation(PosInfo & loc);
   static PosInfo getLocation(ATerm term);

   void setSourcePosition              ( SgLocatedNode* locatedNode, ATerm term );
   void setSourcePosition              ( SgLocatedNode* locatedNode, PosInfo & pos );
   void setSourcePositionFrom          ( SgLocatedNode* locatedNode, SgLocatedNode* fromNode );
   void setSourcePositionExcludingTerm ( SgLocatedNode* locatedNode, ATerm startTerm, ATerm endTerm );
   void setSourcePositionIncludingTerm ( SgLocatedNode* locatedNode, ATerm startTerm, ATerm endTerm );
   void setSourcePositionIncludingNode ( SgLocatedNode* locatedNode, ATerm startTerm, SgLocatedNode* endNode );
   void setSourcePositionFromEndOnly   ( SgLocatedNode* locatedNode, SgLocatedNode* fromNode );


}; // class ATermToUntypedTraversal
}  // namespace ATermSupport

#endif
