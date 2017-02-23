#include "sage3basic.h"
#include "UntypedTraversal.h"

using namespace SgUntyped;

SynthesizedAttribute
UntypedTraversal::evaluateSynthesizedAttribute(SgNode* n, SynthesizedAttributesList childAttrs)
{
// synthesized attribute is temporarily an expression, initialize to NULL for when an expression doesn't make sense;
   SynthesizedAttribute sg_expr = NULL;   

   if ( isSgUntypedExpression(n) != NULL)
      {
         printf ("Found an untyped expression   ... %s\n", n->class_name().c_str());
         if ( isSgUntypedBinaryOperator(n) != NULL )
            {
               SgUntypedBinaryOperator* op = dynamic_cast<SgUntypedBinaryOperator*>(n);
               ROSE_ASSERT(childAttrs.size() == 2);
               SgBinaryOp* sg_operator = convert_SgUntypedBinaryOperator(op, childAttrs[0], childAttrs[1]);
               sg_expr = sg_operator;
               printf ("  - binary operator      ==>   %s\n", op->get_operator_name().c_str());
               printf ("             list size   ==    %lu\n", childAttrs.size());
            }
         else if ( isSgUntypedValueExpression(n) != NULL )
            {
               SgUntypedValueExpression* expr = dynamic_cast<SgUntypedValueExpression*>(n);
               printf ("  - value expression     ==>   %s\n", expr->get_value_string().c_str());
               sg_expr = convert_SgUntypedValueExpression(expr);
               printf ("                 sg_expr ==    %p\n", sg_expr);
            }
         else if ( isSgUntypedReferenceExpression(n) != NULL )
            {
               SgUntypedReferenceExpression* expr = dynamic_cast<SgUntypedReferenceExpression*>(n);
               printf ("  - reference expression ==>   %s\n", expr->get_name().c_str());

               SgVarRefExp* varRef = SageBuilder::buildVarRefExp(expr->get_name(), NULL);
               ROSE_ASSERT(varRef != NULL);
               sg_expr = varRef;
               printf ("                 sg_expr ==    %p\n", sg_expr);
            }
      }
   else if ( isSgUntypedAssignmentStatement(n) != NULL )
      {
         printf ("Found an assignment statement ... %s\n", n->class_name().c_str());
      }
   else
      {
         printf ("Found a node of type ... %s\n", n->class_name().c_str());
      }
   
   return sg_expr;
}

SgExpression*
UntypedTraversal::convert_SgUntypedExpression(SgUntypedExpression* untyped_expression)
{
// TODO: Implementent this function and call it from traversal.  Will need to pass more information.
   SgExpression* sg_expr = NULL;
   return sg_expr;
}

SgValueExp*
UntypedTraversal::convert_SgUntypedValueExpression (SgUntypedValueExpression* untyped_value)
{
   SgValueExp* expr = NULL;

   switch(untyped_value->get_type()->get_type_enum_id())
       {
         case SgUntypedType::e_int:
            {
               printf("----------ValueExpression:TYPE_INT:\n");
               expr = new SgIntVal(atoi(untyped_value->get_value_string().c_str()), untyped_value->get_value_string());
               break;
            }
         default:
            {
               ROSE_ASSERT(0);  // NOT IMPLEMENTED
            }
       }
    return expr;
 }

SgUnaryOp*
UntypedTraversal::convert_SgUntypedUnaryOperator(SgUntypedUnaryOperator* untyped_operator, SgExpression* expr)
 {
    SgUnaryOp* op = NULL;

    switch(untyped_operator->get_operator_enum())
       {
         case SgToken::FORTRAN_INTRINSIC_NOT:
            {
               printf("----------FORTRAN_INTRINSIC_NOT:\n");
               op = new SgNotOp(expr, NULL);
               break;
            }
         default:
            {
               ROSE_ASSERT(0);  // NOT IMPLEMENTED
            }
       }
    return op;
 }

