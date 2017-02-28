#include "sage3basic.h"
#include "UntypedTraversal.h"

using namespace Fortran::Untyped;

void
setSourcePositionUnknown( SgLocatedNode* locatedNode )
{
  // This function sets the source position to be marked as not available (since we don't have token information)
  // These nodes WILL be unparsed in the code generation phase.

  // The SgLocatedNode has both a startOfConstruct and endOfConstruct source position.
     ROSE_ASSERT(locatedNode != NULL);

  // Make sure we never try to reset the source position of the global scope (set elsewhere in ROSE).
     ROSE_ASSERT(isSgGlobal(locatedNode) == NULL);

  // Check the endOfConstruct first since it is most likely NULL (helpful in debugging)
     if (locatedNode->get_endOfConstruct() != NULL || locatedNode->get_startOfConstruct() != NULL)
        {
          printf ("In setSourcePosition(SgLocatedNode* locatedNode): locatedNode = %p = %s \n",locatedNode,locatedNode->class_name().c_str());
        }
     ROSE_ASSERT(locatedNode->get_endOfConstruct()   == NULL);
     ROSE_ASSERT(locatedNode->get_startOfConstruct() == NULL);

     SageInterface::setSourcePosition(locatedNode);
}


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

         SgUntypedGlobalScope*	ut_globalScope = ut_file->get_scope();
         printf ("                    ut_global scope       %p\n", ut_globalScope);

         initialize_global_scope(sg_file);
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
      }

   else if (isSgUntypedProgramHeaderDeclaration(n) != NULL)
      {
         SgUntypedProgramHeaderDeclaration* ut_program = dynamic_cast<SgUntypedProgramHeaderDeclaration*>(n);

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

        setSourcePositionUnknown(programDeclaration->get_parameterList());
        setSourcePositionUnknown(programDeclaration);

#if 0
        // DQ (12/9/2007): Moved so that the label's symbol can be put into the function
        // definition's scope.  See test2007_01.f90.
        setStatementNumericLabel(programDeclaration, label);

        ROSE_ASSERT(programDeclaration->get_parameterList() != NULL);

        if (programKeyword != NULL)
        {
            setSourcePosition(programDeclaration->get_parameterList(),
                    programKeyword);
            setSourcePosition(programDeclaration, programKeyword);
        }
        else
           {
           // These will be marked as isSourcePositionUnavailableInFrontend = true and isOutputInCodeGeneration = true

           // DQ (12/18/2008): These need to make marked with a valid file id (not NULL_FILE, internally),
           // so that any attached comments and CPP directives will be properly attached.
              setSourcePosition(programDeclaration->get_parameterList(), tokenList);
              setSourcePosition(programDeclaration, tokenList);
        }

        // Unclear if we should use the same token list for resetting the source position in all three IR nodes.
        setSourcePosition(programDefinition, tokenList);
        setSourcePosition(programBody, tokenList);

#endif

        setSourcePositionUnknown(programDefinition);
        setSourcePositionUnknown(programBody);

        ROSE_ASSERT(programDeclaration->get_firstNondefiningDeclaration() != programDeclaration);
        ROSE_ASSERT(programDeclaration->get_firstNondefiningDeclaration() == NULL);


//----------------------------- end statement (sort of)

//        if (programDeclaration->get_program_statement_explicit() == false)
           {
           // The function declaration should be forced to match the "end" keyword.
           // Reset the declaration to the current filename.
//FIXME-no this      programDeclaration->get_startOfConstruct()->set_filenameString(p_source_file->getFileName());
//FIXME              programDeclaration->get_endOfConstruct()->set_filenameString(p_source_file->getFileName());
           }

//        if (end_statement_name != NULL)
        {
            programDeclaration->set_named_in_end_statement(true);
        }

        printf("--- finished buiding program %s\n", programDeclaration->get_name().str());

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
               setSourcePositionUnknown(expr);
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
               setSourcePositionUnknown(op);
               break;
            }
         case SgToken::FORTRAN_INTRINSIC_MINUS:
            {
               printf("----------FORTRAN_INTRINSIC_MINUS: lhs=%p rhs=%p\n", lhs, rhs);
               op = new SgSubtractOp(lhs, rhs, NULL);
               setSourcePositionUnknown(op);
               break;
            }
         case SgToken::FORTRAN_INTRINSIC_POWER:
            {
               printf("----------FORTRAN_INTRINSIC_POWER:\n");
               op = new SgExponentiationOp(lhs, rhs, NULL);
               setSourcePositionUnknown(op);
               break;
            }
         case SgToken::FORTRAN_INTRINSIC_CONCAT:
            {
               printf("----------FORTRAN_INTRINSIC_CONCAT:\n");
               op = new SgConcatenationOp(lhs, rhs, NULL);
               setSourcePositionUnknown(op);
               break;
            }
         case SgToken::FORTRAN_INTRINSIC_TIMES:
            {
               printf("----------FORTRAN_INTRINSIC_TIMES: lhs=%p rhs=%p\n", lhs, rhs);
               op = new SgMultiplyOp(lhs, rhs, NULL);
               setSourcePositionUnknown(op);
               break;
            }
         case SgToken::FORTRAN_INTRINSIC_DIVIDE:
            {
               printf("----------FORTRAN_INTRINSIC_DIVIDE: lhs=%p rhs=%p\n", lhs, rhs);
               op = new SgDivideOp(lhs, rhs, NULL);
               setSourcePositionUnknown(op);
               break;
            }
         case SgToken::FORTRAN_INTRINSIC_AND:
            {
               printf("----------FORTRAN_INTRINSIC_AND:\n");
               op = new SgAndOp(lhs, rhs, NULL);
               setSourcePositionUnknown(op);
               break;
            }
         case SgToken::FORTRAN_INTRINSIC_OR:
            {
               printf("----------FORTRAN_INTRINSIC_OR:\n");
               op = new SgOrOp(lhs, rhs, NULL);
               setSourcePositionUnknown(op);
               break;
            }
         case SgToken::FORTRAN_INTRINSIC_EQV:
            {
               printf("----------FORTRAN_INTRINSIC_EQV:\n");
               op = new SgEqualityOp(lhs, rhs, NULL);
               ROSE_ASSERT(0);  // check on logical operands
               setSourcePositionUnknown(op);
               break;
            }
         case SgToken::FORTRAN_INTRINSIC_NEQV:
            {
               printf("----------FORTRAN_INTRINSIC_NEQV:\n");
               op = new SgNotEqualOp(lhs, rhs, NULL);
               ROSE_ASSERT(0);  // check on logical operands
               setSourcePositionUnknown(op);
               break;
            }
         case SgToken::FORTRAN_INTRINSIC_EQ:
            {
               printf("----------FORTRAN_INTRINSIC_EQ:\n");
               op = new SgEqualityOp(lhs, rhs, NULL);
               setSourcePositionUnknown(op);
               break;
            }
         case SgToken::FORTRAN_INTRINSIC_NE:
            {
               printf("----------FORTRAN_INTRINSIC_NE:\n");
               op = new SgNotEqualOp(lhs, rhs, NULL);
               setSourcePositionUnknown(op);
               break;
            }
         case SgToken::FORTRAN_INTRINSIC_GE:
            {
               printf("----------FORTRAN_INTRINSIC_GE:\n");
               op = new SgGreaterOrEqualOp(lhs, rhs, NULL);
               setSourcePositionUnknown(op);
               break;
            }
         case SgToken::FORTRAN_INTRINSIC_LE:
            {
               printf("----------FORTRAN_INTRINSIC_LE:\n");
               op = new SgLessOrEqualOp(lhs, rhs, NULL);
               setSourcePositionUnknown(op);
               break;
            }
         case SgToken::FORTRAN_INTRINSIC_LT:
            {
               printf("----------FORTRAN_INTRINSIC_LT:\n");
               op = new SgLessThanOp(lhs, rhs, NULL);
               setSourcePositionUnknown(op);
               break;
            }
         case SgToken::FORTRAN_INTRINSIC_GT:
            {
               printf("----------FORTRAN_INTRINSIC_GT:\n");
               op = new SgGreaterThanOp(lhs, rhs, NULL);
               setSourcePositionUnknown(op);
               break;
            }
         default:
            {
               ROSE_ASSERT(0);  // NOT IMPLEMENTED
            }
       }
    return op;
 }

