// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "initializeExplicitScopeData.h"
// DQ (11/29/2004): initialize any uninitialized explicitly stored scope data.
void
initializeExplicitScopes ( SgNode *node)
   {
  // DQ (7/7/2005): Introduce tracking of performance of ROSE.
     TimingPerformance resetParentPointerTimer ("Initialize IR nodes containing explicit scope data member:");

  // DQ (7/26/2007): Modified to use memory pool traversal
  // initializeExplicitScopeSupport (node);
     InitializeExplicitScopes t;
     t.traverseMemoryPool();
   }

// DQ (11/29/2004): initialize any uninitialized explicitly stored scope data (hidden in a type!)
void
initializeExplicitScopeDataHiddenInType ( SgType* typeNode )
   {
  // Function forward declaration
  // void initializeExplicitScopeData ( SgNode *node);

     ROSE_ASSERT(typeNode != NULL);

#if 0
     printf ("#### initializeExplicitScopeDataHiddenInType(typeNode = %p = %s) \n",typeNode,typeNode->sage_class_name());
#endif

  // Strip off * and & type modifiers
     typeNode = typeNode->findBaseType();

     switch(typeNode->variantT())
        {
          case V_SgClassType:
             {
            // Find any code (inside of a SgClassDefinition) that is buried within a SgClassType.
            // Examples of such code is: "struct X { int x; } Xvar;" or "typedef struct X { int x; } Xtype;".
               SgClassType* classType = isSgClassType (typeNode);
               SgDeclarationStatement* declarationStatement = classType->get_declaration();
               ROSE_ASSERT(declarationStatement != NULL);
               SgClassDeclaration* classDeclaration = isSgClassDeclaration(declarationStatement);
               ROSE_ASSERT(classDeclaration != NULL);
            // initializeExplicitScopeSupport(classDeclaration);
            // initializeExplicitScopes(classDeclaration);
               break;
             }

       // This is not a realistic case since no IR nodes containing an explicit scope can be 
       // referenced inside of an enum (I think! Maybe pehaps an initializer?)
          case V_SgEnumType:
             {
            // Find any code (inside of a SgEnumDeclaration) that is buried within a SgEnumType.
            // Examples of such code is: "enum X { value = 0; } Xvar;" or "typedef enum X { value = 0; } Xtype;".
            // This is not a significant island of untraversed code except that the parent pointers 
            // are not set and we want to set them all!
               SgEnumType* enumType = isSgEnumType (typeNode);
               SgDeclarationStatement* declarationStatement = enumType->get_declaration();
               ROSE_ASSERT(declarationStatement != NULL);
               SgEnumDeclaration* enumDeclaration = isSgEnumDeclaration(declarationStatement);
               ROSE_ASSERT(enumDeclaration != NULL);
            // initializeExplicitScopeSupport(enumDeclaration);
            // initializeExplicitScopes(enumDeclaration);
               break;
             }

          default:
             {
            // Don't worry about any other nodes!
               break;
             }
        }
   }

