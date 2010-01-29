// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "fixupFriendTemplateDeclarations.h"
// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

void
fixupFriendTemplateDeclarations()
   {
  // DQ (3/10/2007): Introduce tracking of performance of ROSE.
     TimingPerformance timer ("Fixup friend template function declarations:");

     FixupFriendTemplateDeclarations t1;

  // Traverse just a specific memory pool (optimal).
  // t.traverseMemoryPool();
     SgTemplateDeclaration::traverseMemoryPoolNodes(t1);

     fixupFriendDeclarations();
   }

void
fixupFriendDeclarations()
   {
  // DQ (3/10/2007): Introduce tracking of performance of ROSE.
     TimingPerformance timer ("Fixup friend declarations:");

     FixupFriendDeclarations t2;

  // Traverse all the memory pools for now!
     t2.traverseMemoryPool();
   }

void
FixupFriendTemplateDeclarations::visit(SgNode* node)
   {
  // This function marks template declarations that are declared to be friends with the ROSE 
  // internal marking as friends.  Specifically it sets the firend flag in the declaration modifier
  // (the organization of modifiers follows the C++ grammar as laid out in the back of Bjarne's book).
  // EDG does not always record friend functions when they are templates (perhaps because we use the
  // template string implementation at present), however the friend keyward is in the string if it
  // is declared as such.  We determine if a declaration is a friend by checking the scope (stored 
  // explicitly) against the location of the declaration (using the parent information).

#if 0
     printf ("##### FixupFriendTemplateDeclarations::visit(node = %p = %s) \n",node,node->sage_class_name());
#endif

     SgTemplateDeclaration* templateDeclaration = isSgTemplateDeclaration(node);
     ROSE_ASSERT (templateDeclaration != NULL);
     if (templateDeclaration != NULL)
        {
       // DQ (10/21/2007): Need to make friend function declarations in classes as friend in the declarationModifier
       // However, the only clude that they are friend is that the scope is SgGlobal and parent is SgClassDefinition.
       // Thus when they are different we can safely mark the declaration as being a friend, I think!
          if (templateDeclaration->get_scope() != templateDeclaration->get_parent() )
             {
               if (templateDeclaration->get_parent() == NULL)
                  {
                    printf ("templateDeclaration = %p \n",templateDeclaration);
                  }

               ROSE_ASSERT(templateDeclaration->get_parent() != NULL);
               ROSE_ASSERT(templateDeclaration->get_scope() != NULL);

            // This should be a friend declaration, verify this.
            // ROSE_ASSERT(templateDeclaration->get_template_kind() == SgTemplateDeclaration::e_template_function);
#if 0
               printf ("FixupFriendTemplateDeclarations::visit(): templateDeclaration->get_template_kind() = %d \n",templateDeclaration->get_template_kind());
#endif
            // Make sure this is a template function (not a class or template member function)
               if (templateDeclaration->get_template_kind() == SgTemplateDeclaration::e_template_function)
                  {
                 // Only make the declarations that are defined in the class scope
                    if (isSgClassDefinition(templateDeclaration->get_parent()) != NULL)
                       {
#if 0
                         printf ("Marking a template function declaration as a friend, templateDeclaration->get_scope() = %p = %s and templateDeclaration->get_parent() = %p = %s \n",
                              templateDeclaration->get_scope(),templateDeclaration->get_scope()->class_name().c_str(),
                              templateDeclaration->get_parent(),templateDeclaration->get_parent()->class_name().c_str());
#endif
                      // Make sure the scope is not a class definition, should be global scope or a namespace scope.
                      // ROSE_ASSERT(isSgClassDefinition(templateDeclaration->get_scope()) == NULL);
                         if (isSgClassDefinition(templateDeclaration->get_scope()) != NULL)
                            {
                              printf ("templateDeclaration = %p templateDeclaration->get_scope() = %p = %s \n",templateDeclaration,templateDeclaration->get_scope(),templateDeclaration->get_scope()->class_name().c_str());
                              templateDeclaration->get_startOfConstruct()->display("templateDeclaration->get_scope() is a SgClassDefinition");
                            }
                         ROSE_ASSERT(isSgClassDefinition(templateDeclaration->get_scope()) == NULL);

                         templateDeclaration->get_declarationModifier().setFriend();
#if 0
                      // Also set the defining and non-defining declaration
                         if (templateDeclaration->get_definingDeclaration() != NULL && templateDeclaration->get_definingDeclaration()->get_declarationModifier().isFriend() == false)
                            {
                              templateDeclaration->get_definingDeclaration()->get_declarationModifier().setFriend();
                            }
                      // Also set the defining and non-defining declaration
                         if (templateDeclaration->get_firstNondefiningDeclaration() != NULL && templateDeclaration->get_get_firstNondefiningDeclaration()->get_declarationModifier().isFriend() == false)
                            {
                              templateDeclaration->get_get_firstNondefiningDeclaration()->get_declarationModifier().setFriend();
                            }
#endif
                       }
                  }
             }
        }
   }

void
FixupFriendDeclarations::visit(SgNode* node)
   {

#if 0
     printf ("##### FixupFriendDeclarations::visit(node = %p = %s) \n",node,node->sage_class_name());
#endif

     SgDeclarationStatement* declaration = isSgDeclarationStatement(node);
     if (declaration != NULL)
        {
       // DQ (10/21/2007): Need to make friend function declarations in classes as friend in the declarationModifier
       // However, the only clude that they are friend is that the scope is SgGlobal and parent is SgClassDefinition.
       // Thus when they are different we can safely mark the declaration as being a friend, I think!
#if 0
          if (declaration->get_parent() == NULL)
             {
               printf ("declaration = %p \n",declaration);
             }

       // Only make the declarations that are defined in the class scope
          ROSE_ASSERT(declaration->get_parent() != NULL);

          SgClassDefinition* classDefinition = isSgClassDefinition(declaration->get_parent());
          if (classDefinition != NULL)
             {
               ROSE_ASSERT(declaration->get_scope() != NULL);

               SgScopeStatement* declarationScope = declaration->get_scope();

#if 1
               printf ("FixupFriendDeclarations::visit(): declaration->get_declarationModifier().isFriend() = %d \n",declaration->get_declarationModifier().isFriend() ? "true" : "false");
               printf ("FixupFriendDeclarations::visit(): declaration->get_scope() = %p = %s \n",declarationScope,declarationScope->class_name().c_str());
               printf ("FixupFriendDeclarations::visit(): classDefinition = %p = %s \n",classDefinition,classDefinition->class_name().c_str());
#endif
               if (declarationScope != classDefinition )
                  {
                    printf ("##### Marking to be a friend for declaration = %p = %s \n",declaration,declaration->class_name().c_str(),SageInterface::get_name(declaration).c_str());

                    declaration->get_declarationModifier().setFriend();
                  }
             }
#endif
        }
   }

