#include "sage3basic.h"
#include "UntypedTraversal.h"
#include "UntypedConverter.h"

using namespace Fortran::Untyped;

UntypedTraversal::UntypedTraversal(SgSourceFile* sourceFile)
{
   p_source_file = sourceFile;
}

InheritedAttribute
UntypedTraversal::evaluateInheritedAttribute(SgNode* n, InheritedAttribute attr)
{
   if (isSgUntypedFile(n) != NULL)
      {
         SgUntypedFile* ut_file = dynamic_cast<SgUntypedFile*>(n);
         printf ("Down Traverse: found a file   ... %s\n", ut_file->class_name().c_str());

         SgSourceFile* sg_file = p_source_file;  //OpenFortranParser_globalFilePointer;
         ROSE_ASSERT(sg_file != NULL);
         printf ("                       SgSourceFile       %p\n", sg_file);
         printf ("                               name       %s\n", sg_file->getFileName().c_str());

         SgUntypedGlobalScope* ut_globalScope = ut_file->get_scope();
         printf ("                    ut_global scope       %p\n", ut_globalScope);

         attr = UntypedConverter::initialize_global_scope(sg_file);
      }

   else if (isSgUntypedGlobalScope(n) != NULL)
      {
         SgUntypedGlobalScope* ut_scope = dynamic_cast<SgUntypedGlobalScope*>(n);
         printf ("Down Traverse: found a global scope   ... %s\n", n->class_name().c_str());
         printf ("                    ut_global scope       %p\n", ut_scope);
         printf ("                    sg_global scope       %p\n", SageBuilder::getGlobalScopeFromScopeStack());

      // The global scope should have neither declarations nor executables
         ROSE_ASSERT(ut_scope->get_declaration_list()-> get_traversalSuccessorContainer().size() == 0);
         ROSE_ASSERT(ut_scope->get_statement_list()  -> get_traversalSuccessorContainer().size() == 0);

         attr = SageBuilder::getGlobalScopeFromScopeStack();
      }

   else if (isSgUntypedProgramHeaderDeclaration(n) != NULL)
      {
         SgUntypedProgramHeaderDeclaration* ut_program = dynamic_cast<SgUntypedProgramHeaderDeclaration*>(n);

         SgUntypedNamedStatement* ut_program_end_statement = ut_program->get_end_statement();
         ROSE_ASSERT(ut_program_end_statement != NULL);

         SgName programName = ut_program->get_name();
         printf ("                         found a program  %s\n", programName.str());

         SgScopeStatement* topOfStack = SageBuilder::topScopeStack();
         ROSE_ASSERT(topOfStack->variantT() == V_SgGlobal);

      // We should test if this is in the function type table, but do this later
         SgFunctionType* type = new SgFunctionType(SgTypeVoid::createType(), false);

         SgProgramHeaderStatement* programDeclaration = new SgProgramHeaderStatement(programName, type, NULL);

      // This is the defining declaration and there is no non-defining declaration!
         programDeclaration->set_definingDeclaration(programDeclaration);

        programDeclaration->set_scope(topOfStack);
        programDeclaration->set_parent(topOfStack);

     // Add the program declaration to the global scope
        SgGlobal* globalScope = isSgGlobal(topOfStack);
        ROSE_ASSERT(globalScope != NULL);
        globalScope->append_statement(programDeclaration);

        // A symbol using this name should not already exist
        ROSE_ASSERT(!globalScope->symbol_exists(programName));

        // Add a symbol to the symbol table in global scope
        SgFunctionSymbol* symbol = new SgFunctionSymbol(programDeclaration);
        globalScope->insert_symbol(programName, symbol);

        //FIXME if (SgProject::get_verbose() > DEBUG_COMMENT_LEVEL)
           {
              printf("Inserted SgFunctionSymbol in globalScope using name = %s \n", programName.str());
           }

        SgBasicBlock* programBody = new SgBasicBlock();
        SgFunctionDefinition* programDefinition = new SgFunctionDefinition(programDeclaration, programBody);

        programBody->setCaseInsensitive(true);
        programDefinition->setCaseInsensitive(true);

        SageBuilder::pushScopeStack(programDefinition);
        SageBuilder::pushScopeStack(programBody);

        programBody->set_parent(programDefinition);
        programDefinition->set_parent(programDeclaration);

        UntypedConverter::setSourcePositionUnknown(programDeclaration->get_parameterList());
        UntypedConverter::setSourcePositionUnknown(programDeclaration);

     // Convert the labels for the program begin and end statements
        UntypedConverter::convertLabel(ut_program,               programDeclaration, SgLabelSymbol::e_start_label_type, /*label_scope=*/ programDefinition);
        UntypedConverter::convertLabel(ut_program_end_statement, programDeclaration, SgLabelSymbol::e_end_label_type,   /*label_scope=*/ programDefinition);

      // Set the end statement name if it exists
         if (ut_program_end_statement->get_statement_name().empty() != true)
            {
               programDeclaration->set_named_in_end_statement(true);
            }

#if THIS_PART_NEEDS_TO_IMPLEMENT_NO_PROGRAM_STATEMENT
        ROSE_ASSERT(programDeclaration->get_parameterList() != NULL);

        if (programKeyword != NULL)
        {
            UntypedConverter::setSourcePosition(programDeclaration->get_parameterList(), programKeyword);
            UntypedConverter::setSourcePosition(programDeclaration, programKeyword);
        }
        else
           {
           // These will be marked as isSourcePositionUnavailableInFrontend = true and isOutputInCodeGeneration = true

           // DQ (12/18/2008): These need to make marked with a valid file id (not NULL_FILE, internally),
           // so that any attached comments and CPP directives will be properly attached.
              UntypedConverter::setSourcePosition(programDeclaration->get_parameterList(), tokenList);
              UntypedConverter::setSourcePosition(programDeclaration, tokenList);
        }

        // Unclear if we should use the same token list for resetting the source position in all three IR nodes.
        UntypedConverter::setSourcePosition(programDefinition, tokenList);
        UntypedConverter::setSourcePosition(programBody, tokenList);
#endif

        UntypedConverter::setSourcePositionUnknown(programDefinition);
        UntypedConverter::setSourcePositionUnknown(programBody);

        ROSE_ASSERT(programDeclaration->get_firstNondefiningDeclaration() != programDeclaration);
        ROSE_ASSERT(programDeclaration->get_firstNondefiningDeclaration() == NULL);

//        if (programDeclaration->get_program_statement_explicit() == false)
        //   {
           // The function declaration should be forced to match the "end" keyword.
           // Reset the declaration to the current filename.
//FIXME-no this      programDeclaration->get_startOfConstruct()->set_filenameString(p_source_file->getFileName());
//FIXME              programDeclaration->get_endOfConstruct()->set_filenameString(p_source_file->getFileName());
        //    }

        printf("--- finished building program %s\n", programDeclaration->get_name().str());

        attr = programBody;
      }

// R501 type-declaration-stmt
   else if (isSgUntypedVariableDeclaration(n) != NULL)
      {
         SgUntypedVariableDeclaration* ut_decl = dynamic_cast<SgUntypedVariableDeclaration*>(n);
         UntypedConverter::convertSgUntypedVariableDeclaration(ut_decl, attr);
      }

// R560 implicit-stmt
   else if (isSgUntypedImplicitDeclaration(n) != NULL)
      {
         SgUntypedImplicitDeclaration* ut_decl = dynamic_cast<SgUntypedImplicitDeclaration*>(n);
         UntypedConverter::convertSgUntypedImplicitDeclaration(ut_decl, attr);
      }

   else
      {
         printf ("Down traverse: found a node of type ... %s\n", n->class_name().c_str());
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
