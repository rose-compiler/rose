// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "AstFixup.h"
#include "fixupStorageAccessOfForwardTemplateDeclarations.h"
// tps : Added this as it is defined somewhere in rose.h
#include "AstDiagnostics.h"

void
fixupStorageAccessOfForwardTemplateDeclarations( SgNode* node )
   {
  // DQ (7/7/2005): Introduce tracking of performance of ROSE.
     TimingPerformance timer ("Fixup storage access of forward template declarations (EDG bug):");

  // This simplifies how the traversal is called!
     FixupStorageAccessOfForwardTemplateDeclarations declarationFixupTraversal;

  // I think the default should be preorder so that the interfaces would be more uniform
     declarationFixupTraversal.traverse(node,preorder);
   }

void
FixupStorageAccessOfForwardTemplateDeclarations::visit (SgNode* node)
   {
     ROSE_ASSERT(node != NULL);

  // These are output a static functions to avoid being defined using multiple symbols at link time.
  // This is a temporary fix until we get the better prelinker written.
     SgTemplateInstantiationFunctionDecl* functionDeclaration = isSgTemplateInstantiationFunctionDecl(node);
     if (functionDeclaration != NULL)
        {
          SgDeclarationStatement* definingDeclaration = functionDeclaration->get_definingDeclaration();
          if (definingDeclaration != NULL)
             {
               bool isStatic = definingDeclaration->get_declarationModifier().get_storageModifier().isStatic();
               if (isStatic == true)
                  {
                 // This is a static function so make sure that all declarations associated with it 
                 // (e.g. forward declarations) are marked the same.
                    bool currentDeclarationIsStatic = functionDeclaration->get_declarationModifier().get_storageModifier().isStatic();
                    if (currentDeclarationIsStatic == false)
                       {
                         if ( SgProject::get_verbose() >= AST_FIXES_VERBOSE_LEVEL )
                              printf ("AST fixup: Changing non-defining declaration (%p) to match defining declaration (static)\n",functionDeclaration);
                         functionDeclaration->get_declarationModifier().get_storageModifier().setStatic();
                       }
                  }
             }
        }
   }