SgBinaryOp*
UntypedTraversal::convert_SgUntypedBinaryOperator(SgUntypedBinaryOperator* untyped_operator, SgExpression* lhs, SgExpression* rhs)
 {
    SgBinaryOp* op = NULL;

    switch(untyped_operator->get_operator_enum())
       {
         case SgToken::FORTRAN_INTRINSIC_PLUS:
            {
               printf("----------FORTRAN_INTRINSIC_PLUS: lhs=%p rhs=%p\n", lhs, rhs);
               op = new SgAddOp(lhs, rhs, NULL);
               break;
            }
         case SgToken::FORTRAN_INTRINSIC_MINUS:
            {
               printf("----------FORTRAN_INTRINSIC_MINUS: lhs=%p rhs=%p\n", lhs, rhs);
               op = new SgSubtractOp(lhs, rhs, NULL);
               break;
            }
         case SgToken::FORTRAN_INTRINSIC_POWER:
            {
               printf("----------FORTRAN_INTRINSIC_POWER:\n");
               op = new SgExponentiationOp(lhs, rhs, NULL);
               break;
            }
         case SgToken::FORTRAN_INTRINSIC_CONCAT:
            {
               printf("----------FORTRAN_INTRINSIC_CONCAT:\n");
               op = new SgConcatenationOp(lhs, rhs, NULL);
               break;
            }
         case SgToken::FORTRAN_INTRINSIC_TIMES:
            {
               printf("----------FORTRAN_INTRINSIC_TIMES: lhs=%p rhs=%p\n", lhs, rhs);
               op = new SgMultiplyOp(lhs, rhs, NULL);
               break;
            }
         case SgToken::FORTRAN_INTRINSIC_DIVIDE:
            {
               printf("----------FORTRAN_INTRINSIC_DIVIDE: lhs=%p rhs=%p\n", lhs, rhs);
               op = new SgDivideOp(lhs, rhs, NULL);
               break;
            }
         case SgToken::FORTRAN_INTRINSIC_AND:
            {
               printf("----------FORTRAN_INTRINSIC_AND:\n");
               op = new SgAndOp(lhs, rhs, NULL);
               break;
            }
         case SgToken::FORTRAN_INTRINSIC_OR:
            {
               printf("----------FORTRAN_INTRINSIC_OR:\n");
               op = new SgOrOp(lhs, rhs, NULL);
               break;
            }
         case SgToken::FORTRAN_INTRINSIC_EQV:
            {
               printf("----------FORTRAN_INTRINSIC_EQV:\n");
               op = new SgEqualityOp(lhs, rhs, NULL);
               ROSE_ASSERT(0);  // check on logical operands
               break;
            }
         case SgToken::FORTRAN_INTRINSIC_NEQV:
            {
               printf("----------FORTRAN_INTRINSIC_NEQV:\n");
               op = new SgNotEqualOp(lhs, rhs, NULL);
               ROSE_ASSERT(0);  // check on logical operands
               break;
            }
         case SgToken::FORTRAN_INTRINSIC_EQ:
            {
               printf("----------FORTRAN_INTRINSIC_EQ:\n");
               op = new SgEqualityOp(lhs, rhs, NULL);
               break;
            }
         case SgToken::FORTRAN_INTRINSIC_NE:
            {
               printf("----------FORTRAN_INTRINSIC_NE:\n");
               op = new SgNotEqualOp(lhs, rhs, NULL);
               break;
            }
         case SgToken::FORTRAN_INTRINSIC_GE:
            {
               printf("----------FORTRAN_INTRINSIC_GE:\n");
               op = new SgGreaterOrEqualOp(lhs, rhs, NULL);
               break;
            }
         case SgToken::FORTRAN_INTRINSIC_LE:
            {
               printf("----------FORTRAN_INTRINSIC_LE:\n");
               op = new SgLessOrEqualOp(lhs, rhs, NULL);
               break;
            }
         case SgToken::FORTRAN_INTRINSIC_LT:
            {
               printf("----------FORTRAN_INTRINSIC_LT:\n");
               op = new SgLessThanOp(lhs, rhs, NULL);
               break;
            }
         case SgToken::FORTRAN_INTRINSIC_GT:
            {
               printf("----------FORTRAN_INTRINSIC_GT:\n");
               op = new SgGreaterThanOp(lhs, rhs, NULL);
               break;
            }
         default:
            {
               ROSE_ASSERT(0);  // NOT IMPLEMENTED
            }
       }
    return op;
 }

