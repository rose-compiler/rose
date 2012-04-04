#include <rose.h>
#include <Outliner.hh>

#include "Outline.h"
#include "GenericDepAttrib.h"
#include "Utils.h"


using namespace Outliner;
using namespace SageBuilder;

/******************************************************************************
 A FILLET attribute and unitary class.

 Definition:
   Fillet = the 'flesh' of the program; the program without the skeleton.

 Attributes used here:
   "FILLET" -- unit attribute: non-skeleton code.
     Invariants of this attribute:
       if set on node, none of its children/ancestors have it set!
*/

class FilletAttribute : public AstAttribute {
  public:
    FilletAttribute() {}
    FilletAttribute *copy() {
      return new FilletAttribute(*this);
    }
    std::string attribute_class_name() { return "FilletAttribute"; }
    std::string toString() { return "FilletAttribute"; }
};

FilletAttribute fillet = FilletAttribute();

bool isFillet(SgNode *s) {
  return s->getAttribute("FILLET") == (AstAttribute*) &fillet;
}

/******************************************************************************
 shouldOmit - determine if attributes consider this statement skeleton or not
 */
bool shouldOmit(APISpec *spec, SgStatement *s) {
    AstAttribute *attr = s->getAttribute("APIDep");
    return attr ? spec->shouldOmit((GenericDepAttribute*)attr) : true;
}

/******************************************************************************
  SPECIFICATION:
    ifNotSkeletonAddFilletAttrib =
      do
      let toremove = ...
      foreach s in toremove:
        if s->get_parent `notElem` toremove && ... then
          s->setAttribute("FILLET",fillet)
*/
void ifNotSkeletonAddFilletAttrib (APISpecs *specs, SgProject *proj) {
  std::set< SgStatement* > toremove;

  // FIXME[CR2]: why 'synthesizedAttribute'? why query (vs traversal)?
  NodeQuerySynthesizedAttributeType stmts =
      NodeQuery::querySubTree(proj, V_SgStatement);

  for(size_t i = 0; i < stmts.size(); i++) {
      SgStatement *s = isSgStatement(stmts[i]);
      bool omit = false;
      APISpecs::iterator it;
      for(it = specs->begin(); it != specs->end(); it++) {
          if(shouldOmit(*it, s)) {
              omit = true;
          }
      }
      if(omit)
        toremove.insert(s);
  }

  std::set< SgStatement* >::iterator si = toremove.begin();
  for(; si != toremove.end(); si++) {
      SgStatement *s = *si;
      SgStatement *parStmt = isSgStatement(s->get_parent());

      // FIXME: Q. class defn / func defn are statements?
      if(!(    (parStmt && toremove.find(parStmt) != toremove.end())
            || isSgDeclarationStatement(s)
            || isSgReturnStmt(s)
            || isSgClassDefinition(s)
            || isSgFunctionDefinition(s)))
        s->setAttribute("FILLET",&fillet);
  }
}

/******************************************************************************
 blockifySpansOfFillets(ss) - take spans of adjacent FILLETS and put them
   into new basic blocks.
   - the FILLET attributes must be removed and the new basic block must
     get the FILLET attributes.
   - PRECONDITION: not 'in' a FILLET.
 */
void blockifySpansOfFillets (SgBasicBlock *block) {

  // if (debug)  std::cout << "blockifySpansOfFillets:" << std::endl;

  SgStatementPtrList &ss = block->get_statements();
  SgStatementPtrList::iterator i = ss.begin();

  while (i != ss.end()) {

    // Find first FILLET:
    i = find_if (i, ss.end(), isFillet);
    if (i == ss.end())
      break;

    // make 'j' point after end of of span:
    //   I.e.,
    //   j == ss.end() or +1 past the 'span',
    SgStatementPtrList::iterator j = i+1;
    while (j != ss.end() && isFillet(*j))
      j++;

    // Move FILLET span to new basic block, if length > 1:
    if (j > i+1) {

      // Create new Basic Block:
      //  - mark this as a transformation (required)
      Sg_File_Info* srcLoc =
        Sg_File_Info::generateDefaultFileInfoForTransformationNode();
      ROSE_ASSERT(srcLoc != NULL);
      SgBasicBlock *newblock = new SgBasicBlock(srcLoc);
      SgStatementPtrList &newss = newblock->get_statements();
      newblock->set_parent(block);
      newblock->setAttribute("FILLET",&fillet);

      // Add the statements to it, updated accordingly:
      newss.assign (i,j);
      for (SgStatementPtrList::iterator k = newss.begin(); k < newss.end(); k++)
      {
        (*k)->set_parent(newblock);
        (*k)->removeAttribute("FILLET");
      }

      // Replace/Remove from 'block'/'ss':
      i = ss.erase(i,j);
      i = ss.insert(i,newblock);
    }
    i++;
  }
}

