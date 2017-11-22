#include "sage3basic.h"

#include "ATermToUntypedTraversal.h"

#define PRINT_ATERM_TRAVERSAL 0


using namespace ATermSupport;

ATermToUntypedTraversal::ATermToUntypedTraversal(SgSourceFile* source)
{
   SgUntypedDeclarationStatementList* sg_decls = new SgUntypedDeclarationStatementList();
   SgUntypedStatementList*            sg_stmts = new SgUntypedStatementList();
   SgUntypedFunctionDeclarationList*  sg_funcs = new SgUntypedFunctionDeclarationList();
   SgUntypedGlobalScope*          global_scope = new SgUntypedGlobalScope("",sg_decls,sg_stmts,sg_funcs);

   pSourceFile = source;
   ROSE_ASSERT(pSourceFile != NULL);

   pUntypedFile = new SgUntypedFile(global_scope);
   ROSE_ASSERT(pUntypedFile != NULL);

// DQ (2/25/2013): Set the default for source position generation to be consistent with other languages (e.g. C/C++).
   SageBuilder::setSourcePositionClassificationMode(SageBuilder::e_sourcePositionFrontendConstruction);
//TODO     SageBuilder::setSourcePositionClassificationMode(SageBuilder::e_sourcePositionCompilerGenerated);
}

ATermToUntypedTraversal::~ATermToUntypedTraversal()
{
   delete pUntypedFile;
}


void
ATermToUntypedTraversal::fixupLocation(PosInfo & loc)
{
   int end_col = loc.getEndCol();

   // make sure start column isn't the same as end col
   if (loc.getStartLine() == loc.getEndLine() && loc.getStartCol() == loc.getEndCol())
      {
         return;
      }

   // check that end column isn't first column in the line
   if (end_col > 1)
      {
         loc.setEndCol(end_col - 1);
      }
}

PosInfo
ATermToUntypedTraversal::getLocation(ATerm term)
{
   PosInfo pinfo;

   ATerm annotations = ATgetAnnotations(term);
   if (annotations) {
      int i1,i2,i3,i4;
      ATerm loc = ATgetFirst(annotations);
      if (ATmatch(loc, "Location(<int>,<int>,<int>,<int>)", &i1,&i2,&i3,&i4)) {
#if PRINT_ATERM_TRAVERSAL
         printf("... loc: %d %d %d %d\n", i1,i2,i3,i4);
#endif
         pinfo = PosInfo(i1,i2,i3,i4);
      }
   }
   fixupLocation(pinfo);

   return pinfo;
}

void
ATermToUntypedTraversal::setSourcePosition( SgLocatedNode* locatedNode, ATerm term )
{
   PosInfo pos = getLocation(term);
   return setSourcePosition(locatedNode, pos);
}

void
ATermToUntypedTraversal::setSourcePosition( SgLocatedNode* locatedNode, PosInfo & pos )
{
   ROSE_ASSERT(locatedNode != NULL);
   ROSE_ASSERT(locatedNode->get_startOfConstruct() == NULL);
   ROSE_ASSERT(locatedNode->get_endOfConstruct()   == NULL);

   std::string filename = getCurrentFilename();

#if PRINT_SOURCE_POSITION
   std::cout << "... setSourcePosition: " << pos.getStartLine() << " " <<  pos.getStartCol();
   std::cout <<                       " " << pos.getEndLine()   << " " <<  pos.getEndCol() << std::endl;
#endif

   locatedNode->set_startOfConstruct(new Sg_File_Info(filename, pos.getStartLine(), pos.getStartCol()));
   locatedNode->get_startOfConstruct()->set_parent(locatedNode);

   locatedNode->set_endOfConstruct(new Sg_File_Info(filename, pos.getEndLine(), pos.getEndCol()));
   locatedNode->get_endOfConstruct()->set_parent(locatedNode);

   SageInterface::setSourcePosition(locatedNode);
}

void
ATermToUntypedTraversal::setSourcePositionFrom( SgLocatedNode* locatedNode, SgLocatedNode* fromNode )
{
   PosInfo pos;

   pos.setStartLine (fromNode->get_startOfConstruct()-> get_line());
   pos.setStartCol  (fromNode->get_startOfConstruct()-> get_col() );
   pos.setEndLine   (fromNode->get_endOfConstruct()  -> get_line());
   pos.setEndCol    (fromNode->get_endOfConstruct()  -> get_col() );

   return setSourcePosition(locatedNode, pos);
}

void
ATermToUntypedTraversal::setSourcePositionFromEndOnly( SgLocatedNode* locatedNode, SgLocatedNode* fromNode )
{
   PosInfo pos;

   pos.setStartLine (fromNode->get_endOfConstruct()-> get_line());
   pos.setStartCol  (fromNode->get_endOfConstruct()-> get_col() );
   pos.setEndLine   (fromNode->get_endOfConstruct()-> get_line());
   pos.setEndCol    (fromNode->get_endOfConstruct()-> get_col() );

   return setSourcePosition(locatedNode, pos);
}
