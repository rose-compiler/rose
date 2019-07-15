// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "resetTemplateNames.h"
// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

#if 0
// DQ (2/19/2007): We now use the memory pool traversal in specific IR nodes instead of an AST traversal.
void
ResetTemplateNames::visitType ( SgType* typeNode )
   {
     ROSE_ASSERT(typeNode != NULL);

#if 0
     printf ("#### ResetTemplateNames::visitType(typeNode = %p = %s) \n",typeNode,typeNode->sage_class_name());
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

               ResetTemplateNames setTemplateNamesTraversal;
            // Should we be traversing the definingDeclaration, the type's classDeclaration or both!
               setTemplateNamesTraversal.traverse(classDeclaration,preorder);
               break;
             }

       // This is not a realistic case since no template class can be referenced inside of an enum (I think! Maybe pehaps an initializer?)
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

            // error checking
               if (enumDeclaration->get_parent() == NULL)
                  {
                    printf ("enumDeclaration being tested for valid parent pointer (failed) \n");
                    enumDeclaration->get_file_info()->display("enumDeclaration being tested for valid parent pointer");
                  }
               ROSE_ASSERT(enumDeclaration->get_parent() != NULL);

            // resetParentPointers (enumDeclaration,previousNode);
               ResetTemplateNames setTemplateNamesTraversal;
               setTemplateNamesTraversal.traverse(enumDeclaration,preorder);
               break;
             }

          default:
             {
            // Don't worry about any other nodes!
               break;
             }
        }
   }
#endif


#if 0
// DQ (2/19/2007): We now use the memory pool traversal in specific IR nodes instead of an AST traversal.
void
ResetTemplateNames::resetTemplateNamesInAllScopes ( SgScopeStatement* scope )
   {
     list<SgTemplateInstantiationDecl*> templateDeclarationList;

  // Traverse back through the scopes and save all references to template declaration
  // this is required since the name could depend on name qualifiers from any of the 
  // outer scopes so we have to reset the name in the reverse order (outer scopes first).
     while (scope->variantT() != V_SgGlobal)
        {
          SgTemplateInstantiationDefn* templateInstantiationDefinition  = isSgTemplateInstantiationDefn(scope);
          if (templateInstantiationDefinition != NULL)
             {
            // If we have found an instantiated template definition, then get the template instantiation declaration
               SgTemplateInstantiationDecl* templateInstantiationDeclaration = 
                    isSgTemplateInstantiationDecl(templateInstantiationDefinition->get_declaration());
               ROSE_ASSERT(templateInstantiationDeclaration != NULL);
               templateDeclarationList.push_front(templateInstantiationDeclaration);
            // templateInstantiationDeclaration->resetTemplateName();
             }

          scope = scope->get_scope();
       }

     while(templateDeclarationList.empty() == false)
        {
          SgTemplateInstantiationDecl* topOfStack = templateDeclarationList.front();
          ROSE_ASSERT(topOfStack != NULL);
#if 0
          printf ("#### ResetTemplateNames::visit(templateInstantiationDeclaration = %p = %s) \n",topOfStack,topOfStack->get_name().str());
          if (topOfStack->get_nameResetFromMangledForm() == false)
             {
               printf ("     Found a template declaration which has a name not previously reset (calling resetTemplateName) %p = %s \n",
                    topOfStack,topOfStack->get_name().str());
             }
#endif

       // Should this only be done when (topOfStack->get_nameResetFromMangledForm() == false)
          if (topOfStack->get_nameResetFromMangledForm() == false)
               topOfStack->resetTemplateName();
          templateDeclarationList.pop_front();
        }
   }
#endif

