#include "rose.h"

void fixupFortranUseDeclarations (SgNode* node)
   {
  // DQ (7/7/2005): Introduce tracking of performance of ROSE.
     TimingPerformance timer ("Fixup Fortran Use Declarations:");

  // printf ("Fixup the Fortran Use Declarations ... \n");

     FixupFortranUseDeclarations astFixupTraversal;

  // I think the default should be preorder so that the interfaces would be more uniform
     astFixupTraversal.traverse(node,preorder);
   }

#if 1
void
trace_back_through_parent_scopes_lookup_variable_symbol_but_do_not_build_variable(
     const SgName & variableName, SgScopeStatement* currentScope, 
     SgVariableSymbol* & variableSymbol, SgFunctionSymbol* & functionSymbol, 
     SgClassSymbol* & classSymbol);
#endif

void
FixupFortranUseDeclarations::visit ( SgNode* node )
   {
  // DQ (11/24/2007): Output the current IR node for debugging the traversal of the Fortran AST.
  // printf ("node = %s \n",node->class_name().c_str());

     SgUseStatement* useDeclaration = isSgUseStatement(node);
     if (useDeclaration != NULL)
        {
          printf ("Found the SgUseDeclaration \n");

          printf ("In FixupFortranUseDeclarations::visit(): name = %s \n",useDeclaration->get_name().str());
#if 0
          SgExpressionPtrList & rename_list = useDeclaration->get_rename_list()->get_expressions();
          for (size_t i = 0; i < rename_list.size(); i++)
             {
            // For each module listed build SgAliasSymbols and add them to the local symbol table.

               SgExpression* renameExp = rename_list[i];
               printf ("In FixupFortranUseDeclarations::visit(): renameExp = %p = %s = %s \n",renameExp,renameExp->class_name().c_str(),SageInterface::get_name(renameExp).c_str());

               SgVariableSymbol* variableSymbol = NULL;
               SgFunctionSymbol* functionSymbol = NULL;
               SgClassSymbol*    classSymbol    = NULL;
#if 0
               trace_back_through_parent_scopes_lookup_variable_symbol_but_do_not_build_variable(variableName,currentScope,variableSymbol,functionSymbol,classSymbol);
#endif
             }
#endif
#if 0
          printf ("Exiting at the base of FixupFortranUseDeclarations::visit() \n");
          ROSE_ASSERT(false);
#endif
        }

#if 0
  // Comment out this C++ specific part for now!
     SgUsingDeclarationStatement* usingDeclarationStatement = isSgUsingDeclarationStatement(node);
     if (usingDeclarationStatement != NULL)
        {
          printf ("Found the SgUsingDeclarationStatement \n");

          printf ("Exiting at the base of FixupFortranUseDeclarations::visit() \n");
          ROSE_ASSERT(false);
        }

     SgUsingDirectiveStatement* usingDirectiveStatement = isSgUsingDirectiveStatement(node);
     if (usingDirectiveStatement != NULL)
        {
          printf ("Found the SgUsingDirectiveStatement \n");

          printf ("Exiting at the base of FixupFortranUseDeclarations::visit() \n");
          ROSE_ASSERT(false);
        }
#endif
   }



