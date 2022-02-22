#include "sage3basic.h"
#include "ATermTraversal.h"
#include "SageTreeBuilder.h"

#define PRINT_ATERM_TRAVERSAL 0

using namespace ATermSupport;

ATermTraversal::ATermTraversal(SgSourceFile* source) : source_{source}
{
   ROSE_ASSERT(source_ && "ATermTraversal::ATermTraversal: source file is NULL");
}

void
ATermTraversal::fixupLocation(PosInfo & loc)
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
ATermTraversal::getLocation(ATerm term)
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
ATermTraversal::setSourcePositions(ATerm term, rb::SourcePosition &start, rb::SourcePosition &end)
{
   PosInfo pos = getLocation(term);

   start.path   = getCurrentFilename();
   start.line   = pos.getStartLine();
   start.column = pos.getStartCol();

   end.path   = getCurrentFilename();
   end.line   = pos.getStartLine();
   end.column = pos.getStartCol();
}

void ATermTraversal::setSourcePosition(SgLocatedNode* node, PosInfo &pos) {
  ROSE_ASSERT(node != NULL);

  std::string filename = getCurrentFilename();

  node->set_startOfConstruct(new Sg_File_Info(filename, pos.getStartLine(), pos.getStartCol()));
  node->get_startOfConstruct()->set_parent(node);

  node->set_endOfConstruct(new Sg_File_Info(filename, pos.getEndLine(), pos.getEndCol()));
  node->get_endOfConstruct()->set_parent(node);

  SageInterface::setSourcePosition(node);
}

void ATermTraversal::setSourcePosition(SgLocatedNode* node, const std::string &filename, PosInfo &pos) {
  ROSE_ASSERT(node);

  node->set_startOfConstruct(new Sg_File_Info(filename, pos.getStartLine(), pos.getStartCol()));
  node->get_startOfConstruct()->set_parent(node);

  node->set_endOfConstruct(new Sg_File_Info(filename, pos.getEndLine(), pos.getEndCol()));
  node->get_endOfConstruct()->set_parent(node);

  SageInterface::setSourcePosition(node);
}

void ATermTraversal::setSourcePositionFrom(SgLocatedNode* node, SgLocatedNode* fromNode) {
  PosInfo pos{};

  pos.setStartLine (fromNode->get_startOfConstruct()-> get_line());
  pos.setStartCol  (fromNode->get_startOfConstruct()-> get_col() );
  pos.setEndLine   (fromNode->get_endOfConstruct()  -> get_line());
  pos.setEndCol    (fromNode->get_endOfConstruct()  -> get_col() );

  return setSourcePosition(node, fromNode->getFilenameString(), pos);
}

void ATermTraversal::setSourcePositionFromEndOnly(SgLocatedNode* node, SgLocatedNode* fromNode) {
  PosInfo pos{};

  pos.setStartLine (fromNode->get_endOfConstruct()-> get_line());
  pos.setStartCol  (fromNode->get_endOfConstruct()-> get_col() );
  pos.setEndLine   (fromNode->get_endOfConstruct()-> get_line());
  pos.setEndCol    (fromNode->get_endOfConstruct()-> get_col() );

  return setSourcePosition(node, fromNode->getFilenameString(), pos);
}

void
ATermTraversal::setSourcePosition(SgLocatedNode* node, ATerm term)
{
   PosInfo pos = getLocation(term);
   return setSourcePosition(node, pos);
}

void
ATermTraversal::setSourcePositionExcludingTerm(SgLocatedNode* node, ATerm startTerm, ATerm endTerm)
{
   PosInfo pos = getLocation(startTerm);
   PosInfo end = getLocation(endTerm);

   pos.setEndLine(end.getStartLine());
   pos.setEndCol(end.getStartCol());

   return setSourcePosition(node, pos);
}

void
ATermTraversal::setSourcePositionIncludingTerm(SgLocatedNode* node,ATerm startTerm, ATerm endTerm)
{
   PosInfo pos = getLocation(startTerm);
   PosInfo end = getLocation(endTerm);

   pos.setEndLine(end.getEndLine());
   pos.setEndCol(end.getEndCol());

   return setSourcePosition(node, pos);
}

void
ATermTraversal::setSourcePositionIncludingNode(SgLocatedNode* node, ATerm startTerm, SgLocatedNode* endNode)
{
   PosInfo pos = getLocation(startTerm);

   pos.setEndLine(endNode->get_endOfConstruct()->get_line());
   pos.setEndCol (endNode->get_endOfConstruct()->get_col() );

   return setSourcePosition(node, pos);
}
