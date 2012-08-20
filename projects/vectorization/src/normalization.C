#include "normalization.h"

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;
using namespace SIMDNormalization;

/******************************************************************************************************************************/
/*
  Swap the lhs and rhs operand
*/
/******************************************************************************************************************************/
void SIMDNormalization::swapOperands(SgBinaryOp* binaryOp) 
{
  SgExpression* lhs = binaryOp->get_lhs_operand();
  SgExpression* rhs = binaryOp->get_rhs_operand();
  binaryOp->set_lhs_operand(rhs);
  binaryOp->set_rhs_operand(lhs);
}


/******************************************************************************************************************************/
/*
  Normalize the expression to the following format:
  No transformation needed: a * b + c ==> (a * b) + c
  No transformation needed: a * b - c ==> (a * b) - c
  Transformation needed   : c + a * b ==> (a * b) + c 
  Transformation needed   : c - a * b ==> -( (a * b) - c)
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
    /*
        a + (b * c) ===>  (b * c) + a
    */
    if((binaryOp->variantT() == V_SgAddOp) && (binaryOp->get_rhs_operand()->variantT() == V_SgMultiplyOp))
    {
      swapOperands(binaryOp);
    }
    /*
        a - (b * c) ===>  -((b * c) - a)
    */
    else if((binaryOp->variantT() == V_SgSubtractOp) && (binaryOp->get_rhs_operand()->variantT() == V_SgMultiplyOp))
    {
      swapOperands(binaryOp);
      SgMinusOp* minusOp = buildMinusOp(deepCopy(binaryOp), SgUnaryOp::prefix);
      replaceExpression(binaryOp, minusOp, false);
      
    }
  }
}

void SIMDNormalization::normalizeExpression(SgProject* project)
{
  normalizeTraversal traversal;
  traversal.traverse(project, postorder); 
}

