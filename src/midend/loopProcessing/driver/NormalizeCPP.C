#include "rose.h"
#include "ProcessAstTree.h"

#include "AstInterface_ROSE.h"
#include <vector>

extern Sg_File_Info* GetFileInfo();

void FixFileInfo(SgNode* n)
{
  if (n == 0)
    return ;
  SgFunctionDeclaration *fd = isSgFunctionDeclaration(n);
  if (fd != 0) {
     SgFunctionDefinition* def = fd->get_definition();
     if (def != 0) {
       SgBasicBlock *body = def->get_body();
       Sg_File_Info *end = body->get_endOfConstruct();
       fd->set_endOfConstruct( new Sg_File_Info( end->get_filename(), end->get_line(), end->get_col()));
     }
  }
  std::vector <SgNode*> children = n->get_traversalSuccessorContainer();
  for (std::vector<SgNode*>::const_iterator cp = children.begin();
         cp != children.end(); ++cp) {
      FixFileInfo(*cp);
  } 
  if (n->get_endOfConstruct() == 0) {
       int size = children.size(); 
       for ( ; size > 0; --size) {
          if (children[size-1] != 0)
               break;
       }
       if (size > 0) {
          Sg_File_Info *end = children[size-1]->get_endOfConstruct();
          if (end != 0) {
            SgLocatedNode *n1 = isSgLocatedNode(n);
            if (n1 != 0) 
              n1->set_endOfConstruct( new Sg_File_Info( end->get_filename(), end->get_line(), end->get_col()));
          }
       }
  }
}

/* Goal: 
    * test expression:
           i<x is normalized to i<= (x-1)
           i>x is normalized to i>= (x+1) 
    * increment expression:
           i++ is normalized to i=i+1
           i-- is normalized to i=i-1 
 */
class NormalizeLoopTraverse : public ProcessAstTree
{
  bool succ;
  //Always return true ??? 
  //set succ to true if the loop has been normalized, otherwise set it to false
  virtual bool ProcessLoop(AstInterface &fa, const AstNodePtr& _s,
                               const AstNodePtr& body,
                               AstInterface::TraversalVisitType t)
   {
    SgNode* s = AstNodePtrImpl(_s).get_ptr();
    if (t == AstInterface::PreVisit) {
      // Skip normalization if it is not a for loop
      SgForStatement *fs = isSgForStatement(s);
      if (fs == 0) 
        { succ = false; return true; }

      // Must have only one init statement
      SgStatementPtrList &init = fs->get_init_stmt();
      if (init.size() != 1)  
        {succ = false; return true; }
      
      // Normalized the test expressions
      AstNodePtrImpl test = fs->get_test_expr();
      AstNodePtr testlhs, testrhs;
      if (!fa.IsBinaryOp(test, 0, &testlhs, &testrhs)) {
          succ = false; return true;
      }
      switch (test->variantT()) {
        case V_SgLessThanOp:  // i<x is normalized to i<= (x-1)
             fa.ReplaceAst( test,
                          fa.CreateBinaryOP(AstInterface::BOP_LE, fa.CopyAstTree(testlhs),
                           fa.CreateBinaryOP(AstInterface::BOP_MINUS, fa.CopyAstTree(testrhs),
                                                   fa.CreateConstInt(1)))); 
              break;
        case V_SgGreaterThanOp: // i>x is normalized to i>= (x+1)
             fa.ReplaceAst( test,
                       fa.CreateBinaryOP(AstInterface::BOP_GE, fa.CopyAstTree(testlhs), 
                                 fa.CreateBinaryOP(AstInterface::BOP_PLUS, fa.CopyAstTree(testrhs),
                                                           fa.CreateConstInt(1))));
             break;
        case V_SgLessOrEqualOp:
        case V_SgGreaterOrEqualOp:
        case V_SgNotEqualOp:
            break;
        default:
           succ = false; return true;
      }
      // Normalize the increment expression
      AstNodePtrImpl incr = fs->get_increment();
      switch (incr->variantT()) {
        case V_SgPlusPlusOp: //i++ is normalized to i=i+1
           if (! fa.IsSameVarRef(AstNodePtrImpl(isSgPlusPlusOp(incr.get_ptr())->get_operand()), 
                                fa.SkipCasting(testlhs))) // tolerate type casting within expressions
               { succ = false; return true; }
           fa.ReplaceAst( incr, 
             AstNodePtrImpl(new SgPlusAssignOp( GetFileInfo(), 
              isSgExpression(AstNodePtrImpl(fa.CopyAstTree(testlhs)).get_ptr()),
              isSgExpression(AstNodePtrImpl(fa.CreateConstInt(1)).get_ptr()))));
           break;
        case V_SgMinusMinusOp: //i-- is normalized to i=i-1
           if (! fa.IsSameVarRef(AstNodePtrImpl(isSgMinusMinusOp(incr.get_ptr())->get_operand()), 
                                 fa.SkipCasting(testlhs)))
               { succ = false; return true; }
           fa.ReplaceAst( incr, 
             AstNodePtrImpl(new SgPlusAssignOp( GetFileInfo(),
              isSgExpression(AstNodePtrImpl(fa.CopyAstTree(testlhs)).get_ptr()),
              isSgExpression(AstNodePtrImpl(fa.CreateConstInt(-1)).get_ptr()))));
           break;
        default:
           succ =  false; return true;
      }
      succ = true;
     } // end if (PreVisit)
     return true;
   }
 public:
   bool operator () ( AstInterface& li, const AstNodePtr& root)
    {
       succ = true;
       ProcessAstTree::operator()(li, root);
       return succ;
    }
};

void NormalizeForLoop (AstInterface& fa, const AstNodePtr& head) {
  NormalizeLoopTraverse()(fa, head);
}