#if 0
// DQ (2/19/2007): We now use the memory pool traversal in specific IR nodes instead of an AST traversal.
void
ResetTemplateNames::visit ( SgNode* node )
   {
     ROSE_ASSERT(node);
  // cerr << "reset parent for node " << node->unparseToString();

#if 0
     printf ("#### ResetTemplateNames::visit(node = %p = %s) \n",node,node->sage_class_name());
#endif

  // DQ (11/26/2004): Traverse back through the scopes and force each 
  // SgTemplateInstantiationDecl to have it's name reset!
     SgStatement* statement = isSgStatement(node);
     if (statement != NULL)
        {
#if 0
          printf ("#### ResetTemplateNames::visit(statement = %p = %s) \n",statement,statement->sage_class_name());
#endif

          SgScopeStatement* scope = statement->get_scope();

       // Error checking!
          if (scope == NULL)
             {
               printf ("#### ResetTemplateNames::visit(): scope == NULL for statement = %p = %s \n",statement,statement->sage_class_name());
               statement->get_file_info()->display("Error: ResetTemplateNames::visit(): scope == NULL");

               SgClassDeclaration* classDeclaration = isSgClassDeclaration(statement);
               if (classDeclaration != NULL && classDeclaration->get_scope() == NULL)
                  {
                    printf ("***** Super Warning: SgClassDeclaration with explicit scope unset at %p = %s \n",
                         classDeclaration,classDeclaration->sage_class_name());
                    SgNode* parentNode = classDeclaration->get_parent();
                    ROSE_ASSERT(parentNode != NULL);
                    SgScopeStatement* parentScope = isSgScopeStatement(parentNode);
                    ROSE_ASSERT(parentScope != NULL);
                    printf ("     (Note fix done in WRONG place, just in time): Set scope to parent = %p = %s \n",
                         parentScope,parentScope->sage_class_name());
                    classDeclaration->set_scope(parentScope);

                 // Now reinitialize the scope!
                    scope = statement->get_scope();
                  }
             }
          ROSE_ASSERT(scope != NULL);

       // Refactored the code below into a separate member function!
          resetTemplateNamesInAllScopes(scope);
        }

     switch (node->variantT())
        {
          case V_SgTemplateInstantiationDecl:
             {
               SgTemplateInstantiationDecl* templateClassDeclaration = isSgTemplateInstantiationDecl(node);
               ROSE_ASSERT(templateClassDeclaration != NULL);

            // Calling resetTemplateName()
               templateClassDeclaration->resetTemplateName();
               break;
             }

       // DQ (2/16/2005): New support for function templates
       // This step interfers with templated function prototypes which must be of the form:
       //      template <char> void foo3();
       // and so can NOT include the template arguments in the function name.  So as an alternative
       // the unparser should output the function name and if not in a prototype, append the 
       // template arguments.  So it was commented out on the same day it was applied.
       // DQ (2/16/2005): I think this is corrent after all!!!
          case V_SgTemplateInstantiationFunctionDecl:
             {
               SgTemplateInstantiationFunctionDecl* templateFunctionDeclaration = isSgTemplateInstantiationFunctionDecl(node);
               ROSE_ASSERT(templateFunctionDeclaration != NULL);

            // DQ (2/16/2005): For now as a test, output the mangled name
            // string oldName     = templateFunctionDeclaration->get_name().str();
            // string mangledName = templateFunctionDeclaration->get_mangled_name().str();
            // printf ("function: oldName = %s mangledName = %s \n",oldName.c_str(),mangledName.c_str());
               templateFunctionDeclaration->resetTemplateName();
            // string newName     = templateFunctionDeclaration->get_name().str();
            // printf ("function: oldName = %s mangledName = %s newName = %s \n",oldName.c_str(),mangledName.c_str(),newName.c_str());
               break;
             }

       // DQ (10/20/2004): Not clear if this case will be a problem
          case V_SgTemplateInstantiationMemberFunctionDecl:
             {
               SgTemplateInstantiationMemberFunctionDecl* templateMemberFunctionDeclaration = isSgTemplateInstantiationMemberFunctionDecl(node);
               ROSE_ASSERT(templateMemberFunctionDeclaration != NULL);

            // It is better to set this properly than just have the constructor name exclude the template parameters 
            // because the constructor name is used for several purposes (e.g. case operator names) and here the 
            // template names arguments are required even if they are not required for the constructors function declaration.
               if ( templateMemberFunctionDeclaration->get_specialFunctionModifier().isConstructor() || 
                    templateMemberFunctionDeclaration->get_specialFunctionModifier().isDestructor()  ||
                    templateMemberFunctionDeclaration->get_specialFunctionModifier().isConversion() )
                  {
                 // printf ("Case V_SgTemplateInstantiationMemberFunctionDecl: found a constructor, destructor or conversion operator \n");
                 // templateMemberFunctionDeclaration->set_name(fixedUpName);
                    SgName className = templateMemberFunctionDeclaration->get_class_scope()->get_declaration()->get_name();
                    SgName fixedUpName = className;
                    if ( templateMemberFunctionDeclaration->get_specialFunctionModifier().isDestructor() )
                       {
                         fixedUpName = string("~") + className;
                      // printf ("In ASTFixs: Destructor case fixedUpName = %s \n",fixedUpName.str());
                      // templateMemberFunctionDeclaration->set_name(fixedUpName);
                       }
                      else
                       {
                         if (templateMemberFunctionDeclaration->get_specialFunctionModifier().isConstructor() == true)
                            {
#if 0
                              printf ("AST Fix: Resetting function name from %s to %s \n",templateMemberFunctionDeclaration->get_name().str(),fixedUpName.str());
#endif
                              templateMemberFunctionDeclaration->set_name(fixedUpName);
                            }
                           else
                            {
                           // DQ (11/24/2004): Separate out the case of a conversion operator (don't reset the 
                           // name from the form "operator X&(result)"). See test2004_141.C.
                              ROSE_ASSERT (templateMemberFunctionDeclaration->get_specialFunctionModifier().isConversion() == true);
#if 0
                              printf ("AST Fix: Might have to reset target of conversion (more complex) %s \n",
                                   templateMemberFunctionDeclaration->get_name().str());
#endif
                           // templateMemberFunctionDeclaration->set_name(fixedUpName);
                            }
                       }
                  }
                 else
                  {
                 // This was implemented as a test when there were problems finding the use of the STL 
                 // string class and changing all the names to include the template arguments explicitly.
                    ROSE_ASSERT(templateMemberFunctionDeclaration->get_name() != "basic_string");

                 // This is the case of a member function, if it is a template member function then we 
                 // should modify the name to include the template parameters.

                 // DQ (2/16/2005): For now as a test, output the mangled name
                 // string mangledName = templateMemberFunctionDeclaration->get_mangled_name().str();
                 // printf ("member function: mangledName = %s \n",mangledName.c_str());
                  }

            // printf ("ResetTemplateNames::visit(): case V_SgTemplateInstantiationMemberFunctionDecl: not implemented! \n");
            // ROSE_ASSERT(false);

            // Should we be calling resetTemplateName()
            // templateClassDeclaration->resetTemplateName();
            // DQ (2/17/2005): For now as a test, output the mangled name
               string oldName     = templateMemberFunctionDeclaration->get_name().str();
               string mangledName = templateMemberFunctionDeclaration->get_mangled_name().str();
            // printf ("member function: oldName = %s mangledName = %s \n",oldName.c_str(),mangledName.c_str());
               templateMemberFunctionDeclaration->resetTemplateName();
               string newName     = templateMemberFunctionDeclaration->get_name().str();
            // printf ("member function: oldName = %s mangledName = %s newName = %s \n",oldName.c_str(),mangledName.c_str(),newName.c_str());
               break;
             }

       // DQ (12/9/2004): Handle the template instantiation names that are found in the preinitialization lists
          case V_SgCtorInitializerList:
             {
               SgCtorInitializerList* preinitializationList = isSgCtorInitializerList(node);
               ROSE_ASSERT(preinitializationList != NULL);

               SgInitializedNamePtrList & list = preinitializationList->get_ctors();

               for (SgInitializedNamePtrList::iterator i = list.begin(); i != list.end(); i++)
                  {
                 // Note that the declaration found in the initalized name is to the member function 
                 // owning the preinitialization list, not clear that this is correct!
                 // printf ("##### i = %s i->get_name() = %s i->get_type() = %p = %s \n",
                 //      (*i)->sage_class_name(),(*i)->get_name().str(),(*i)->get_type(),(*i)->get_type()->sage_class_name());

                    SgType* initializerType = (*i)->get_type();
                    ROSE_ASSERT(initializerType != NULL);
                 // printf ("initializerType = %p = %s \n",initializerType,initializerType->sage_class_name());
                    visitType(initializerType);

                 // DQ (12/11/2004): I have placed information into the SgInitializedName IR nodes to classify them 
                 // when they are used within preinitialization lists.
                    if ( (*i)->get_preinitialization() == SgInitializedName::e_virtual_base_class ||
                         (*i)->get_preinitialization() == SgInitializedName::e_nonvirtual_base_class )
                       {
                         SgClassType* classType = isSgClassType(initializerType);
                         ROSE_ASSERT(classType != NULL);
                      // printf ("Reset the name in the SgInitializedName object (base class): (*i)->get_name() = %s classType->get_name() = %s \n",
                      //      (*i)->get_name().str(),classType->get_name().str());
                         (*i)->set_name(classType->get_name());
                      // printf ("After reset name in SgInitializedName: i->get_name() = %s i->get_type() = %p = %s \n",
                      //      (*i)->get_name().str(),(*i)->get_type(),(*i)->get_type()->sage_class_name());
                       }
                      else
                       {
                      // printf ("Skipping reset for this non base class entry in preinitialization list (*i)->get_name() = %s \n",(*i)->get_name().str());
                       }
                  }
               break;
             }

          case V_SgTypedefDeclaration:
             {
            // printf ("Found a SgTypedefDeclaration = %p looking for islands of untraversed AST ... \n",node);
               SgTypedefDeclaration* typedefDeclaration = isSgTypedefDeclaration(node);
               ROSE_ASSERT(typedefDeclaration != NULL);

            // typedefDeclaration->get_file_info()->display("case V_SgTypedefDeclaration: typedefDeclaration");

            // We only want to traverse the base type (since all "*" and "&" are associated
            // with the variables in this variable list, e.g. list of SgInitializedName objects)
               SgType* baseType = typedefDeclaration->get_base_type();
               ROSE_ASSERT(baseType != NULL);

               visitType (baseType);

            // DQ (10/14/2004): Even if this is not an island it might be a typedef of a template which 
            // needs its name to be reset.  If it is a typedef that is later used then it would be reset
            // where the type of the variable declaration would be seen, but if it is not used then we will 
            // only see it here and thus we have to find and reset the name of the template declaration.
               ROSE_ASSERT(baseType != NULL);
            // printf ("baseType = %p = %s \n",baseType,baseType->sage_class_name());
               SgNamedType* namedType = isSgNamedType(baseType);

            // DQ (10/16/2004): Need to set this since typedef types are used in cases where they
            // must be provided with qualified names and so there parents must be setup properly!
               if (namedType != NULL)
                  {
                    SgClassType* classType = isSgClassType(namedType);
                    if (classType != NULL)
                       {
                         SgDeclarationStatement* declaration = classType->get_declaration();
                         ROSE_ASSERT(declaration != NULL);
                         SgTemplateInstantiationDecl* templateInstantiation = isSgTemplateInstantiationDecl(declaration);
                         if (templateInstantiation != NULL)
                            {
                          // Calling resetTemplateName()
                              templateInstantiation->resetTemplateName();
                            }
                       }
                  }

            // DQ (4/1/2005): Find any code (such as a SgTemplateInstantiationDecl) that is buried within a SgPointerMemberType.
            // Examples of such code is: "typedef void (MeshConcept<int>::* funcMeshMeshConcept)();".
            // See test2005_41.C for an example.
               SgPointerMemberType* pointerMemberType = isSgPointerMemberType (baseType);
               if (pointerMemberType != NULL)
                  {
                    SgClassDefinition* classDefinition = pointerMemberType->get_class_of();

                 // It is a little less trivial to obtain the SgTemplateInstatiationDecl from the SgTemplateInstatiationDefn
                    SgClassDeclaration* classDeclaration = classDefinition->get_declaration();
                    ROSE_ASSERT(classDeclaration != NULL);
                    SgTemplateInstantiationDecl* templateInstantiation = isSgTemplateInstantiationDecl(classDeclaration);
                    if (templateInstantiation != NULL)
                       {
                      // printf ("Resetting template name hidden within a SgPointerMemberType \n");
                         templateInstantiation->resetTemplateName();
                       }
                  }

               break;
             }

          case V_SgVariableDeclaration:
             {
            // printf ("Found a SgVariableDeclaration = %p looking for islands of untraversed AST ... \n",node);
               SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(node);
               ROSE_ASSERT(variableDeclaration != NULL);

            // variableDeclaration->get_file_info()->display("case V_SgVariableDeclaration: variableDeclaration");

            // We only want to traverse the base type (since all "*" and "&" are associated
            // with the variables in this variable list, e.g. list of SgInitializedName objects)
               SgInitializedNamePtrList & variableList = variableDeclaration->get_variables();
               ROSE_ASSERT(variableList.size() > 0);
               SgInitializedName* firstVariable = *(variableList.begin());
               ROSE_ASSERT(firstVariable != NULL);
               SgType* baseType = firstVariable->get_typeptr();
               ROSE_ASSERT(baseType != NULL);

               visitType (baseType);
               break;
             }

          case V_SgInitializedName:
             {
            // Find the types within the function declaration and set the parents of any 
            // declarations that are contained in the types.
               SgInitializedName* initializedName = isSgInitializedName(node);
               ROSE_ASSERT(initializedName != NULL);
               SgType* type = initializedName->get_type();
               if (type != NULL)
                  {
                    type = type->findBaseType();
                    SgClassType* classType = isSgClassType(type);
                    if (classType != NULL)
                       {
                         SgDeclarationStatement* declaration = classType->get_declaration();
                         ROSE_ASSERT(declaration != NULL);
                         SgClassDeclaration* classDeclaration = isSgClassDeclaration(declaration);
                         ROSE_ASSERT(classDeclaration != NULL);
                         SgTemplateInstantiationDecl* templateInstantiation = isSgTemplateInstantiationDecl(declaration);
                         if (templateInstantiation != NULL)
                            {
                           // Calling resetTemplateName()
                              templateInstantiation->resetTemplateName();
                            }
                       }
                  }
               break;
             }

       // DQ (11/27/2004): These can force the generation of qualified names so make sure that any instantiated 
       // template classes along the path of scopes to the global scope have there name reset!
          case V_SgFunctionRefExp:
             {
            // loop back through scopes to reset template names (in case function is used with qualified name)
               SgFunctionRefExp* functionRefExp = isSgFunctionRefExp(node);
               ROSE_ASSERT(functionRefExp != NULL);
               SgFunctionDeclaration* functionDeclaration = functionRefExp->get_symbol()->get_declaration();
               ROSE_ASSERT(functionDeclaration != NULL);

               SgScopeStatement* scope = functionDeclaration->get_scope();
               ROSE_ASSERT(scope != NULL);
               resetTemplateNamesInAllScopes(scope);
               break;
             }

       // DQ (11/27/2004): These can force the generation of qualified names so make sure that any instantiated 
       // template classes along the path of scopes to the global scope have there name reset!
          case V_SgMemberFunctionRefExp:
             {
            // loop back through scopes to reset template names (in case member function is used with qualified name)
               SgMemberFunctionRefExp* memberFunctionRefExp = isSgMemberFunctionRefExp(node);
               ROSE_ASSERT(memberFunctionRefExp != NULL);
               SgMemberFunctionDeclaration* memberFunctionDeclaration = memberFunctionRefExp->get_symbol()->get_declaration();
               ROSE_ASSERT(memberFunctionDeclaration != NULL);

               SgScopeStatement* scope = memberFunctionDeclaration->get_scope();
               ROSE_ASSERT(scope != NULL);
               resetTemplateNamesInAllScopes(scope);
               break;
             }

       // DQ (6/21/2005): Take care of names hidden in base class specifications!
          case V_SgClassDefinition:
          case V_SgTemplateInstantiationDefn:
             {
               SgClassDefinition* classDefinition = isSgClassDefinition(node);
               ROSE_ASSERT(classDefinition != NULL);

               SgBaseClassPtrList::iterator i = classDefinition->get_inheritances().begin();
               while ( i != classDefinition->get_inheritances().end() )
                  {
                 // Check the parent pointer to make sure it is properly set
                    ROSE_ASSERT( (*i)->get_parent() != NULL);
                    ROSE_ASSERT( (*i)->get_parent() == classDefinition);

                 // Calling resetTemplateName()
                    SgClassDeclaration* baseClassDeclaration = (*i)->get_base_class();
                    ROSE_ASSERT(baseClassDeclaration != NULL);
                    SgTemplateInstantiationDecl* templateInstantiation = isSgTemplateInstantiationDecl(baseClassDeclaration);
                    if (templateInstantiation != NULL)
                       {
                         templateInstantiation->resetTemplateName();
                       }

                    i++;
                  }
               break;
             }

          case V_SgTemplateInstantiationDirectiveStatement:
             {
               SgTemplateInstantiationDirectiveStatement* directive = isSgTemplateInstantiationDirectiveStatement(node);
               ROSE_ASSERT(directive != NULL);
#if 0
               printf ("Found a SgTemplateInstantiationDirectiveStatement to reset  directive->get_declaration() = %p = %s \n",
                    directive->get_declaration(),directive->get_declaration()->class_name().c_str());
#endif

            // DQ (8/31/2005): Better implementation
               ResetTemplateNames setTemplateNamesTraversal;
               setTemplateNamesTraversal.traverse(directive->get_declaration(),preorder);
               break;
             }

       // DQ (8/31/2005): I expect that we do need this case!
          case V_SgUsingDeclarationStatement:
             {
            // printf ("In ResetTemplateNames::visit(): Could a template instantiation be hidden in a SgUsingDeclarationStatement? \n");

               SgUsingDeclarationStatement* usingDeclaration = isSgUsingDeclarationStatement(node);
               ROSE_ASSERT(usingDeclaration != NULL);

               ResetTemplateNames setTemplateNamesTraversal;
               setTemplateNamesTraversal.traverse(usingDeclaration->get_declaration(),preorder);
               break;
             }

       // DQ (8/31/2005): I don't think we need this case!
          case V_SgUsingDirectiveStatement:
             {
            // printf ("In ResetTemplateNames::visit(): Could a template instantiation be hidden in a SgUsingDirectiveStatement? \n");
               break;
             }

          default:
             {
            // Don't worry about any other nodes!
               break;
             }
        }
   }
