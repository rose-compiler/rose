#include <boost/foreach.hpp>
#include <string>
#include <iostream>

#include <rose.h>
#include <Outliner.hh>

#include "Outline.h"
#include "GenericDepAttrib.h"
#include "Utils.h"
#include "annotatePragmas.h"
#define foreach BOOST_FOREACH

using namespace std;
using namespace Outliner;
using namespace SageBuilder;
using namespace SageInterface;

static const string filletAttrib = "FILLET";

SgStatement * getPreviousStatementForOutline( SgStatement * targetStmt );

/******************************************************************************
 A FILLET attribute and unitary class.

 Definition:
   Fillet = the 'flesh' of the program; the program without the skeleton.

 Attributes used here:
   filletAttrib -- unit attribute: non-skeleton code.
     Invariants of this attribute:
       if set on node, none of its children/ancestors have it set!
*/

class FilletAttribute : public AstAttribute {
  public:
    FilletAttribute() {}
    FilletAttribute *copy() const {
      return new FilletAttribute(*this);
    }
    std::string attribute_class_name() { return "FilletAttribute"; }
    std::string toString() { return "FilletAttribute"; }
};

FilletAttribute fillet = FilletAttribute();

bool isFillet(SgNode *s) {
  return s->getAttribute(filletAttrib) == (AstAttribute*) &fillet;
}

/******************************************************************************
 shouldOmit - determine if attributes consider this statement skeleton or not
 */
bool shouldOmitGivenAPI(APISpec *spec, SgStatement *s) {
    if( s->getAttribute("APIDep") != NULL ){
      GenericDepAttribute *attr = dynamic_cast<GenericDepAttribute*>(s->getAttribute("APIDep"));
      ROSE_ASSERT( attr != NULL );
      return spec->shouldOmit(attr);
    }
    return true;
}

// FIXME: ADHOC: For debugging!
string ppPresRemUnd (PresRemUnd x);

/******************************************************************************
  SPECIFICATION:
    ifNotSkeletonAddFilletAttrib =
      do
      let toremove = ...
      foreach s in toremove:
        if s->get_parent `notElem` toremove && ... then
          s->setAttribute(filletAttrib,fillet)
*/
void ifNotSkeletonAddFilletAttrib (APISpecs *specs, SgProject *proj) {
  std::set< SgStatement* > toremove;

  // FIXME[CR2]: why 'synthesizedAttribute'? why query (vs traversal)?
  NodeQuerySynthesizedAttributeType stmts =
      NodeQuery::querySubTree(proj, V_SgStatement);

  bool debug = false;

  for (size_t i = 0; i < stmts.size(); i++) {
      SgStatement *s = isSgStatement(stmts[i]);
      if (debug) cout << "O: stmt: " << s->unparseToString() << endl;
      if (debug) printf ("%p\n", s);
      if (debug) cout << "O: attr: " << ppPresRemUnd (getPresRemUnd(s)) << endl;

      bool stdOmit = false;
      APISpecs::iterator it;
      for(it = specs->begin(); it != specs->end(); ++it) {
          if(shouldOmitGivenAPI(*it, s)) {
              stdOmit = true;
          }
      }

      // pragmas override 'stdOmit' status:
      bool omit;
      switch (getPresRemUnd(s))
        {
        case pragma_undef   : omit = stdOmit; break;
        case pragma_preserve: omit = false;   break;
        case pragma_remove  : omit = true;    break;
        default             : assert(0);
        }

      if (omit)
        toremove.insert(s);

      if (debug) printf ("O: omit = %i\n", omit);
      if (debug) printf ("O: stdOmit = %i\n", stdOmit);
  }

  std::set< SgStatement* >::iterator si = toremove.begin();
  for(; si != toremove.end(); ++si) {
      SgStatement *s = *si;
      SgStatement *parStmt = isSgStatement(s->get_parent());

      // FIXME: Q. class defn / func defn are statements?
      if(!(    (parStmt && toremove.find(parStmt) != toremove.end())
            || isSgDeclarationStatement(s)
            || isSgReturnStmt(s)
            || isSgClassDefinition(s)
            || isSgFunctionDefinition(s)))
        s->setAttribute(filletAttrib,&fillet);
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
      ++j;

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
      newblock->setAttribute(filletAttrib,&fillet);

      // Add the statements to it, updated accordingly:
      newss.assign (i,j);
      for (SgStatementPtrList::iterator k = newss.begin(); k < newss.end(); ++k)
      {
        (*k)->set_parent(newblock);
        (*k)->removeAttribute(filletAttrib);
      }

      // Replace/Remove from 'block'/'ss':
      i = ss.erase(i,j);
      i = ss.insert(i,newblock);
    }
    ++i;
  }
}