void
UntypedTraversal::initialize_global_scope(SgSourceFile* file)
{
 // First we have to get the global scope initialized (and pushed onto the stack).

 // Set the default for source position generation to be consistent with other languages (e.g. C/C++).
    SageBuilder::setSourcePositionClassificationMode(SageBuilder::e_sourcePositionFrontendConstruction);

    SgGlobal* globalScope = file->get_globalScope();
    ROSE_ASSERT(globalScope != NULL);
    ROSE_ASSERT(globalScope->get_parent() != NULL);

 // Fortran is case insensitive
    globalScope->setCaseInsensitive(true);

 // DQ (8/21/2008): endOfConstruct is not set to be consistent with startOfConstruct.
    ROSE_ASSERT(globalScope->get_endOfConstruct()   != NULL);
    ROSE_ASSERT(globalScope->get_startOfConstruct() != NULL);

 // DQ (10/10/2010): Set the start position of global scope to "1".
    globalScope->get_startOfConstruct()->set_line(1);

 // DQ (10/10/2010): Set this position to the same value so that if we increment
 // by "1" the start and end will not be the same value.
    globalScope->get_endOfConstruct()->set_line(1);

    ROSE_ASSERT(SageBuilder::emptyScopeStack() == true);
    SageBuilder::pushScopeStack(globalScope);

 // CER - DELETE_ME
    printf ("                    sg_global scope       %p\n", globalScope);

#if WHERE_IS_DEBUG_COMMENT_LEVEL
    if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
       {
          SageBuilder::topScopeStack()->get_startOfConstruct()->display("In initialize_global_scope(): start");
          SageBuilder::topScopeStack()->get_endOfConstruct  ()->display("In initialize_global_scope(): end");
       }
#endif
}