#endif

// DQ (2/19/2007): This is a simplified handling of template names using the memory pool traversal on
// specific IR nodes (SgTemplateInstantiationDecl,SgTemplateInstantiationFunctionDecl,and 
// SgTemplateInstantiationMemberFunctionDecl) so that we can be certain that we visit ALL relavant IR nodes
// (the AST traversal forced us to hunt for them, as in the resetParent() traversal).
void
ResetTemplateNamesOnMemoryPool::visit ( SgNode* node )
   {
     ROSE_ASSERT(node);
  // cerr << "reset parent for node " << node->unparseToString();

#if 0
  // DQ (6/30/2018): Added to support debuggin test2018_118.C (infinit loop).
  // Moved declaration here to avoid unused variable warning [Rasmussen 2019.01.29]
     static int counter = 0;

  // printf ("#### ResetTemplateNamesOnMemoryPool::visit(node = %p = %s) \n",node,node->sage_class_name());
     printf ("#### ResetTemplateNamesOnMemoryPool::visit(node = %p = %s): counter = %d \n",node,node->sage_class_name(),counter++);
#endif
     
     switch (node->variantT())
        {
          case V_SgTemplateInstantiationDecl:
             {
               SgTemplateInstantiationDecl* templateClassDeclaration = isSgTemplateInstantiationDecl(node);
               ROSE_ASSERT(templateClassDeclaration != NULL);
#if 0
               string oldName     = templateClassDeclaration->get_name().str();
               string mangledName = templateClassDeclaration->get_mangled_name().str();
               printf ("template class: oldName = %s mangledName = %s \n",oldName.c_str(),mangledName.c_str());
#endif
            // Calling resetTemplateName()
               templateClassDeclaration->resetTemplateName();
#if 0
               string newName     = templateClassDeclaration->get_name().str();
               printf ("template class: oldName = %s mangledName = %s newName = %s \n",oldName.c_str(),mangledName.c_str(),newName.c_str());
#endif
               break;
             }

          case V_SgTemplateInstantiationFunctionDecl:
             {
               SgTemplateInstantiationFunctionDecl* templateFunctionDeclaration = isSgTemplateInstantiationFunctionDecl(node);
               ROSE_ASSERT(templateFunctionDeclaration != NULL);

            // DQ (2/16/2005): For now as a test, output the mangled name
            // string oldName     = templateFunctionDeclaration->get_name().str();
            // string mangledName = templateFunctionDeclaration->get_mangled_name().str();
            // printf ("template function: oldName = %s mangledName = %s \n",oldName.c_str(),mangledName.c_str());
               templateFunctionDeclaration->resetTemplateName();
            // string newName     = templateFunctionDeclaration->get_name().str();
            // printf ("template function: oldName = %s mangledName = %s newName = %s \n",oldName.c_str(),mangledName.c_str(),newName.c_str());
               break;
             }

       // DQ (10/20/2004): Not clear if this case will be a problem
          case V_SgTemplateInstantiationMemberFunctionDecl:
             {
               SgTemplateInstantiationMemberFunctionDecl* templateMemberFunctionDeclaration = isSgTemplateInstantiationMemberFunctionDecl(node);
               ROSE_ASSERT(templateMemberFunctionDeclaration != NULL);

            // It is better to set this properly than just have the constructor name exclude the template parameters 
            // because the constructor name is used for several purposes (e.g. case operator names) and here the 
            // template names arguments are required even if they are not required for the constructors function declaration.
               if ( templateMemberFunctionDeclaration->get_specialFunctionModifier().isConstructor() || 
                    templateMemberFunctionDeclaration->get_specialFunctionModifier().isDestructor()  ||
                    templateMemberFunctionDeclaration->get_specialFunctionModifier().isConversion() )
                  {
                 // printf ("Case V_SgTemplateInstantiationMemberFunctionDecl: found a constructor, destructor or conversion operator \n");
                 // templateMemberFunctionDeclaration->set_name(fixedUpName);

                 // DQ (7/26/2007): Modified to handle case where class scope is not avialble (see test2007_116.C).
                 // ROSE_ASSERT(templateMemberFunctionDeclaration->get_class_scope() != NULL);

                 // DQ (7/28/2007): Modified to use get_scope(), it might still be too eary to call get_class_scope().
                 // printf ("Modified to use get_scope(), it might still be too early to call get_class_scope() \n");
                 // if (templateMemberFunctionDeclaration->get_class_scope() != NULL)
                    if (templateMemberFunctionDeclaration->get_scope() != NULL)
                       {
                      // ROSE_ASSERT(templateMemberFunctionDeclaration->get_class_scope()->get_declaration() != NULL);
                      // SgName className = templateMemberFunctionDeclaration->get_class_scope()->get_declaration()->get_name();
                      // ROSE_ASSERT(templateMemberFunctionDeclaration->get_scope()->get_declaration() != NULL);
                      // SgName className = templateMemberFunctionDeclaration->get_scope()->get_declaration()->get_name();
                         SgScopeStatement* classScope = templateMemberFunctionDeclaration->get_scope();
                         SgClassDefinition* classDefinition = isSgClassDefinition(classScope);
                         ROSE_ASSERT(classDefinition != NULL);
                         SgClassDeclaration* classDeclaration = classDefinition->get_declaration();
                         ROSE_ASSERT(classDeclaration != NULL);
                      // ROSE_ASSERT(templateMemberFunctionDeclaration->get_scope()->get_declaration() != NULL);
                      // SgName className = templateMemberFunctionDeclaration->get_scope()->get_declaration()->get_name();
                         SgName className = classDeclaration->get_name();
                         SgName fixedUpName = className;
                      // printf ("fixedUpName = %s \n",fixedUpName.str());
                         if ( templateMemberFunctionDeclaration->get_specialFunctionModifier().isDestructor() )
                            {
                           // fixedUpName = string("~") + className;

                           // DQ (2/10/2007): I am unclear why we don't reset the name here (commented out)!
                           // Likely it is done in the code generation phase.
                           // printf ("In ASTFixs: Destructor case fixedUpName = %s (original name = %s) (why don't we call set_name for the SgTemplateInstantiationMemberFunctionDecl?) \n",fixedUpName.str(),templateMemberFunctionDeclaration->get_name().str());
                           // templateMemberFunctionDeclaration->set_name(fixedUpName);
                           // templateMemberFunctionDeclaration->set_name(fixedUpName);
                            }
                           else
                            {
                              if (templateMemberFunctionDeclaration->get_specialFunctionModifier().isConstructor() == true)
                                 {
#if 0
                                   printf ("AST Fix: Resetting function name for constructor from %s to %s \n",templateMemberFunctionDeclaration->get_name().str(),fixedUpName.str());
#endif
                                // DQ (2/19/2007): The symbol table will be handled by the resetTemplateName() called below!
                                // printf ("WARNING: Need to unload symbol for %s and load symbol for %s \n",className.str(),fixedUpName.str());

                                // I think that this should be commented out since the name will be updated directly
                                // templateMemberFunctionDeclaration->set_name(fixedUpName);

                                // ROSE_ASSERT(false);
                                 }
                                else
                                 {
                                // DQ (11/24/2004): Separate out the case of a conversion operator (don't reset the 
                                // name from the form "operator X&(result)"). See test2004_141.C.
                                   ROSE_ASSERT (templateMemberFunctionDeclaration->get_specialFunctionModifier().isConversion() == true);
#if 0
                                   printf ("AST Fix: Might have to reset target of conversion (more complex) %s \n",
                                        templateMemberFunctionDeclaration->get_name().str());
#endif
                                // templateMemberFunctionDeclaration->set_name(fixedUpName);
                                 }
                            }
                       }
                      else
                       {
                         printf ("Case where templateMemberFunctionDeclaration->get_class_scope() == NULL \n");
                         ROSE_ASSERT(false);
                       }
                  }
                 else
                  {
                 // This was implemented as a test when there were problems finding the use of the STL 
                 // string class and changing all the names to include the template arguments explicitly.
                 // ROSE_ASSERT(templateMemberFunctionDeclaration->get_name() != "basic_string");

                 // This is the case of a member function, if it is a template member function then we 
                 // should modify the name to include the template parameters.

                 // DQ (2/16/2005): For now as a test, output the mangled name
                 // string mangledName = templateMemberFunctionDeclaration->get_mangled_name().str();
                 // printf ("member function: mangledName = %s \n",mangledName.c_str());
                  }

            // printf ("ResetTemplateNames::visit(): case V_SgTemplateInstantiationMemberFunctionDecl: not implemented! \n");
            // ROSE_ASSERT(false);

            // Should we be calling resetTemplateName()
            // templateClassDeclaration->resetTemplateName();
            // DQ (2/17/2005): For now as a test, output the mangled name
#if 0
               string oldName     = templateMemberFunctionDeclaration->get_name().str();
               string mangledName = templateMemberFunctionDeclaration->get_mangled_name().str();
               printf ("member function: oldName = %s mangledName = %s \n",oldName.c_str(),mangledName.c_str());
#endif

            // DQ (2/19/2007): This function will correctly unload/reload the symbol table.
               templateMemberFunctionDeclaration->resetTemplateName();
#if 0
               string newName     = templateMemberFunctionDeclaration->get_name().str();
               printf ("member function: oldName = %s mangledName = %s newName = %s \n",oldName.c_str(),mangledName.c_str(),newName.c_str());
#endif
               break;
             }

          default:
             {
            // Don't worry about any other nodes (since we use the memory pool we know that we get all the template instantiations)!
               break;
             }
        }
   }

