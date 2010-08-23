// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "transformationSupport.h"
#include "resolveFortranReferences.h"
void fixupFortranReferences (SgNode* node)
   {
  // DQ (7/7/2005): Introduce tracking of performance of ROSE.
     TimingPerformance timer ("Fixup Fortran references:");

  // printf ("Fixup the Fortran array vs. function references ... \n");

  // FixUpGlobalFunctionTypeTable v;
  // SgFunctionType::traverseMemoryPoolNodes(v);
  // SgMemberFunctionType::traverseMemoryPoolNodes(v);

     FixupFortranReferences astFixupTraversal;

  // I think the default should be preorder so that the interfaces would be more uniform
     astFixupTraversal.traverse(node,preorder);
   }


void
FixupFortranReferences::visit ( SgNode* node )
   {
  // DQ (11/24/2007): Output the current IR node for debugging the traversal of the Fortran AST.
  // printf ("node = %s \n",node->class_name().c_str());

     SgUnknownArrayOrFunctionReference* unknownArrayOrFunctionReference = isSgUnknownArrayOrFunctionReference(node);
     if (unknownArrayOrFunctionReference != NULL)
        {
       // printf ("Found the SgUnknownArrayOrFunctionReference \n");

       // SgStatement* currentStatement = TransformationSupport::getStatement(unknownArrayOrFunctionReference);
          ROSE_ASSERT(unknownArrayOrFunctionReference->get_parent() != NULL);
          SgExprStatement* currentExpressionStatement = isSgExprStatement(unknownArrayOrFunctionReference->get_parent());

          if (currentExpressionStatement != NULL)
             {
               SgScopeStatement* currentScope = currentExpressionStatement->get_scope();
               ROSE_ASSERT(currentScope != NULL);

               SgName functionName = unknownArrayOrFunctionReference->get_name();
               SgFunctionSymbol* functionSymbol = SageInterface::lookupFunctionSymbolInParentScopes(functionName,currentScope);
               ROSE_ASSERT(functionSymbol != NULL);

               SgFunctionRefExp* functionRefExp = new SgFunctionRefExp(functionSymbol,NULL);
               SageInterface::setSourcePosition(functionRefExp);

               SgExprListExp* functionArguments = unknownArrayOrFunctionReference->get_expression_list();
               ROSE_ASSERT(functionArguments != NULL);

               SgFunctionCallExp* functionCallExp = new SgFunctionCallExp(functionRefExp,functionArguments,NULL);
               SageInterface::setSourcePosition(functionCallExp);

            // Replace the SgUnknownArrayOrFunctionReference in the SgExprStatement.
               currentExpressionStatement->set_expression(functionCallExp);
               functionCallExp->set_parent(currentExpressionStatement);

            // Remove the SgUnknownArrayOrFunctionReference IR node (add this to a delete list so we don't confuse the AST traversal).
            // delete unknownArrayOrFunctionReference;
               unknownArrayOrFunctionReference = NULL;
             }

       // printf ("Exiting as a test: Found the SgUnknownArrayOrFunctionReference \n");
       // ROSE_ASSERT(false);
        }

  // DQ (12/9/2007): Fixup the labels in the SgArithmeticIfStatement IR node
     SgArithmeticIfStatement* arithmeticIf = isSgArithmeticIfStatement(node);
     if (arithmeticIf != NULL)
        {
       // Fixup the label references.  This is required because the labeled statements were not available
       // (had not been visited) when the SgArithmeticIfStatement IR node was built.

       // DQ (12/30/2007): This is now modified due to the introduction of the SgLabelRefExp
       // SgLabelSymbol* labelArray[3] = { arithmeticIf->get_less_label(), 
       //                                  arithmeticIf->get_equal_label(),
       //                                  arithmeticIf->get_greater_label() };
          SgLabelSymbol* labelArray[3] = { arithmeticIf->get_less_label()->get_symbol(), 
                                           arithmeticIf->get_equal_label()->get_symbol(),
                                           arithmeticIf->get_greater_label()->get_symbol() };

          for (int i=0; i < 3; i++)
             {
            // printf ("labelArray[%d] = %p \n",i,labelArray[i]);

               SgLabelSymbol* labelSymbol = labelArray[i];
               if (labelSymbol != NULL)
                  {
                 // Check if this is a SgLabelSymbol pointing to a SgNullStatement (usually, if not always, 
                 // because the reference to the label appeared before the statement containing the label)
                 // in the AST construction form OFP, I build SgNullStatements just to hold the numeric label.
                 // This is better than having the IR node carry around scratch space for storing such values 
                 // that would later not be relavant.
                    SgStatement* fortranStatement = labelSymbol->get_fortran_statement();
                    ROSE_ASSERT(fortranStatement != NULL);
                    if (isSgNullStatement(fortranStatement) != NULL)
                       {
                      // This is the most common case.
                      // int numericLabel = fortranStatement->get_numeric_label();
                         int numericLabel = labelSymbol->get_numeric_label_value();
                         printf ("Looking up SgLabelSymbol from symbol table for numeric label = %d \n",numericLabel);

                      // fortran labels have a scope in the function where they are defined, I think.
                         SgFunctionDeclaration* functionDeclaration = TransformationSupport::getFunctionDeclaration(fortranStatement);
                         ROSE_ASSERT(functionDeclaration != NULL);
                         SgFunctionDefinition* functionDefinition = functionDeclaration->get_definition();
                         ROSE_ASSERT(functionDefinition != NULL);

                         SgName name = StringUtility::numberToString(numericLabel);
                         printf ("Looking up name = %s \n",name.str());

                         SgLabelSymbol* labelSymbolFromSymbolTable = functionDefinition->lookup_label_symbol(name);
                         if (labelSymbolFromSymbolTable == NULL)
                            {
                              printf ("Error: labelSymbolFromSymbolTable == NULL \n");
                              functionDefinition->get_symbol_table()->print("Error: labelSymbolFromSymbolTable == NULL",V_SgLabelSymbol);
                            }
                         ROSE_ASSERT(labelSymbolFromSymbolTable != NULL);

                      // delete fortranStatement;
                      // delete labelSymbol;

                         labelArray[i] = NULL;

#if 0
                      // DQ (12/30/2007): This is now modified due to the introduction of the SgLabelRefExp.
                      // The SgLabelRefExp need not be reset since only the SgLabelSymbol is fixedup.

                         switch(i)
                            {
                              case 0: arithmeticIf->set_less_label(labelSymbolFromSymbolTable);    break;
                              case 1: arithmeticIf->set_equal_label(labelSymbolFromSymbolTable);   break;
                              case 2: arithmeticIf->set_greater_label(labelSymbolFromSymbolTable); break;

                              default:
                                 {
                                   printf ("Error: default reached i = %d \n",i);
                                   ROSE_ASSERT(false);
                                 }
                            }
#endif
                       }
                      else
                       {
                      // This label has already been set.
                       }
                  }
                 else
                  {
                    printf ("The SgLabelSymbol should have been built (if only so that the numeric label value could be communicated via a SgNullStatement) \n");
                    ROSE_ASSERT(false);
                  }
             }

       // printf ("Exiting in   SgArithmeticIfStatement fixup! \n");
       // ROSE_ASSERT(false);
        }
   }



