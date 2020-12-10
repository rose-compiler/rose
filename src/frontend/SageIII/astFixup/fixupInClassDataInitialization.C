
// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "fixupInClassDataInitialization.h"

void
fixupInClassDataInitialization( SgNode* node )
   {
  // DQ (7/7/2005): Introduce tracking of performance of ROSE.
     TimingPerformance timer ("Fixup class data member initialization:");

  // This simplifies how the traversal is called!
     FixupInClassDataInitialization inClassFixupTraversal;

  // I think the default should be preorder so that the interfaces would be more uniform
     inClassFixupTraversal.traverse(node,preorder);
   }

void
FixupInClassDataInitialization::visit (SgNode* node)
   {

  // CR (8/12/2020): This has been a continuing source of problems for unparsing Jovial files
  // whenever there is a variable declaration with initialization. Since I'm not at all sure that
  // the semantics of the language should match C++, I'm skipping this check for Jovial. The same
  // should probably be done for Fortran as the check isn't run when module *.rmod files are created and
  // unparsed but only when the original module files are later unparsed. At least this is true for Jovial
  // Compool files (and *.rcmp files).
  //
     if (SageInterface::is_Jovial_language())
        {
           return;
        }

     switch(node->variantT())
        {
          case V_SgVariableDeclaration:
             {
               SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(node);
               ROSE_ASSERT(variableDeclaration != NULL);

            // Check if this is a variable declaration inside of a class
               SgClassDefinition* classDefinition = isSgClassDefinition(variableDeclaration->get_parent());
               if (classDefinition != NULL )
                  {
                 // Check if it has an initializer (there should not be many of these!)
                    SgInitializedNamePtrList::iterator variable = variableDeclaration->get_variables().begin();
                    if ( (*variable)->get_initializer() != NULL )
                       {
                      // Check the type (looking for float or double) (should be uniform for all types)
                      // Check that it is non-static and const and modify to make it static const
                         SgType* variableType = (*variable)->get_typeptr();
                         ROSE_ASSERT(variableType != NULL);
                         SgModifierType* modifierType = isSgModifierType(variableType);
                         if (modifierType == NULL)
                            {
                           // The EDG/Sage connection ignores the const when the type is non-integral
                           // So "const double pi = 3.14;" will compile with EDG, but the EDG/Sage 
                           // connection only generates "double pi = 3.14;" (dropping the const).
                           // Since this is a bug in a non-standard usage, it is patch here, the dropping 
                           // of the "const" could be fixed in the EDG/Sage connection but I have not done so.
                           // It is not clear how seriously to treat a bug in a non-standard feature.

                           // printf ("This is not a modifier type (so it is not const!) \n");
                           // variableDeclaration->get_file_info()->display("Error: this is not a modifier type (so it is not const!)");

                              modifierType = new SgModifierType(variableType);
                              ROSE_ASSERT(modifierType != NULL);
                              modifierType->get_typeModifier().get_constVolatileModifier().setConst();

                           // printf ("DEBUGGING: just called new SgModifierType() modifierType = %p \n",modifierType);

#if DEBUG_SAGE_ACCESS_FUNCTIONS
                           // DQ (6/13/2007): New access function tests using DEBUG_SAGE_ACCESS_FUNCTIONS and 
                           // DEBUG_SAGE_ACCESS_FUNCTIONS_ASSERTION in sage3.h indicate this is required.
                              if ((*variable)->get_typeptr() != NULL)
                                 {
                                   printf ("Note: overwriting (*variable)->get_typeptr() = %p with NULL before assignment to modifierType = %p \n",
                                        (*variable)->get_typeptr(),modifierType);
                                   (*variable)->set_typeptr(NULL);
                                 }
#endif
                              (*variable)->set_typeptr(modifierType);
                            }
                         ROSE_ASSERT(modifierType != NULL);

                         // FMZ (12/18/2009): 
                         //    the flag may set in the SgVariableDeclaration instead of individual variable entry
                         if (variableDeclaration->get_declarationModifier().get_typeModifier().get_constVolatileModifier().isConst() ==false ) {
                                  ROSE_ASSERT (modifierType->get_typeModifier().get_constVolatileModifier().isConst());
                         }

                         if (modifierType->get_typeModifier().get_constVolatileModifier().isConst() == false)
                            {
                              printf ("Error: this should be a const variable (might be a fortran specific problem) \n");
                              variableDeclaration->get_file_info()->display("this should be a const variable");
                            }
                         ROSE_ASSERT (modifierType->get_typeModifier().get_constVolatileModifier().isConst() == true);
                         if (variableDeclaration->get_declarationModifier().get_storageModifier().isStatic() == false)
                            {
                           // Then modify this node to make it static
                           // printf ("Modify non-static const in-class initialization to be static \n");
                              variableDeclaration->get_declarationModifier().get_storageModifier().setStatic();
                            }
                           else
                            {
                           // printf ("Found static const in-class initialization (no modification required) \n");
                            }
                       }
                  }
               break;
             }

          case V_SgFloatVal:
          case V_SgDoubleVal:
             {
            // This whole subject is a non-standard feature of C++ which is allowed 
            // by EDG but not handled correctly in the EDG/Sage connection.
               SgValueExp* value = isSgValueExp(node);
               ROSE_ASSERT(value != NULL);

            // printf ("Found an float or double value expression ... \n");
               SgNode* parentOfValueExp = value->get_parent();

            // printf ("parentOfValueExp = %p = %s \n",parentOfValueExp,parentOfValueExp->sage_class_name());

               SgDotExp*   dotExpParentOfValueExp   = isSgDotExp(parentOfValueExp);
               SgArrowExp* arrowExpParentOfValueExp = isSgArrowExp(parentOfValueExp);
               if (dotExpParentOfValueExp != NULL)
                  {
                 // This is an illegal code construction such as x.3.14159 instead of just 3.14159
                 // get the parent and substitute the floating point constant value in its place
                    SgNode* parentOfProblemExpression = dotExpParentOfValueExp->get_parent();
                    ROSE_ASSERT(parentOfProblemExpression != NULL);
                 // printf ("parentOfProblemExpression = %s \n",parentOfProblemExpression->sage_class_name());
                    SgAssignInitializer* initializer = isSgAssignInitializer(parentOfProblemExpression);
                    ROSE_ASSERT(initializer != NULL);

#if DEBUG_SAGE_ACCESS_FUNCTIONS
                 // DQ (6/13/2007): New access function tests using DEBUG_SAGE_ACCESS_FUNCTIONS and 
                 // DEBUG_SAGE_ACCESS_FUNCTIONS_ASSERTION in sage3.h indicate this is required.
                    if (initializer->get_operand() != NULL)
                       {
                         printf ("Note: overwriting initializer->get_operand() = %p with NULL before assignment to value = %p \n",
                              initializer->get_operand(),value);
                         initializer->set_operand(NULL);
                       }
#endif
                    initializer->set_operand(value);
                  }

               if (arrowExpParentOfValueExp != NULL)
                  {
                 // This is an illegal code construction such as x.3.14159 instead of just 3.14159
                 // get the parent and substitute the floating point constant value in its place
                    SgNode* parentOfProblemExpression = arrowExpParentOfValueExp->get_parent();
                    ROSE_ASSERT(parentOfProblemExpression != NULL);
                 // printf ("parentOfProblemExpression = %s \n",parentOfProblemExpression->sage_class_name());

                 // Note that these two cases were all that were required to compile KULL, but one could 
                 // imagine more cases being required.  Since this is support for a non-standard C++ feature 
                 // in EDG and GNU g++ I don't feel compelled to handle it more robustly than is required to 
                 // compile KULL.
                    switch(parentOfProblemExpression->variantT())
                       {
                         case V_SgAssignInitializer:
                            {
                              SgAssignInitializer* initializer = isSgAssignInitializer(parentOfProblemExpression);
                              ROSE_ASSERT(initializer != NULL);

#if DEBUG_SAGE_ACCESS_FUNCTIONS
                           // DQ (6/13/2007): New access function tests using DEBUG_SAGE_ACCESS_FUNCTIONS and 
                           // DEBUG_SAGE_ACCESS_FUNCTIONS_ASSERTION in sage3.h indicate this is required.
                              if (initializer->get_operand() != NULL)
                                 {
                                   printf ("Note: overwriting initializer->get_operand() = %p with NULL before assignment to value = %p \n",
                                        initializer->get_operand(),value);
                                   initializer->set_operand(NULL);
                                 }
#endif
                              initializer->set_operand(value);
                              break;
                            }

                         case V_SgAssignOp:
                            {
                              SgAssignOp* assignmentOperator = isSgAssignOp(parentOfProblemExpression);
                              ROSE_ASSERT(assignmentOperator != NULL);
                           // I assume that the constant is on the rhs!
                              assignmentOperator->set_rhs_operand(value);
                              break;
                            }

                         default:
                            {
                              printf ("Error default reached parentOfProblemExpression = %s \n",parentOfProblemExpression->sage_class_name());
                              ROSE_ASSERT(false);
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

