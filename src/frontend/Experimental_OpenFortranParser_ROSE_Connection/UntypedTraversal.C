#include "sage3basic.h"
#include "UntypedTraversal.h"
#include "UntypedConverter.h"

using namespace Fortran::Untyped;


UntypedTraversal::UntypedTraversal(SgSourceFile* sourceFile)
{
   p_source_file = sourceFile;
}


InheritedAttribute
UntypedTraversal::evaluateInheritedAttribute(SgNode* n, InheritedAttribute currentScope)
{
   if (isSgUntypedFile(n) != NULL)
      {
         SgUntypedFile* ut_file = dynamic_cast<SgUntypedFile*>(n);
         SgSourceFile*  sg_file = p_source_file;
         ROSE_ASSERT(sg_file != NULL);

         currentScope = UntypedConverter::initialize_global_scope(sg_file);
      }

   else if (isSgUntypedGlobalScope(n) != NULL)
      {
         SgUntypedGlobalScope* ut_scope = dynamic_cast<SgUntypedGlobalScope*>(n);
         SgGlobal*             sg_scope = UntypedConverter::convertSgUntypedGlobalScope(ut_scope, SageBuilder::getGlobalScopeFromScopeStack());

         currentScope = sg_scope;
      }

   else if (isSgUntypedProgramHeaderDeclaration(n) != NULL)
      {
         SgUntypedProgramHeaderDeclaration* ut_program = dynamic_cast<SgUntypedProgramHeaderDeclaration*>(n);
         SgProgramHeaderStatement*          sg_program = UntypedConverter::convertSgUntypedProgramHeaderDeclaration(ut_program, currentScope);

         currentScope = SageBuilder::topScopeStack();
      }

   else if (isSgUntypedVariableDeclaration(n) != NULL)
      {
         SgUntypedVariableDeclaration* ut_decl = dynamic_cast<SgUntypedVariableDeclaration*>(n);
         UntypedConverter::convertSgUntypedVariableDeclaration(ut_decl, currentScope);
      }

   else if (isSgUntypedImplicitDeclaration(n) != NULL)
      {
         SgUntypedImplicitDeclaration* ut_decl = dynamic_cast<SgUntypedImplicitDeclaration*>(n);
         UntypedConverter::convertSgUntypedImplicitDeclaration(ut_decl, currentScope);
      }

   else
      {
         printf ("Down traverse: found a node of type ... %s\n", n->class_name().c_str());
      }
   return currentScope;
}


