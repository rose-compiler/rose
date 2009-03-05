#include "rose.h"

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

void
resetTypesInAST()
   {
     ResetTypes t;
     t.traverseMemoryPool();
   }

void
ResetTypes::visit(SgNode* node)
   {
  // The purpose of this traversal is to fixup the AST to share types more uniformally shared.
  // It appears that in the EDG/Sage translation some types are not shared properly, particularly 
  // SgNamedTypes.

#if 0
     printf ("##### ResetTypes::visit(node = %p = %s) \n",node,node->class_name().c_str());
#endif

     SgNamedType* namedType = isSgNamedType(node);
     if (namedType != NULL)
        {
          SgDeclarationStatement* declaration = namedType->get_declaration();
          ROSE_ASSERT(declaration != NULL);
          SgDeclarationStatement* definingDeclaration    = declaration->get_definingDeclaration();
          SgDeclarationStatement* nondefiningDeclaration = declaration->get_firstNondefiningDeclaration();
          if ( (definingDeclaration != NULL) && (nondefiningDeclaration != NULL) )
             {
            // printf ("ResetTypes::visit(): resetting the definition used in namedType = %p = %s \n",namedType,namedType->class_name().c_str());
#if DEBUG_SAGE_ACCESS_FUNCTIONS || 0
            // DQ (6/12/2007): New access function tests using DEBUG_SAGE_ACCESS_FUNCTIONS and 
            // DEBUG_SAGE_ACCESS_FUNCTIONS_ASSERTION in sage3.h indicate this is required.
               if (namedType->get_declaration() != NULL)
                  {
                    printf ("Note in ResetTypes::visit(): overwriting namedType->get_declaration() = %p with NULL before assignment to nondefiningDeclaration = %p \n",namedType->get_declaration(),nondefiningDeclaration);
                    namedType->set_declaration(NULL);
                  }
#endif
               namedType->set_declaration(nondefiningDeclaration);
             }
        }

#if 0
  // DQ (10/10/2007): Moved to after any possible use below (reported by Valentin).
  // Partial function types are used to build function types, but after we have used them then can be removed!
     if (isSgPartialFunctionType(node) != NULL)
        {
          delete node;
        }
#endif

     SgDeclarationStatement* declaration = isSgDeclarationStatement(node);
     if (declaration != NULL)
        {
          switch (declaration->variantT())
             {
               case V_SgClassDeclaration:
                  {
                    SgClassDeclaration* declaration            = isSgClassDeclaration(node);
                    SgClassDeclaration* definingDeclaration    = isSgClassDeclaration(declaration->get_definingDeclaration());
                    SgClassDeclaration* nondefiningDeclaration = isSgClassDeclaration(declaration->get_firstNondefiningDeclaration());
#if 0
                    printf ("ResetTypes::visit(): declaration = %p definingDeclaration = %p nondefiningDeclaration = %p \n",
                         declaration,definingDeclaration,nondefiningDeclaration);
#endif
                 // SgType* declarationType = declaration->get_type();
                    bool resetType = false;
                    if (definingDeclaration != NULL)
                       {
#if 0
                      // SgType* declarationType = declaration->get_type();
                         if (declaration->get_type() != definingDeclaration->get_type())
                            {
                              printf ("Error: declaration->get_type()         = %p = %s \n",declaration->get_type(),declaration->get_type()->class_name().c_str());
                              printf ("Error: definingDeclaration->get_type() = %p = %s \n",definingDeclaration->get_type(),definingDeclaration->get_type()->class_name().c_str());
                            }
#endif
                      // Set the name to that in the defining declaration
                      // DQ (6/12/2007): When this is executed we have a bug in ROSE!
#if 1
                      // DQ (6/12/2007): New access function tests using DEBUG_SAGE_ACCESS_FUNCTIONS and 
                      // DEBUG_SAGE_ACCESS_FUNCTIONS_ASSERTION in sage3.h indicate this is required.
                      // Avoid calling definingDeclaration->get_type() multiple times.
                      // printf ("definingDeclaration = %p = %s \n",definingDeclaration,definingDeclaration->class_name().c_str());
                         SgClassType* newType = definingDeclaration->get_type();
#if DEBUG_SAGE_ACCESS_FUNCTIONS
                         if (declaration->get_type() != NULL && newType != NULL)
                            {
                              printf ("Note in ResetTypes::visit(): overwriting declaration->get_type() = %p with NULL before assignment to definingDeclaration->get_type() = %p \n",declaration->get_type(),newType);
                              declaration->set_type(NULL);
                            }
#endif
                         declaration->set_type(newType);
#else
                      // DQ (6/12/2007): this version calls get_type too many times and causes a subtle bug which I don't understand.
#if DEBUG_SAGE_ACCESS_FUNCTIONS || 0
                      // DQ (6/12/2007): New access function tests using DEBUG_SAGE_ACCESS_FUNCTIONS and 
                      // DEBUG_SAGE_ACCESS_FUNCTIONS_ASSERTION in sage3.h indicate this is required.
                         SgType* newType = definingDeclaration->get_type();
                         if (declaration->get_type() != NULL && definingDeclaration->get_type() != NULL)
                            {
                              printf ("Note in ResetTypes::visit(): overwriting declaration->get_type() = %p with NULL before assignment to definingDeclaration->get_type() = %p \n",declaration->get_type(),definingDeclaration->get_type());
                              declaration->set_type(NULL);
                            }
#endif
                         declaration->set_type(definingDeclaration->get_type());
#endif

                      // printf ("ResetTypes::visit(): resetType == true: Can we delete declarationType = %p \n",declarationType);
                      // delete declarationType;
                         ROSE_ASSERT(declaration->get_type() == definingDeclaration->get_type());

                         resetType = true;
                       }

                    if ( (resetType == false) && (nondefiningDeclaration != NULL) )
                       {
#if 0
                         if (declaration->get_type() != nondefiningDeclaration->get_type())
                            {
                              printf ("Error: declaration->get_type()            = %p = %s \n",declaration->get_type(),declaration->get_type()->class_name().c_str());
                              printf ("Error: nondefiningDeclaration->get_type() = %p = %s \n",nondefiningDeclaration->get_type(),nondefiningDeclaration->get_type()->class_name().c_str());
                            }
#endif
                      // Set the name to that in the defining declaration
                         SgClassType* newType = nondefiningDeclaration->get_type();
#if DEBUG_SAGE_ACCESS_FUNCTIONS
                         if (declaration->get_type() != NULL && newType != NULL)
                            {
                              printf ("Note in ResetTypes::visit(): overwriting declaration->get_type() = %p with NULL before assignment to nondefiningDeclaration->get_type() = %p \n",declaration->get_type(),newType);
                              declaration->set_type(NULL);
                            }
#endif
                      // declaration->set_type(nondefiningDeclaration->get_type());
                         declaration->set_type(newType);
                      // printf ("ResetTypes::visit(): resetType == false: Can we delete declarationType = %p \n",declarationType);
                      // delete declarationType;
                      // ROSE_ASSERT(declaration->get_type() == definingDeclaration->get_type());
                         ROSE_ASSERT(declaration->get_type() == nondefiningDeclaration->get_type());
                         resetType = true;
                       }

#if 0
                    if ( (definingDeclaration != NULL) && (nondefiningDeclaration != NULL) )
                       {
                         if (definingDeclaration->get_type() != nondefiningDeclaration->get_type())
                            {
                              SgClassType* classTypeDefining    = definingDeclaration->get_type();
                              SgClassType* classTypeNondefining = nondefiningDeclaration->get_type();
                              printf ("classTypeDefining = %p classTypeNondefining = %p \n",classTypeDefining,classTypeNondefining);
                              ROSE_ASSERT(classTypeDefining    != NULL);
                              ROSE_ASSERT(classTypeNondefining != NULL);

                              ROSE_ASSERT(classTypeDefining->get_typedefs()    != NULL);
                              ROSE_ASSERT(classTypeNondefining->get_typedefs() != NULL);
                              int numberOfTypedefsDefining    = classTypeDefining->get_typedefs()->get_typedefs().size();
                              int numberOfTypedefsNondefining = classTypeNondefining->get_typedefs()->get_typedefs().size();
                              printf ("numberOfTypedefsDefining = %d numberOfTypedefsNondefining = %d \n",numberOfTypedefsDefining,numberOfTypedefsNondefining);
                              if (numberOfTypedefsDefining == 0)
                                 {
                                   definingDeclaration->set_type(nondefiningDeclaration->get_type());
                                   delete classTypeDefining;
                                 }
                                else
                                 {
                                   if (numberOfTypedefsNondefining == 0)
                                      {
                                        nondefiningDeclaration->set_type(definingDeclaration->get_type());
                                        delete classTypeNondefining;
                                      }
                                     else
                                      {
                                        printf ("Error: ResetTypes::visit(node = %p = %s) \n",node,node->class_name().c_str());
                                        printf ("Warning: Must merge both SgTypedefSeq objects \n");
                                        ROSE_ASSERT(false);
                                      }
                                 }
                            }                         
                         ROSE_ASSERT(definingDeclaration->get_type() == nondefiningDeclaration->get_type());
                       }
#endif
                    break;
                  }

               case V_SgFunctionDeclaration:
                  {
                    SgFunctionDeclaration* declaration            = isSgFunctionDeclaration(node);
                    SgFunctionDeclaration* definingDeclaration    = isSgFunctionDeclaration(declaration->get_definingDeclaration());
                    SgFunctionDeclaration* nondefiningDeclaration = isSgFunctionDeclaration(declaration->get_firstNondefiningDeclaration());
#if 1
                    if (definingDeclaration != NULL)
                       {
                         if (declaration->get_type() != definingDeclaration->get_type())
                            {
                           // DQ (3/3/2009): Make this conditional upon it being a transformation, since they cause types to be build redundantly in the SageBuilder function (I think).
                              if (declaration->get_file_info()->isTransformation() == false)
                                 {
                                   printf ("Error: types in declaration = %p = %s and definingDeclaration = %p not shared \n",declaration,declaration->class_name().c_str(),definingDeclaration);
                                   declaration->get_file_info()->display("Error: types in declaration and definingDeclaration are not shared");
                                   printf ("definingDeclaration->unparseToString() = %s \n",definingDeclaration->unparseToString().c_str());
                                 }

                              if (definingDeclaration->get_type() != NULL)
                                 {
                                // Use the type held in the defining declaration
                                // delete declaration->get_type();
                                   declaration->set_type(definingDeclaration->get_type());
                                 }
                            }
                         ROSE_ASSERT(declaration->get_type() == definingDeclaration->get_type());
                       }

                    if (nondefiningDeclaration != NULL && definingDeclaration != NULL)
                       {
                         if (declaration->get_type() != nondefiningDeclaration->get_type())
                            {
                              if (nondefiningDeclaration->get_type() != NULL)
                                 {
                                // delete nondefiningDeclaration->get_type();
                                   nondefiningDeclaration->set_type(definingDeclaration->get_type());
                                 }
                                else
                                 {
                                   if (definingDeclaration->get_type() != NULL)
                                      {
                                     // delete definingDeclaration->get_type();
                                        definingDeclaration->set_type(nondefiningDeclaration->get_type());
                                      }
                                     else
                                      {
                                        printf ("Error: ResetTypes::visit(node = %p = %s) \n",node,node->class_name().c_str());
                                        printf ("Warning: get_type() == NULL for both defining and nondefining declarations \n");
                                        ROSE_ASSERT(false);
                                      }
                                 }
                            }
                         ROSE_ASSERT(declaration->get_type() == nondefiningDeclaration->get_type());
                       }

                    if ( (definingDeclaration != NULL) && (nondefiningDeclaration != NULL) )
                       {
                         if (definingDeclaration->get_type() != NULL)
                            {
                           // Use the type held in the defining declaration
                              if (nondefiningDeclaration->get_type() != NULL)
                                 {
                                // delete nondefiningDeclaration->get_type();
                                 }
                              nondefiningDeclaration->set_type(definingDeclaration->get_type());
                            }
                         ROSE_ASSERT(definingDeclaration->get_type() == nondefiningDeclaration->get_type());
                       }
#endif
                    break;
                  }

               default:
                  {
                    break;
                  }
             }
        }

  // DQ (10/10/2007): Moved to after any possible use above (reported by Valentin).
  // Partial function types are used to build function types, but after we have used them then can be removed!
     if (isSgPartialFunctionType(node) != NULL)
        {
          delete node;
        }
   }