/******************************************************************************
 moveFilletSpansToNewBasicBlocks =
   // and mv attribs from children to new BasicBlock
   foreach basicblock b that's not contained in a FILLET:
     do
     ss <- statements b
     if any (\s-> s->getAttribute("FILLET") == fillet) ss then
       do
       ss' <- blockifySpansOfFillets ss
     b.statements := ss'
*/

class Move : public AstPrePostProcessing {
  private:
    bool inFillet;   // true if node has ancestor with FILLET attribute.
  public:
    virtual void preOrderVisit (SgNode* n);
    virtual void postOrderVisit(SgNode* n);
};

void Move::preOrderVisit(SgNode* n) {
  if (!inFillet && isFillet(n))
    inFillet = true;
}

void Move::postOrderVisit(SgNode* n) {
  if (isFillet(n)) {
    inFillet = false;
  } else if (!inFillet) {
    SgBasicBlock* block = isSgBasicBlock(n);
    if (block)
      blockifySpansOfFillets( block );
  }
}

void moveFilletSpansToNewBasicBlocks (SgProject *proj) {
  Move traversal;
  traversal.traverseInputFiles (proj);
}

/*******************************************************************************
 outlineStatementsWithFilletAttrib (proj,outline) -
  foreach statement s in proj:
    if s->getAttribute() == FILLET then
      if outline
        Outliner::outline(s)
      else
        remove(s)

  NOTE:  If 'outline' is false, just remove the 'Fillet's.
*/
void outlineStatementsWithFilletAttrib (SgProject *proj, bool outline) {
  // FIXME[CR2]: why 'synthesizedAttribute'? why query (vs traversal)?
  NodeQuerySynthesizedAttributeType stmts =
    NodeQuery::querySubTree(proj, V_SgStatement);

  for(size_t i = 0; i < stmts.size(); i++) {
    SgStatement *s = isSgStatement(stmts[i]);
    if (isFillet(s))
      if (outline)
        if (! Outliner::isOutlineable (s))
          std::cout << "statement isn't outlineable: "
                    << "(" << s->unparseToString() << ")@"
                    << std::endl;
        else
          Outliner::outline(s);
      else if (isSgStatement(s->get_parent()))
        if(LowLevelRewrite::isRemovableStatement(s))
          LowLevelRewrite::remove(s);
        else {
          SgStatement *emptyStmt = new SgNullStatement(s->get_file_info());
          if(debug)
              std::cout << "Replacing "
                        << s->class_name() << " "
                        << "(" << s->unparseToString() << ")@"
                        << s->getFilenameString()
                        << std::endl;
          LowLevelRewrite::replace(s, emptyStmt);
        }
  }
}

/*******************************************************************************
 skeletonizeCode - the exported function.
*/
void skeletonizeCode (APISpecs *specs, SgProject *proj, bool outline,
                      bool genPDF) {
  ifNotSkeletonAddFilletAttrib(specs, proj);
  if (debug) AstTests::runAllTests(proj);

  // Create report of attributes on AST (before code changes):
  if(debug) std::cout << "Generating PDF report" << std::endl;
  if(genPDF) generatePDF(*proj);

  moveFilletSpansToNewBasicBlocks(proj);
    // and moves fillet attrib from these spans to the new BasicBlocks
  if (debug) AstTests::runAllTests(proj);

  outlineStatementsWithFilletAttrib(proj, outline);
  if (debug) AstTests::runAllTests(proj);
}