SynthesizedAttribute
UntypedTraversal::evaluateSynthesizedAttribute(SgNode* n, InheritedAttribute attr, SynthesizedAttributesList childAttrs)
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
         SgUntypedAssignmentStatement* ut_stmt = dynamic_cast<SgUntypedAssignmentStatement*>(n);

         printf ("Found an assignment statement ... %s\n", n->class_name().c_str());
         ROSE_ASSERT(childAttrs.size() == 2);
         SgExpression* lhs = isSgExpression(childAttrs[0]);
         SgExpression* rhs = isSgExpression(childAttrs[1]);

         if ( lhs != NULL && rhs != NULL )
            {
               SgExpression* assignmentExpr = new SgAssignOp(lhs, rhs, NULL);
               UntypedConverter::setSourcePositionUnknown(assignmentExpr);

               SgExprStatement* expressionStatement = new SgExprStatement(assignmentExpr);
               UntypedConverter::setSourcePositionUnknown(expressionStatement);

               SgScopeStatement* currentScope = attr;
               currentScope->append_statement(expressionStatement);

               UntypedConverter::convertLabel(ut_stmt, expressionStatement);
            }
      }
   else
      {
         printf ("Up   traverse: found a node of type ... %s\n", n->class_name().c_str());
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
               UntypedConverter::setSourcePositionUnknown(expr);
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
               UntypedConverter::setSourcePositionUnknown(op);
               break;
            }
         case SgToken::FORTRAN_INTRINSIC_MINUS:
            {
               printf("----------FORTRAN_INTRINSIC_MINUS: lhs=%p rhs=%p\n", lhs, rhs);
               op = new SgSubtractOp(lhs, rhs, NULL);
               UntypedConverter::setSourcePositionUnknown(op);
               break;
            }
         case SgToken::FORTRAN_INTRINSIC_POWER:
            {
               printf("----------FORTRAN_INTRINSIC_POWER:\n");
               op = new SgExponentiationOp(lhs, rhs, NULL);
               UntypedConverter::setSourcePositionUnknown(op);
               break;
            }
         case SgToken::FORTRAN_INTRINSIC_CONCAT:
            {
               printf("----------FORTRAN_INTRINSIC_CONCAT:\n");
               op = new SgConcatenationOp(lhs, rhs, NULL);
               UntypedConverter::setSourcePositionUnknown(op);
               break;
            }
         case SgToken::FORTRAN_INTRINSIC_TIMES:
            {
               printf("----------FORTRAN_INTRINSIC_TIMES: lhs=%p rhs=%p\n", lhs, rhs);
               op = new SgMultiplyOp(lhs, rhs, NULL);
               UntypedConverter::setSourcePositionUnknown(op);
               break;
            }
         case SgToken::FORTRAN_INTRINSIC_DIVIDE:
            {
               printf("----------FORTRAN_INTRINSIC_DIVIDE: lhs=%p rhs=%p\n", lhs, rhs);
               op = new SgDivideOp(lhs, rhs, NULL);
               UntypedConverter::setSourcePositionUnknown(op);
               break;
            }
         case SgToken::FORTRAN_INTRINSIC_AND:
            {
               printf("----------FORTRAN_INTRINSIC_AND:\n");
               op = new SgAndOp(lhs, rhs, NULL);
               UntypedConverter::setSourcePositionUnknown(op);
               break;
            }
         case SgToken::FORTRAN_INTRINSIC_OR:
            {
               printf("----------FORTRAN_INTRINSIC_OR:\n");
               op = new SgOrOp(lhs, rhs, NULL);
               UntypedConverter::setSourcePositionUnknown(op);
               break;
            }
         case SgToken::FORTRAN_INTRINSIC_EQV:
            {
               printf("----------FORTRAN_INTRINSIC_EQV:\n");
               op = new SgEqualityOp(lhs, rhs, NULL);
               ROSE_ASSERT(0);  // check on logical operands
               UntypedConverter::setSourcePositionUnknown(op);
               break;
            }
         case SgToken::FORTRAN_INTRINSIC_NEQV:
            {
               printf("----------FORTRAN_INTRINSIC_NEQV:\n");
               op = new SgNotEqualOp(lhs, rhs, NULL);
               ROSE_ASSERT(0);  // check on logical operands
               UntypedConverter::setSourcePositionUnknown(op);
               break;
            }
         case SgToken::FORTRAN_INTRINSIC_EQ:
            {
               printf("----------FORTRAN_INTRINSIC_EQ:\n");
               op = new SgEqualityOp(lhs, rhs, NULL);
               UntypedConverter::setSourcePositionUnknown(op);
               break;
            }
         case SgToken::FORTRAN_INTRINSIC_NE:
            {
               printf("----------FORTRAN_INTRINSIC_NE:\n");
               op = new SgNotEqualOp(lhs, rhs, NULL);
               UntypedConverter::setSourcePositionUnknown(op);
               break;
            }
         case SgToken::FORTRAN_INTRINSIC_GE:
            {
               printf("----------FORTRAN_INTRINSIC_GE:\n");
               op = new SgGreaterOrEqualOp(lhs, rhs, NULL);
               UntypedConverter::setSourcePositionUnknown(op);
               break;
            }
         case SgToken::FORTRAN_INTRINSIC_LE:
            {
               printf("----------FORTRAN_INTRINSIC_LE:\n");
               op = new SgLessOrEqualOp(lhs, rhs, NULL);
               UntypedConverter::setSourcePositionUnknown(op);
               break;
            }
         case SgToken::FORTRAN_INTRINSIC_LT:
            {
               printf("----------FORTRAN_INTRINSIC_LT:\n");
               op = new SgLessThanOp(lhs, rhs, NULL);
               UntypedConverter::setSourcePositionUnknown(op);
               break;
            }
         case SgToken::FORTRAN_INTRINSIC_GT:
            {
               printf("----------FORTRAN_INTRINSIC_GT:\n");
               op = new SgGreaterThanOp(lhs, rhs, NULL);
               UntypedConverter::setSourcePositionUnknown(op);
               break;
            }
         default:
            {
               ROSE_ASSERT(0);  // NOT IMPLEMENTED
            }
       }
    return op;
 }
