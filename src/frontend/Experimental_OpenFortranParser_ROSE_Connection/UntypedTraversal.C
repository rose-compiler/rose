#include "sage3basic.h"
#include "UntypedTraversal.h"

using namespace Fortran::Untyped;

void initialize_global_scope(SgSourceFile* file)
   {
  // First we have to get the global scope initialized (and pushed onto the stack).

  // Set the default for source position generation to be consistent with other languages (e.g. C/C++).
     SageBuilder::setSourcePositionClassificationMode(SageBuilder::e_sourcePositionFrontendConstruction);

#if 0
  // printf ("In initialize_global_scope_if_required(): astScopeStack.empty() = %s \n",astScopeStack.empty() ? "true" : "false");
     if (astScopeStack.empty() == true)
        {
          if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
               printf ("In initialize_global_scope_if_required(): OpenFortranParser_globalFilePointer = %p \n",OpenFortranParser_globalFilePointer);

          //CER          ROSE_ASSERT(OpenFortranParser_globalFilePointer != NULL);
          //CER          SgSourceFile* file = OpenFortranParser_globalFilePointer;
          SgGlobal* globalScope = file->get_globalScope();
          ROSE_ASSERT(globalScope != NULL);
          ROSE_ASSERT(globalScope->get_parent() != NULL);

       // DQ (11/28/2010): Added specification of case insensitivity for Fortran.
          globalScope->setCaseInsensitive(true);

       // DQ (8/21/2008): endOfConstruct is not set to be consistant with startOfConstruct.
          ROSE_ASSERT(globalScope->get_endOfConstruct()   != NULL);
          ROSE_ASSERT(globalScope->get_startOfConstruct() != NULL);

       // Scopes should be pushed onto the front of the stack (we define the top of the stack to
       // be the front).  I have used a vector instead of a stack for now, this might change later.
          ROSE_ASSERT(astScopeStack.empty() == true);
          astScopeStack.push_front(globalScope);

       // DQ (10/10/2010): Set the start position of global scope to "1".
          globalScope->get_startOfConstruct()->set_line(1);

       // DQ (10/10/2010): Set this position to the same value so that if we increment
       // by "1" the start and end will not be the same value.
          globalScope->get_endOfConstruct()->set_line(1);
#if 0
          astScopeStack.front()->get_startOfConstruct()->display("In initialize_global_scope_if_required(): start");
          astScopeStack.front()->get_endOfConstruct  ()->display("In initialize_global_scope_if_required(): end");
#endif
        }

  // Testing the scope stack...
  // DQ (11/28/2010): Added specification of case insensitivity for Fortran.
     std::list<SgScopeStatement*>::iterator scopeInterator = astScopeStack.begin();
     while (scopeInterator != astScopeStack.end())
        {
          if ((*scopeInterator)->isCaseInsensitive() == false)
             {
               printf ("##### Error (in initialize_global_scope_if_required): the scope handling is set to case sensitive scopeInterator = %p = %s \n",*scopeInterator,(*scopeInterator)->class_name().c_str());
            // (*scopeInterator)->setCaseInsensitive(true);
             }
          ROSE_ASSERT((*scopeInterator)->isCaseInsensitive() == true);
          scopeInterator++;
        }
#endif
   }


InheritedAttribute
UntypedTraversal::evaluateInheritedAttribute(SgNode* n, InheritedAttribute attr)
{
   SgScopeStatement* sg_scope = SageBuilder::getGlobalScopeFromScopeStack();

   if (isSgUntypedFile(n) != NULL)
      {
         SgUntypedFile* ut_file = dynamic_cast<SgUntypedFile*>(n);
         printf ("Down Traverse: found a file   ... %s\n", ut_file->class_name().c_str());

         SgSourceFile* sg_file = OpenFortranParser_globalFilePointer;
         ROSE_ASSERT(sg_file != NULL);
         printf ("                       SgSourceFile       %p\n", sg_file);

         SgUntypedGlobalScope*	ut_globalScope = ut_file->get_scope();
         printf ("                    ut_global scope       %p\n", ut_globalScope);

        // DQ: This is the earliest location to setup the global scope (I think).
         initialize_global_scope(sg_file);
      }

   else if (isSgUntypedGlobalScope(n) != NULL)
      {
         std::vector<SgNode*> list;

         SgUntypedGlobalScope* scope = dynamic_cast<SgUntypedGlobalScope*>(n);
         printf ("Down Traverse: found a global scope   ... %s\n", n->class_name().c_str());
         printf ("                    ut_global scope       %p\n", scope);

         if (OpenFortranParser_globalFilePointer != NULL)
            {
               printf ("                               name       %s\n", OpenFortranParser_globalFilePointer->getFileName().c_str());
            }

         list = scope->get_declaration_list()->get_traversalSuccessorContainer();
         ROSE_ASSERT(list.size() == 0);

         list = scope->get_statement_list()->get_traversalSuccessorContainer();
         ROSE_ASSERT(list.size() == 0);

         list = scope->get_function_list()->get_traversalSuccessorContainer();
         ROSE_ASSERT(list.size() != 0);

         if (isSgUntypedProgramHeaderDeclaration(list.front()))
            {
               SgUntypedProgramHeaderDeclaration* program = dynamic_cast<SgUntypedProgramHeaderDeclaration*>(list.front());
               printf ("                         found a program  %s\n", program->get_name().c_str());

               SgName name = program->get_name();

               // We should test if this is in the function type table, but do this later
               SgFunctionType* type = new SgFunctionType(SgTypeVoid::createType(), false);

               SgProgramHeaderStatement* programDeclaration = new SgProgramHeaderStatement(name, type, NULL);

#if 0
               // This is the defining declaration and there is no non-defining declaration!
               programDeclaration->set_definingDeclaration(programDeclaration);

               programDeclaration->set_scope(topOfStack);
               programDeclaration->set_parent(topOfStack);
#endif

            }
      }
   else
      {
         printf ("Down Traverse: found a node of type ... %s\n", n->class_name().c_str());
      }
   return attr;
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