// DQ (11/29/2004): initialize any uninitialized explicitly stored scope data.
// void initializeExplicitScopeSupport ( SgNode *node)
void
InitializeExplicitScopes::visit ( SgNode *node)
   {
  // cerr << " visiting " << node->sage_class_name() << " : " << node << endl;

     switch(node->variantT())
        {
          case V_SgInitializedName:
             {
            // DQ (11/28/2004): Verify that newly added explicit scope data members are initialized properly (should be a valid pointer)
               SgInitializedName* initializedName = isSgInitializedName(node);
               ROSE_ASSERT(initializedName != NULL);
            // printf ("Testing initializedName->get_scope() \n");
               if (initializedName->get_scope() == NULL)
                  {
#if 0
                    printf ("Found: SgInitializedName with explicit scope unset at %p = %s \n",
                         initializedName,initializedName->sage_class_name());
                    ROSE_ASSERT(initializedName->get_file_info() != NULL);
                    initializedName->get_file_info()->display("Found: SgInitializedName with explicit scope unset");
#endif
#if 0
                    printf ("Set scope to initializedName->get_parent() = %s \n",initializedName->get_parent()->sage_class_name());
#endif
                    SgScopeStatement* scope = NULL;
                    SgNode* parentNode = initializedName->get_parent();

                    if (parentNode == NULL)
                       {
                         printf ("Error: parentNode == NULL for initializedName = %p = %s \n",initializedName,initializedName->get_name().str());
                       }
                    ROSE_ASSERT(parentNode != NULL);
                    switch (parentNode->variantT())
                       {
                         case V_SgFunctionParameterList:
                            {
                              SgFunctionParameterList* parameterList = isSgFunctionParameterList(parentNode);
                              ROSE_ASSERT(parameterList != NULL);
                              SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(parameterList->get_parent());
                              ROSE_ASSERT(functionDeclaration != NULL);
                           // printf ("functionDeclaration->get_definition() = %p \n",functionDeclaration->get_definition());

                           // Check if this function declaration has a function definition
                              if (functionDeclaration->get_definition() == NULL)
                                 {
                                   scope = functionDeclaration->get_scope();
                                 }
                                else
                                 {
                                   scope = functionDeclaration->get_definition();
                                 }

                              ROSE_ASSERT(scope != NULL);
                              break;
                            }

                         case V_SgVariableDeclaration:
                            {
                           // This is the most common case.  Allowing this to be initialized here permits the 
                           // scope to be optionally NOT specified explicitly within the construction of a 
                           // variable declaration.
                              SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(parentNode);
                              ROSE_ASSERT(variableDeclaration != NULL);

                           // DQ (7/4/2005): Call the base class get_scope() (which is now implemented to look 
                           // at the SgInitializedName we are trying to set the scope for currently)!
                           // scope = variableDeclaration->get_scope();
                           // SgDeclarationStatement* declarationStatement = variableDeclaration;
                           // scope = declarationStatement->get_scope();
                              SgNode* parentNode = variableDeclaration->get_parent();
                              ROSE_ASSERT(parentNode != NULL);
                              SgScopeStatement *parentScopeStatement = isSgScopeStatement(parentNode);
                              ROSE_ASSERT(parentScopeStatement != NULL);
                              if (isSgGlobal(parentScopeStatement) != NULL)
                                 {
                                   printf ("Computation of scope in variable declaration is SgGlobal (might be incorrect) \n");
                                   variableDeclaration->get_file_info()->display("Computation of scope in variable declaration is SgGlobal (might be incorrect)");
                                 }
                              scope = parentScopeStatement;
                              ROSE_ASSERT(scope != NULL);
                              break;
                            }

                         case V_SgCtorInitializerList:
                            {
                              SgCtorInitializerList* initializerList = isSgCtorInitializerList(parentNode);
                              ROSE_ASSERT(initializerList != NULL);
                              SgMemberFunctionDeclaration* memberFunctionDeclaration = isSgMemberFunctionDeclaration(initializerList->get_parent());
                              ROSE_ASSERT(memberFunctionDeclaration != NULL);
                              //printf ("memberFunctionDeclaration->get_definition() = %p \n",memberFunctionDeclaration->get_definition());

                           // Check if this function declaration has a function definition
                              if (memberFunctionDeclaration->get_definition() == NULL)
                                 {
                                   scope = memberFunctionDeclaration->get_scope();
                                 }
                                else
                                 {
                                   scope = memberFunctionDeclaration->get_definition();
                                 }

                              ROSE_ASSERT(scope != NULL);
                              break;
                            }

                         case V_SgProcedureHeaderStatement:
                            {
                              SgProcedureHeaderStatement* declaration = isSgProcedureHeaderStatement(parentNode);
                              ROSE_ASSERT(declaration != NULL);

                              SgNode* parentNode = declaration->get_parent();
                              ROSE_ASSERT(parentNode != NULL);
                              SgScopeStatement *parentScopeStatement = isSgScopeStatement(parentNode);
                              ROSE_ASSERT(parentScopeStatement != NULL);
                              if (isSgGlobal(parentScopeStatement) != NULL)
                                 {
                                   printf ("Computation of scope in SgProcedureHeaderStatement declaration is SgGlobal (might be incorrect) \n");
                                   declaration->get_file_info()->display("Computation of scope in variable declaration is SgGlobal (might be incorrect)");
                                 }
                              scope = parentScopeStatement;
                              ROSE_ASSERT(scope != NULL);
                              break;
                            }

                         default:
                            {
                              printf ("Error: default reached initializedName->get_parent() = %s \n",
                                   initializedName->get_parent()->sage_class_name());
                              ROSE_ASSERT(false);
                            }
                       }

                 // Set the scope to the parent if it is not already set!
                    ROSE_ASSERT(scope != NULL);
                    initializedName->set_scope(scope);
                  }
               ROSE_ASSERT(initializedName->get_scope() != NULL);
               break;
             }

          case V_SgClassDeclaration:
          case V_SgTemplateInstantiationDecl:
             {
            // DQ (11/28/2004): Verify that newly added explicit scope data members are initialized properly (should be a valid pointer)
               SgClassDeclaration* classDeclaration = isSgClassDeclaration(node);
               ROSE_ASSERT(classDeclaration != NULL);
               if (classDeclaration->get_scope() == NULL)
                  {
#if PRINT_DEVELOPER_WARNINGS
                    printf ("***** Warning: SgClassDeclaration with explicit scope unset at %p = %s \n",
                         classDeclaration,classDeclaration->sage_class_name());
#endif
                    SgNode* parentNode = classDeclaration->get_parent();
                    ROSE_ASSERT(parentNode != NULL);
                    SgScopeStatement* parentScope = isSgScopeStatement(parentNode);
                    ROSE_ASSERT(parentScope != NULL);
#if PRINT_DEVELOPER_WARNINGS
                    printf ("***** Set scope to parent = %p = %s \n",parentScope,parentScope->sage_class_name());
#endif
                    classDeclaration->set_scope(parentScope);

                 // DQ (8/28/2006): In some rare cases the scope is left unset in the EDG/SageIII translation 
                 // and we are forced to set the scope here, when this is done we have to reset the fixupScope 
                 // flag to mark it as having been done! See test2006_124.C for an example of this.
                    classDeclaration->set_fixupScope(false);
                  }
               ROSE_ASSERT(classDeclaration->get_scope() != NULL);
               break;
             }

          case V_SgFunctionDeclaration:
          case V_SgMemberFunctionDeclaration:
          case V_SgTemplateInstantiationFunctionDecl:
          case V_SgTemplateInstantiationMemberFunctionDecl:
             {
            // DQ (11/28/2004): Verify that newly added explicit scope data members are initialized properly (should be a valid pointer)
               SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(node);
               ROSE_ASSERT(functionDeclaration != NULL);
#if 0
               if (functionDeclaration->get_scope() == NULL)
                  {
                 // DQ (10/22/2007): This can be a false positive. There are reasons (such a friend functions) why this can be true, 
                 // so it is interesting, but not serious.
                    printf ("Error: SgFunctionDeclaration with explicit scope unset at %p = %s \n",
                         functionDeclaration,functionDeclaration->sage_class_name());
                  }
#endif
               SgMemberFunctionDeclaration* memberFunctionDeclaration = isSgMemberFunctionDeclaration(functionDeclaration);
               if (memberFunctionDeclaration != NULL)
                  {
                    if (memberFunctionDeclaration->get_scope() == NULL)
                       {
                      // DQ (10/12/2007): This review of the semantics of get_associatedClassDeclaration() appears fine!
                      // This should work even when the memberFunctionDeclaration is associated with a pointer to a member 
                      // function and the class definition has not been declared.
                         SgClassDeclaration* classDeclaration = memberFunctionDeclaration->get_associatedClassDeclaration();
                         ROSE_ASSERT(classDeclaration != NULL);

                         SgClassDeclaration* definingClassDeclaration = isSgClassDeclaration(classDeclaration->get_definingDeclaration());
                      // ROSE_ASSERT(definingClassDeclaration != NULL);
                         if (definingClassDeclaration == NULL)
                            {
                           // DQ (7/31/2007): If the class is not defined and we have a member function pointer 
                           // of this class type then we can't set the scope, so we use the SgGlobal case.
#if PRINT_DEVELOPER_WARNINGS
                              printf ("Can't define class scope for classDeclaration = %p = %s = %s \n",
                                   memberFunctionDeclaration,memberFunctionDeclaration->class_name().c_str(),
                                   SageInterface::get_name(memberFunctionDeclaration).c_str());

                              memberFunctionDeclaration->get_startOfConstruct()->display("memberFunctionDeclaration: debug");
#endif
                              SgGlobal* globalScope = TransformationSupport::getGlobalScope(classDeclaration);
                              ROSE_ASSERT(globalScope != NULL);
                              memberFunctionDeclaration->set_scope(globalScope);
                            }
                           else
                            {
                              SgClassDefinition* classDefinition = definingClassDeclaration->get_definition();
                              ROSE_ASSERT(classDefinition != NULL);
                              memberFunctionDeclaration->set_scope(classDefinition);
                            }
#if 0
                         printf ("Fixed up memberFunctionDeclaration = %p = %s = %s \n",
                              memberFunctionDeclaration,memberFunctionDeclaration->class_name().c_str(),
                              SageInterface::get_name(memberFunctionDeclaration).c_str());
#endif
                       }
                  }

               ROSE_ASSERT(functionDeclaration->get_scope() != NULL);
            // printf ("resetting explicit scopes: function name = %s \n",functionDeclaration->get_name().str());
               break;
             }

          case V_SgTemplateDeclaration:
             {
            // DQ (11/28/2004): Verify that newly added explicit scope data members are initialized properly (should be a valid pointer)
               SgTemplateDeclaration* templateDeclaration = isSgTemplateDeclaration(node);
               ROSE_ASSERT(templateDeclaration != NULL);
               if (templateDeclaration->get_scope() == NULL)
                  {
                 // printf ("Warning: SgTemplateDeclaration with explicit scope unset at %p = %s \n",
                 //      templateDeclaration,templateDeclaration->sage_class_name());
                    SgNode* parentNode = templateDeclaration->get_parent();
                    ROSE_ASSERT(parentNode != NULL);
                    SgScopeStatement* parentScope = isSgScopeStatement(parentNode);
                    ROSE_ASSERT(parentScope != NULL);
                 // printf ("Set scope to parent = %p = %s \n",parentScope,parentScope->sage_class_name());
                    templateDeclaration->set_scope(parentScope);
                  }
               ROSE_ASSERT(templateDeclaration->get_scope() != NULL);
               break;
             }

          case V_SgTypedefDeclaration:
             {
            // DQ (11/28/2004): Verify that newly added explicit scope data members are initialized properly (should be a valid pointer)
               SgTypedefDeclaration* typedefDeclaration = isSgTypedefDeclaration(node);
               ROSE_ASSERT(typedefDeclaration != NULL);

            // DQ (6/28/2005): Now evaluate the consistancy of the preset scope if it is SgGlobal,
            // since these are sometimes set incorrectly to a default value of SgGlobal.
               SgScopeStatement* currentScope = typedefDeclaration->get_scope();
               if (currentScope == NULL)
                  {
                    printf ("Error: SgTypedefDeclaration with explicit scope unset at %p = %s \n",
                         typedefDeclaration,typedefDeclaration->sage_class_name());
                  }
               ROSE_ASSERT(currentScope != NULL);

               if ( isSgGlobal(currentScope) != NULL )
                  {
#if 0
                    printf ("Invistigating possibly incorrectly set typedef scope %p = %s \n",
                         typedefDeclaration,typedefDeclaration->get_name().str());
                    typedefDeclaration->get_file_info()->display("Invistigating possibly incorrectly set typedef scope");
#endif
                    ROSE_ASSERT(typedefDeclaration->get_parent() != NULL);
                    SgScopeStatement* parentScope = isSgScopeStatement(typedefDeclaration->get_parent());
                    if (parentScope == NULL)
                       {
                         printf ("typedefDeclaration->get_parent() = %p = %s \n",typedefDeclaration->get_parent(),typedefDeclaration->get_parent()->class_name().c_str());
                         printf ("typedefDeclaration->get_parent()->get_parent() = %p = %s \n",typedefDeclaration->get_parent()->get_parent(),typedefDeclaration->get_parent()->get_parent()->class_name().c_str());
                         printf ("typedefDeclaration->get_parent()->get_parent() = %p = %s \n",typedefDeclaration->get_parent()->get_parent()->get_parent(),typedefDeclaration->get_parent()->get_parent()->get_parent()->class_name().c_str());
                         typedefDeclaration->get_parent()->get_parent()->get_file_info()->display("Error: typedefDeclaration->get_parent()->get_parent(): debug");
                         typedefDeclaration->get_parent()->get_parent()->get_parent()->get_file_info()->display("Error: typedefDeclaration->get_parent()->get_parent()->get_parent(): debug");
                       }
                    ROSE_ASSERT(parentScope != NULL);
#if 0
                    printf ("     parentScope = %p = %s \n",parentScope,parentScope->sage_class_name());
#endif

                 // DQ (6/29/2005): Make this a warning for now while we try to get back to compileing KULL
                    if (currentScope != parentScope)
                       {
                         printf ("Warning in ASTFixes: typedef = %p currentScope (%s) != parentScope (%s) (ignoring) \n",
                              typedefDeclaration,currentScope->sage_class_name(),parentScope->sage_class_name());
                      // parentScope->get_file_info()->display("typedef's currentScope != parentScope");
                       }

                 // DQ (7/4/2005): fixup the scope to match that of the parent (which appears to be more accurate)!
                    if (currentScope != parentScope)
                       {
                      // Make sure that the parentScope is not the global scope (since there is only one global scope 
                      // and the current scope is global scope and the parent scope is different than the current 
                      // scope this should always be true).
                         ROSE_ASSERT ( isSgGlobal(parentScope) == NULL );
                         typedefDeclaration->set_scope(parentScope);

                      // recompute the current scope
                         currentScope = typedefDeclaration->get_scope();
                         ROSE_ASSERT(currentScope == parentScope);
                       }
                    ROSE_ASSERT(currentScope == parentScope);
                  }
               break;
             }

          case V_SgVariableDeclaration:
             {
            // Handle case of island of code hidden in class definition within a variable declaration!

            // printf ("Found a SgVariableDeclaration = %p looking for islands of untraversed AST ... \n",node);
               SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(node);
               ROSE_ASSERT(variableDeclaration != NULL);

            // variableDeclaration->get_file_info()->display("case V_SgVariableDeclaration: variableDeclaration");

            // We only want to traverse the base type (since all "*" and "&" are associated
            // with the variables in this variable list, e.g. list of SgInitializedName objects)
               SgInitializedNamePtrList & variableList = variableDeclaration->get_variables();
               if (variableList.empty() == true)
                  {
                    printf ("Error: empty variable list in variableDeclaration = %p \n",variableDeclaration);
                  }
               ROSE_ASSERT(variableList.size() > 0);
               SgInitializedName* firstVariable = *(variableList.begin());
               ROSE_ASSERT(firstVariable != NULL);
               SgType* baseType = firstVariable->get_typeptr();
               if (baseType == NULL)
                  {
                    printf ("Error: variableDeclaration = %p firstVariable = %p = %s \n",variableDeclaration,firstVariable,SageInterface::get_name(firstVariable).c_str());
                  }
               ROSE_ASSERT(baseType != NULL);

               initializeExplicitScopeDataHiddenInType(baseType);

#if 0
            // DQ (7/4/2005): The scope at a SgVariableDeclaration is take from the SgInitializedName objects
            // these have not been set yet in this preorder traversal of the AST.  Also it makes no sense to 
            // set the scope on a SgVariableDeclaration becuase the scope is only explicitly stored in the 
            // SgInitializedName objects in the variable declaration's SgInitializedNamePtrList.

            // DQ (6/28/2005): Now evaluate the consistancy of the preset scope if it is SgGlobal,
            // since these are sometimes set incorrectly to a default value of SgGlobal.
            // Note that the SgVariableDeclaration::get_scope() gets its scope from the attached SgInitializedName 
            // object (the first one). And that the SgInitializedName objects will later be set to the value of 
            // SgVariableDeclaration::get_scope().  Thus this is a test of the correctness of the setting on the 
            // SgInitializedName object, but it sets the value on the 
            // printf ("Calling get_scope on variableDeclaration \n");
               SgScopeStatement* currentScope = variableDeclaration->get_scope();
               if (currentScope == NULL)
                  {
                    printf ("Error: SgVariableDeclaration with explicit scope unset at %p = %s \n",
                         variableDeclaration,variableDeclaration->sage_class_name());
                  }
               ROSE_ASSERT(currentScope != NULL);

               if ( isSgGlobal(currentScope) != NULL )
                  {
#if 0
                    printf ("Invistigating possibly incorrectly set typedef scope %p = %s \n",
                         variableDeclaration,variableDeclaration->get_name().str());
                    typedefDeclaration->get_file_info()->display("Invistigating possibly incorrectly set typedef scope");
#endif
                    SgScopeStatement* parentScope = isSgScopeStatement(variableDeclaration->get_parent());
                    ROSE_ASSERT(parentScope != NULL);
#if 0
                 // DQ (7/4/2005): I think that this generates access to some IR nodes with template names not properly reset!
                    printf ("Calling parentScopeScope = parentScope->get_scope() \n");
                    SgScopeStatement* parentScopeScope = parentScope->get_scope();
                    printf ("DONE: Calling parentScopeScope = parentScope->get_scope() = %s \n",parentScopeScope->sage_class_name());
#endif
#if 0
                    printf ("     parentScope = %p = %s \n",parentScope,parentScope->sage_class_name());
#endif

#if 0
                 // DQ (6/29/2005): Make this a warning for now while we try to get back to compileing KULL
                    if (currentScope != parentScope)
                       {
                         printf ("Warning in ASTFixes: variable declaration = %p currentScope (%s) != parentScope (%s) (ignoring) \n",
                              variableDeclaration,currentScope->sage_class_name(),parentScope->sage_class_name());
                         parentScope->get_file_info()->display("variable's currentScope != parentScope");
                       }
#endif

                 // DQ (7/4/2005): fixup the scope to match that of the parent (which appears to be more accurate)!
                 // Note that the SgInitializedName objects will be set to the value of SgVariableDeclaration::get_scope().
                    if (currentScope != parentScope)
                       {
                      // Make sure that the parentScope is not the global scope (since there is only one global scope 
                      // and the current scope is global scope and the parent scope is different than the current 
                      // scope this should always be true).
                         ROSE_ASSERT ( isSgGlobal(parentScope) == NULL );

                      // This is meaningless since the scope is not stored explicitly at this IR node (nor need it be!)
                      // variableDeclaration->set_scope(parentScope);

                      // recompute the current scope (except that the scope of the Initialized name has not been set yet!)
                      // currentScope = variableDeclaration->get_scope();
                         currentScope = parentScope;
                         ROSE_ASSERT(currentScope == parentScope);
                       }
                    ROSE_ASSERT(currentScope == parentScope);
                  }
#endif

               break;
             }

          default:
             {
            // nothing to do here!
             }
        }

     SgDeclarationStatement* declarationStatement = isSgDeclarationStatement(node);
     if (declarationStatement != NULL)
        {
          if (declarationStatement->hasExplicitScope() == true)
             {
               if (declarationStatement->get_scope() == NULL)
                  {
                    printf ("InitializeExplicitScopes::visit() declarationStatement->get_scope() == NULL: declarationStatement = %p = %s = %s \n",
                         declarationStatement,declarationStatement->class_name().c_str(),SageInterface::get_name(declarationStatement).c_str());
                  }
               ROSE_ASSERT(declarationStatement->get_scope() != NULL);
             }
        }

#if 0
  // DQ (7/26/2007): Modified to use memory pool traversal
     vector <SgNode*> children = node->get_traversalSuccessorContainer(); 
     for (vector<SgNode*>::const_iterator p = children.begin(); p != children.end(); ++p)
        {
          SgNode *currentNode = *p;
          if ( currentNode != NULL)
             {
            // Why is this conditional here, what does it test
               if (node == currentNode->get_parent())
                    initializeExplicitScopeSupport(currentNode);
             }
        }
#endif
   }