void resetTemplateNames( SgNode* node )
   {
  // DQ (7/7/2005): Introduce tracking of performance of ROSE.
     TimingPerformance resetTemplateNameTimer ("Reset template names:");

  // DQ (7/29/2005): Added support with Qing for AST fragments that occure in the ASTInterface classes.
  // I gather that once the parent pointer is set then that indicates that the class declaration is 
  // part of a valid AST?
  // setTemplateNamesTraversal.traverse(node,preorder);
  // GB (8/19/2009): Allowing SgProject nodes here, under the assumption
  // that a project node also represents a valid AST.
     if (node->get_parent() != NULL || isSgProject(node))
        {
       // DQ (2/10/2007): Use the memory pool traversal so that we can get all relavant IR nodes (missed to many previously, which made the AST merge a problem)
       // ResetTemplateNames setTemplateNamesTraversal;
       // setTemplateNamesTraversal.traverse(node,preorder);

#if 0
       // This will traverse the whole memory pool
          ResetTemplateNamesOnMemoryPool setTemplateNamesTraversal;
          setTemplateNamesTraversal.traverseMemoryPool();
#else
       // DQ (2/19/2007): We can alternatively just traverse the types of IR nodes that are relavant, 
       // instead of the whole memory pool.  This is a cool feature of the memory pool traversal
       // in this case this make the traversal about 30% faster, since we visit such a small subset 
       // of IR nodes.
          ResetTemplateNamesOnMemoryPool t;
#if 0
          printf ("Calling memory pool traversal on SgTemplateInstantiationDecl \n");
#endif
          SgTemplateInstantiationDecl::traverseMemoryPoolNodes(t);
#if 0
          printf ("Calling memory pool traversal on SgTemplateInstantiationFunctionDecl \n");
#endif
          SgTemplateInstantiationFunctionDecl::traverseMemoryPoolNodes(t);
#if 0
          printf ("Calling memory pool traversal on SgTemplateInstantiationMemberFunctionDecl \n");
#endif
          SgTemplateInstantiationMemberFunctionDecl::traverseMemoryPoolNodes(t);
#if 0
          printf ("DONE: Calling memory pool traversal on template instantiations \n");
#endif
#endif
        }
       else
        {
       // DQ (2/10/2007): This should hopefully not happen on too many IR nodes of a properly built AST!
          printf ("Detected AST fragement not associated with primary AST, ignore template handling ... (premature point to call resetTemplateNames() function) \n");
        }
   }

