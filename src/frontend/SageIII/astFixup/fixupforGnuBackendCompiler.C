// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "fixupforGnuBackendCompiler.h"
void
fixupforGnuBackendCompiler( SgNode* node )
   {
  // DQ (7/7/2005): Introduce tracking of performance of ROSE.
     TimingPerformance timer ("Fixup for generation of GNU compatable code:");

  // This simplifies how the traversal is called!
     FixupforGnuBackendCompiler GnuFixupTraversal;

  // I think the default should be preorder so that the interfaces would be more uniform
     GnuFixupTraversal.traverse(node,postorder);
   }

void
FixupforGnuBackendCompiler::visit (SgNode* node)
   {
  // Convert:
  //      SgVariableDeclaration->SgInitializedName->SgConstructorInitializer->SgExprListExp->
  //                                                SgConstructorInitializer->SgExprListExp->
  //                                                SgConstructorInitializer->SgExprListExp->X
  // To:
  //      SgVariableDeclaration->SgInitializedName->SgConstructorInitializer->SgExprListExp->X
  // So rewrite:
  //      SgExprListExp->SgConstructorInitializer->SgExprListExp->X 
  // to 
  //      SgExprListExp->X

     switch(node->variantT())
        {
          case V_SgExprListExp:
             {
               SgExprListExp* expressionListExp = isSgExprListExp(node);
               ROSE_ASSERT(expressionListExp != NULL);

            // printf ("Found a SgExprListExp \n");

            // Transformation only applies for single argument copy constructors (I think)
            // and only to those inside of constructors (so leave one level of constructor!)
               SgConstructorInitializer* outerConstructorInitializer = isSgConstructorInitializer(expressionListExp->get_parent());
               if ( (outerConstructorInitializer != NULL) && (expressionListExp->get_expressions().size() == 1) )
                  {
                    SgExpression* argument = *(expressionListExp->get_expressions().begin());
                    ROSE_ASSERT(argument != NULL);

                    SgConstructorInitializer* innerConstructorInitializer = isSgConstructorInitializer(argument);
                    if (innerConstructorInitializer != NULL)
                       {
                      // printf ("Found a second level SgConstructorInitializer \n");
                      // Why are the types int in this case!
                      // bool constructorTypesMatch = outerConstructorInitializer->get_type() == innerConstructorInitializer->get_type();
                         bool constructorMatch = outerConstructorInitializer->get_declaration() == innerConstructorInitializer->get_declaration();

#if PRINT_DEVELOPER_WARNINGS
                         printf ("Warning: calling \"outerConstructorInitializer->get_declaration()->unparseToString()\" fails in ASTFixup \n");
                      // This causes and error: attachPreprocessingInfo.C:262: void AttachPreprocessingInfoTreeTrav::
                      //      setupPointerToPreviousNode(SgLocatedNode*): Assertion `parentStatement != __null' failed.
                      // string unparsedCode = outerConstructorInitializer->get_declaration()->unparseToString();
                      // This is OK
                      // string unparsedCode = outerConstructorInitializer->get_type()->unparseToString();
#endif

#if 0
                         printf ("outerConstructorInitializer = %p get_type() = %p = %s function declaration = %p = %s \n",
                              outerConstructorInitializer,
                              outerConstructorInitializer->get_type(),
                              outerConstructorInitializer->get_type()->unparseToString().c_str(),
                              outerConstructorInitializer->get_declaration(),
                              outerConstructorInitializer->get_declaration()->unparseToString().c_str());
                         printf ("innerConstructorInitializer = %p get_type() = %p = %s function declaration = %p = %s \n",
                              innerConstructorInitializer,
                              innerConstructorInitializer->get_type(),
                              innerConstructorInitializer->get_type()->unparseToString().c_str(),
                              innerConstructorInitializer->get_declaration(),
                              innerConstructorInitializer->get_declaration()->unparseToString().c_str());
#endif
                         if (constructorMatch == true)
                            {
#if PRINT_DEVELOPER_WARNINGS
                              if (outerConstructorInitializer->get_type() == innerConstructorInitializer->get_type())
                                 {
                                // DQ (3/25/2005): This is something to investigate more deeply at some point!
                                   printf ("Warning in AST Fixup: constructors don't match but types are reported as being the same! \n");
                                 }
#endif
                              SgExprListExp* innerExpressionListExp = isSgExprListExp(innerConstructorInitializer->get_args());
                              if ( (innerExpressionListExp != NULL) && (innerExpressionListExp->get_expressions().size() == 1) )
                                 {
                                   SgExpression* targetArgument = *(innerExpressionListExp->get_expressions().begin());
                                   ROSE_ASSERT(targetArgument != NULL);

                                // printf ("Found a target expression = %s \n",targetArgument->sage_class_name());

                                // We might want to delete this IR node (but skip that for now).
                                // SgExpression* elementToDelete = expressionListExp->get_expressions().front();
                                // delete elementToDelete;

                                   ROSE_ASSERT(expressionListExp->get_expressions().empty() == false);

                                // Remove the current element and put in the targetArgument!

                                // DQ (9/25/2007): Moved uniformally from std::list to std::vector in ROSE, also since the list is length == 1, pop_front() can be replaced with pop_back().
                                // expressionListExp->get_expressions().pop_front();
                                // expressionListExp->get_expressions().erase(expressionListExp->get_expressions().begin());
                                   expressionListExp->get_expressions().pop_back();

                                // DQ (9/25/2007): Moved uniformally from std::list to std::vector in ROSE, also since the list is length == 1, pop_front() can be replaced with pop_back().
                                // expressionListExp->get_expressions().push_front(targetArgument);
                                   expressionListExp->get_expressions().push_back(targetArgument);

                                   ROSE_ASSERT(expressionListExp->get_expressions().size() == 1);

                                // reset the parent to make this make sense
                                   targetArgument->set_parent(expressionListExp);
                                 }
                            }
                           else
                            {
                           // printf ("This is a required constructor call (can't be optimized away) \n");
                            }
                       }
                  }
               break;
             }

          default:
             {
            // Nothing to do here ...
             }
        }
   }


