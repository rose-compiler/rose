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
    if(binaryOp->variantT() == V_SgAddOp)
    {
      if(binaryOp->get_rhs_operand()->variantT() == V_SgMultiplyOp)
      {
        swapOperands(binaryOp);
      }
      else if(binaryOp->get_rhs_operand()->variantT() == V_SgCastExp)
      {
        SgCastExp* castExp = isSgCastExp(binaryOp->get_rhs_operand());
        if(castExp->get_operand()->variantT() == V_SgMultiplyOp)
          swapOperands(binaryOp);
      }
    }
    /*
        a - (b * c) ===>  -((b * c) - a)
    */
    else if(binaryOp->variantT() == V_SgSubtractOp)
    {
      if(binaryOp->get_rhs_operand()->variantT() == V_SgMultiplyOp)
      {
        swapOperands(binaryOp);
        SgMinusOp* minusOp = buildMinusOp(deepCopy(binaryOp), SgUnaryOp::prefix);
        replaceExpression(binaryOp, minusOp, false);
      }
      else if(binaryOp->get_rhs_operand()->variantT() == V_SgCastExp)
      {
        SgCastExp* castExp = isSgCastExp(binaryOp->get_rhs_operand());
        if(castExp->get_operand()->variantT() == V_SgMultiplyOp)
        {
          swapOperands(binaryOp);
          SgMinusOp* minusOp = buildMinusOp(deepCopy(binaryOp), SgUnaryOp::prefix);
          replaceExpression(binaryOp, minusOp, false);
        }
      }
      
    }
  }
}

void SIMDNormalization::normalizeExpression(SgProject* project)
{
  normalizeTraversal traversal;
  traversal.traverse(project, postorder); 
}

/******************************************************************************************************************************/
/*
  Normalize the CompoundAssignOp:
  a += b ==> a = a + b
  a -= b ==> a = a - b
  a *= b ==> a = a * b
  a /= b ==> a = a / b
  a &= b ==> a = a & b
  a |= b ==> a = a | b
  a ^= b ==> a = a ^ b
*/
/******************************************************************************************************************************/
void SIMDNormalization::normalizeCompoundAssignOp(SgForStatement* forStatement)
{

  SgStatement* loopBody = forStatement->get_loop_body();
  ROSE_ASSERT(loopBody);
  Rose_STL_Container<SgNode*> compoundAssignOpList = NodeQuery::querySubTree (loopBody,V_SgCompoundAssignOp);
  for (Rose_STL_Container<SgNode*>::iterator i = compoundAssignOpList.begin(); i != compoundAssignOpList.end(); i++)
  {
    SgCompoundAssignOp* compoundAssignOp = isSgCompoundAssignOp(*i);
    switch(compoundAssignOp->variantT())
    {
      case V_SgPlusAssignOp:
        {
          SgAddOp* addOp = buildAddOp(deepCopy(compoundAssignOp->get_lhs_operand()), deepCopy(compoundAssignOp->get_rhs_operand()));
          replaceExpression(compoundAssignOp,
                            buildAssignOp(deepCopy(compoundAssignOp->get_lhs_operand()),addOp),
                            false);
        }
        break;
      case V_SgMinusAssignOp:
        {
          SgSubtractOp* subtractOp = buildSubtractOp(deepCopy(compoundAssignOp->get_lhs_operand()), deepCopy(compoundAssignOp->get_rhs_operand()));
          replaceExpression(compoundAssignOp,
                            buildAssignOp(deepCopy(compoundAssignOp->get_lhs_operand()),subtractOp),
                            false);
        }
        break;
      case V_SgMultAssignOp:
        {
          SgMultiplyOp* multiplyOp = buildMultiplyOp(deepCopy(compoundAssignOp->get_lhs_operand()), deepCopy(compoundAssignOp->get_rhs_operand()));
          replaceExpression(compoundAssignOp,
                            buildAssignOp(deepCopy(compoundAssignOp->get_lhs_operand()),multiplyOp),
                            false);
        }
        break;
      case V_SgDivAssignOp:
        {
          SgDivideOp* divideOp = buildDivideOp(deepCopy(compoundAssignOp->get_lhs_operand()), deepCopy(compoundAssignOp->get_rhs_operand()));
          replaceExpression(compoundAssignOp,
                            buildAssignOp(deepCopy(compoundAssignOp->get_lhs_operand()),divideOp),
                            false);
        }
        break;
      case V_SgAndAssignOp:
        {
          SgBitAndOp* bitAndOp = buildBitAndOp(deepCopy(compoundAssignOp->get_lhs_operand()), deepCopy(compoundAssignOp->get_rhs_operand()));
          replaceExpression(compoundAssignOp,
                            buildAssignOp(deepCopy(compoundAssignOp->get_lhs_operand()),bitAndOp),
                            false);
        }
        break;
      case V_SgIorAssignOp:
        {
          SgBitOrOp* bitOrOp = buildBitOrOp(deepCopy(compoundAssignOp->get_lhs_operand()), deepCopy(compoundAssignOp->get_rhs_operand()));
          replaceExpression(compoundAssignOp,
                            buildAssignOp(deepCopy(compoundAssignOp->get_lhs_operand()),bitOrOp),
                            false);
        }
        break;
      case V_SgXorAssignOp:
        {
          SgBitXorOp* bitXorOp = buildBitXorOp(deepCopy(compoundAssignOp->get_lhs_operand()), deepCopy(compoundAssignOp->get_rhs_operand()));
          replaceExpression(compoundAssignOp,
                            buildAssignOp(deepCopy(compoundAssignOp->get_lhs_operand()),bitXorOp),
                            false);
        }
        break;
      default:
        {
          cerr<<"warning, unhandled CompoundAssignOp: "<< compoundAssignOp->class_name()<<endl;
        }
    }
  }
}
