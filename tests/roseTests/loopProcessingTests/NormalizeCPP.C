
#include <ProcessAstTree.h>
#include <AstInterface_ROSE.h>
#include <vector>
#include <iostream>
//do not include the following files from rose.h
#define CFG_ROSE_H
#define CONTROLFLOWGRAPH_H
#define PRE_H
#define ASTDOTGENERATION_TEMPLATES_C
#include <sage3.h>

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

class NormalizeLoopTraverse : public ProcessAstTree
{
  bool succ;
  virtual bool ProcessLoop(AstInterface &fa, const AstNodePtr& _s,
                               const AstNodePtr& body,
                               AstInterface::TraversalVisitType t)
  {
    SgNode* s = AstNodePtrImpl(_s).get_ptr();
    // jichi (10/9/2009): Add in normalization for FortranDo.
    if (t != AstInterface::PreVisit)
     return true;
    switch(s->variantT()) {
    case V_SgFortranDo:
      {
        SgFortranDo *f = isSgFortranDo(s);
        //if (f == 0) { succ = false; return true; }
        SgExpression* incr = f->get_increment();
        ROSE_ASSERT(incr != NULL);

        // If increment is empty, replace it with default 1.
        if(isSgNullExpression(incr)) {
          //SgExpression* incr = new SgIntVal(1, "1");        // SgIntVal(int value, std::string valueString)
          //fa.ReplaceAst(AstNodePtrImpl(f->get_increment()), AstNodePtrImpl(incr));
          //fa.ReplaceAst(AstNodePtrImpl(f->get_increment()), fa.CreateConstInt(1));
                // This won't work due to deficiency of ReplaceAst, which directly use generic replace_expression().

          AstNodePtr one = fa.CreateConstInt(1);
          SgExpression* new_incr = isSgExpression((SgNode*) one.get_ptr());
          ROSE_ASSERT(new_incr != NULL);

          incr->set_parent(NULL);        // Detach original increment node from FortranDo.
          f->set_increment(new_incr);
          new_incr->set_parent(f);
        }

      }
      succ = true;
      break;

    case V_SgForStatement:
      {
        SgForStatement *fs = isSgForStatement(s);
        //if (fs == 0) { succ = false; return true; }

        SgStatementPtrList &init = fs->get_init_stmt();
        if (init.size() != 1)  { succ = false; return true; }

        AstNodePtrImpl test = fs->get_test_expr();
        AstNodePtr testlhs, testrhs;
        if (!fa.IsBinaryOp(test, 0, &testlhs, &testrhs)) {
          succ = false; return true;
        }

        switch (test->variantT()) {
        case V_SgLessThanOp:
          fa.ReplaceAst( test,
                          fa.CreateBinaryOP(AstInterface::BOP_LE, fa.CopyAstTree(testlhs),
                           fa.CreateBinaryOP(AstInterface::BOP_MINUS, fa.CopyAstTree(testrhs),
                                                   fa.CreateConstInt(1)))); 
           break;
         case V_SgGreaterThanOp:
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

        AstNodePtrImpl incr = fs->get_increment();
        switch (incr->variantT()) {
        case V_SgPlusPlusOp:
          if (! fa.IsSameVarRef(AstNodePtrImpl(isSgPlusPlusOp(incr.get_ptr())->get_operand()), testlhs))
          { succ = false; return true; }
          fa.ReplaceAst( incr, 
             AstNodePtrImpl(new SgPlusAssignOp( GetFileInfo(), 
              isSgExpression(AstNodePtrImpl(fa.CopyAstTree(testlhs)).get_ptr()),
              isSgExpression(AstNodePtrImpl(fa.CreateConstInt(1)).get_ptr()))));
           break;
        case V_SgMinusMinusOp:
          if (! fa.IsSameVarRef(AstNodePtrImpl(isSgMinusMinusOp(incr.get_ptr())->get_operand()), testlhs))
          { succ = false; return true; }
          fa.ReplaceAst( incr, 
            AstNodePtrImpl(new SgPlusAssignOp( GetFileInfo(),
             isSgExpression(AstNodePtrImpl(fa.CopyAstTree(testlhs)).get_ptr()),
             isSgExpression(AstNodePtrImpl(fa.CreateConstInt(-1)).get_ptr()))));
          break;
        default:
          succ =  false; return true;
        }
      }
      succ = true;
      break;

    default:
      succ = false;
    }
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