/******************************************************************************
 moveFilletSpansToNewBasicBlocks =
   // and mv attribs from children to new BasicBlock
   foreach basicblock b that's not contained in a FILLET:
     do
     ss <- statements b
     if any (\s-> s->getAttribute(filletAttrib) == fillet) ss then
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
    if (isFillet(s)) {
      if (outline) {
        if (! Outliner::isOutlineable(s))
          std::cout << "statement isn't outlineable: "
                    << "(" << s->unparseToString() << ")@"
                    << std::endl;
        else
          Outliner::outline(s);
      } else if (isSgStatement(s->get_parent())) {
          // FIXME: above condition needed, but not sure if there's
          //        a more elegant way?
        // JED: TODO this looks like the place where we want to check for preprocessing info
        // and if it's there move it to a nearby (next?) AST node
        SgStatement * const previousStatement = getPreviousStatementForOutline(s);
        ROSE_ASSERT( previousStatement != NULL );
        if( SgBasicBlock * const block = isSgBasicBlock(s) ){
          foreach(SgStatement * stmt, block->get_statements()){
            ROSE_ASSERT( stmt != NULL );
            if( AttachedPreprocessingInfoType* comments = stmt->getAttachedPreprocessingInfo() ){
              std::vector<int> captureList;
              int commentIndex = 0;
              if (debug) std::cout << "stmt is '"              << stmt->unparseToString()              << "'" << std::endl;
              if (debug) std::cout << "previousStatement is '" << previousStatement->unparseToString() << "'" << std::endl;
              for(AttachedPreprocessingInfoType::iterator i = comments->begin(); i != comments->end(); ++i){
                captureList.push_back(commentIndex);
                commentIndex++;
              }
              moveCommentsToNewStatement(stmt,captureList,previousStatement,true);
              //removeStatement(stmt); // JED: should be fine to remove the whole block below instead of the individual statements
                                       // as the previousStatement was chosen relative to the block.
            }
          }
        }
        removeStatement(s);
        if(debug) {
          const std::string action = outline ? "Replacing " : "Removing ";
          std::cout << action
                    << s->class_name() << " "
                    << "(" << s->unparseToString() << ")@"
                    << s->getFilenameString()
                    << std::endl;
        }
      }
    }
  }
}

/*

This function exists to workaround the way ROSE's builtin getPreviousStatement
works.  Specificaly, getPreviousStatement on an SgBasicBlock (or the first
statement of a scope) returns the surrounding scope. Instead, we want to find
the previous program statement in those cases. In the case of an SgBasicBlock
this function will search through the statements of the surrounding scope and
return the statement before the basic block. In the case where that is also a
scope it will recursively go up a level.

Note: This function is not very well tested.

*/
SgStatement * getPreviousStatementForOutline( SgStatement * const targetStmt )
{
  if( SgBasicBlock * const block = isSgBasicBlock(targetStmt) ){
    // Find the statement before the basic block, we can't simply call getPreviousStatement for basic blocks
    // because that actually returns a scope statement. The best we can do is to then scan that scope for
    // the statement before the basic block.
    SgStatement * previousStatement = NULL;
    if( SgScopeStatement * const scope = block->get_scope() ){
      // TODO: look at the rose code for getPreviousStatement for an example of how to write this search
      if (isSgIfStmt(scope)) {
         // TODO: I believe this is wrong. Write a test case...
         previousStatement = isSgStatement(targetStmt->get_parent());
         ROSE_ASSERT (isSgIfStmt(previousStatement) != NULL);
         if( previousStatement == NULL ){
           if (debug) std::cout << "previousStatement is null, and statement is " << targetStmt->unparseToString() << std::endl;
         }
         ROSE_ASSERT( previousStatement != NULL );
      } else {
        // refactor this search to a separate function
        SgStatementPtrList & statementList = scope->getStatementList();
        Rose_STL_Container<SgStatement*>::iterator i = statementList.begin();
        Rose_STL_Container<SgStatement*>::iterator previousStatementIterator =  statementList.end();
        while ( ( i != statementList.end() ) && ( (*i) != targetStmt ) ){
          previousStatementIterator = i++;
        }
        
        if ( previousStatementIterator != statementList.end() ){
          previousStatement = *previousStatementIterator;
        }
        if( previousStatement == NULL ){
          if (debug) std::cout << "previousStatement is null, and statement is " << targetStmt->unparseToString() << std::endl;
          previousStatement = getPreviousStatementForOutline(scope);
        }
        ROSE_ASSERT( previousStatement != NULL );
      }
    } else {
      ROSE_ASSERT( false ); // What does it mean if the block has no scope?
    }
    // JED: What TODO when previousStatement is null?
    if( previousStatement == NULL ){
      if (debug) std::cout << "previousStatement is null, and statement is " << targetStmt->unparseToString() << std::endl;
    }
    return previousStatement;
  } else {
    return getPreviousStatement(targetStmt);
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
