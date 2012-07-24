#include "normalization.h"

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;
using namespace normalization;

/******************************************************************************************************************************/
/*
  Swap the lhs and rhs operand
*/
/******************************************************************************************************************************/
void normalization::swapOperands(SgBinaryOp* binaryOp) 
{
  SgExpression* lhs = binaryOp->get_lhs_operand();
  SgExpression* rhs = binaryOp->get_rhs_operand();
  binaryOp->set_lhs_operand(NULL);
  binaryOp->set_rhs_operand(NULL);
  binaryOp->set_lhs_operand(rhs);
  binaryOp->set_rhs_operand(lhs);
}


/******************************************************************************************************************************/
/*
  Normalize the expression to the following format:
  a * b + c ==> (a * b) + c
  a * b - c ==> (a * b) - c
  c + a * b ==> (a * b) + c
  c - a * b ==> -( (a * b) - c)
*/
/******************************************************************************************************************************/

class normalizeTraversal : public AstSimpleProcessing
{
  public:
    virtual void visit(SgNode* n);
};

void normalizeTraversal::visit(SgNode* n)
{
  SgBinaryOp* binaryOp = isSgBinaryOp(n);
  if(binaryOp != NULL)
  {
    ROSE_ASSERT(binaryOp);
    if((binaryOp->variantT() == V_SgAddOp) && (binaryOp->get_rhs_operand()->variantT() == V_SgMultiplyOp))
    {
      swapOperands(binaryOp);
    }
    else if((binaryOp->variantT() == V_SgSubtractOp) && (binaryOp->get_rhs_operand()->variantT() == V_SgMultiplyOp))
    {
      swapOperands(binaryOp);
      SgNode* parent = binaryOp->get_parent();
      SgMinusOp* minusOp = buildMinusOp(deepCopy(binaryOp), SgUnaryOp::prefix);
      replaceExpression(binaryOp, minusOp, false);
      
    }
  }
}

void normalization::normalizeExperission(SgProject* project)
{
  normalizeTraversal traversal;
  traversal.traverse(project, postorder); 
}

