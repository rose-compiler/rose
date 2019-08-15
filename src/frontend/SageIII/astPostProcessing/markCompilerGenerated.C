// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"

#include "rose_config.h"

#include "markCompilerGenerated.h"

void
markAsCompilerGenerated( SgNode* node )
   {
  // This simplifies how the traversal is called!
     MarkAsCompilerGenerated astFixupTraversal;

  // printf ("In markAsCompilerGenerated(): What sort of IR node is being resursively marked as compiler generated? node = %p = %s \n",node,node->class_name().c_str());

  // I think the default should be preorder so that the interfaces would be more uniform
     astFixupTraversal.traverse(node,preorder);
   }

// DQ (12/23/2011): Template declarations are now derived from there associated non-template 
// declarations (e.g. SgTemplateClassDeclaration is derived from SgClassDeclaration). It was
// previously the case that SgTemplateClassDeclaration was derived from SgTemplateDeclaration
// in the first attempt to put the template declarations into the AST.  Previous to this (within 
// the EDG 3.3 support within ROSE there was only a SgTemplateDeclaration and there was no 
// SgTemplateClassDeclaration.  The same story was the case for functions, member functions, 
// and variable template declrations.
bool
MarkAsCompilerGenerated::templateDeclarationCanBeMarkedAsCompilerGenerated(SgDeclarationStatement* templateDeclaration)
   {
  // Note that this function uses and requires parent pointers to be previously set.

     bool markAsCompilerGenerated = true;

     ROSE_ASSERT(templateDeclaration != NULL);

  // We should not be marking template declarations as compiler generated (I think)
  // if (isSgTemplateDeclaration(node) != NULL)
  // SgTemplateDeclaration* templateDeclaration = isSgTemplateDeclaration(node);
     if (templateDeclaration != NULL)
        {
#if 0
          printf ("In MarkAsCompilerGenerated::templateDeclarationCanBeMarkedAsCompilerGenerated():\n");
          printf ("  --- templateDeclaration = %p (%s)\n", templateDeclaration, templateDeclaration ? templateDeclaration->class_name().c_str() : "");
          if (isSgTemplateClassDeclaration(templateDeclaration)) {
            printf ("  --- templateDeclaration->get_specialization() = %d\n", isSgTemplateClassDeclaration(templateDeclaration)->get_specialization());
          }
#endif
       // Get the structural representation from the parent since in the case of a 
       // global function marked as friend we want to ignore the scope.

          SgScopeStatement* parentScope = isSgScopeStatement(templateDeclaration->get_parent());
#if 0
          printf ("  --- parentScope = %p (%s)\n", parentScope, parentScope ? parentScope->class_name().c_str() : "");
#endif
          SgGlobal* globalScope = isSgGlobal(parentScope);
          SgNamespaceDefinitionStatement* namespaceScope = isSgNamespaceDefinitionStatement(parentScope);
          if (globalScope != NULL || namespaceScope != NULL)
             {
            // This is an error case
               bool isFriend = templateDeclaration->get_declarationModifier().isFriend();
               if (isFriend == true)
                  {
                 // This is OK case too!
                  }
                 else
                  {
                 // printf ("Warning: detected attempt to mark a template declaration in scope = %s as compiler generated! (will not be marked)\n",parentScope->class_name().c_str());
                 // templateDeclaration->get_file_info()->display("template declaration being marked as compiler generated");
                 // ROSE_ASSERT(false);

                    markAsCompilerGenerated = false;
                  }
             }
            else
             {
            // This is an OK case
#if 0
               printf ("Note: Found a template declaration in scope = %s being marked as compiler generated! (ok case) \n",parentScope->class_name().c_str());
               templateDeclaration->get_file_info()->display("template declaration being marked as compiler generated");
#endif
             }
        }
  // ROSE_ASSERT(isSgTemplateDeclaration(node) == NULL);

     return markAsCompilerGenerated;
   }

void
MarkAsCompilerGenerated::visit(SgNode* node)
   {
     ROSE_ASSERT(node != NULL);

  // Only make statements (skip expressions since compiler generated casts are not output!)
  // We should not be borrowing the compiler generated flag to mark IR statement nodes for output by the unparser!
     SgStatement* statement = isSgStatement(node);
     if (statement != NULL)
        {
       // printf ("Attempting to mark %s as compiler generated \n",node->class_name().c_str());

          bool couldBeCompilerGenerated = true;
          SgTemplateDeclaration* templateDeclaration = isSgTemplateDeclaration(node);
          if (templateDeclaration != NULL)
             {
            // DQ (8/12/2005): There are non-trivial cases where a template declaration can be compiler generated (e.g. when it is a nested class)
               couldBeCompilerGenerated = MarkAsCompilerGenerated::templateDeclarationCanBeMarkedAsCompilerGenerated(templateDeclaration);
               if (couldBeCompilerGenerated == false)
                  {
                    printf ("Warning: detected attempt to mark a template declaration in global or namespace scope as compiler generated! (will not be marked)\n");
                    templateDeclaration->get_file_info()->display("template declaration nearly marked as compiler generated (prevented)");
                  }
             }

       // DQ (8/17/2005): Mark any compiler generated member function instatiations as non-specialied.
       // EDG marks functions as specialization when they are just defined outside of their parent scope.
          SgTemplateInstantiationMemberFunctionDecl* memberFunctionTemplateInstantiation = isSgTemplateInstantiationMemberFunctionDecl(node);
          if (memberFunctionTemplateInstantiation != NULL)
             {
            // printf ("Found a memberFunctionTemplateInstantiation \n");
               if (memberFunctionTemplateInstantiation->isSpecialization() == true)
                  {
                 // printf ("In AST Fixup: resetting memberFunctionTemplateInstantiation to be marked as non-specialized \n");
                    memberFunctionTemplateInstantiation->set_specialization (SgDeclarationStatement::e_no_specialization);
                  }
             }

          Sg_File_Info* fileInfo = node->get_file_info();
          if (fileInfo != NULL)
             {
#if 0
               printf ("MarkAsCompilerGenerated::visit(): Marking node = %p = %s as compiler generated \n",node,node->class_name().c_str());
#endif
            // Mark the file info object as being compiler generated instead of part of a transformation. If
            // it were part of a transformation the the unparser would be forced to output the associated code.
            // if (markAsCompilerGenerated == true)
               if (couldBeCompilerGenerated == true)
                  {
                 // DQ (12/21/2006): Modified to make the settings uniform over all 
                 // possible source position (there are two for statements).
                 // fileInfo->setCompilerGenerated();
                    statement->setCompilerGenerated();
                  }
             }
        }
       else 
        {
       // DQ (3/31/2006): SgInitializedName IR nodes need to be marked (and they are not SgStatements)
          SgInitializedName* initializedName = isSgInitializedName(node);
          if (initializedName != NULL)
             {
               Sg_File_Info* fileInfo = node->get_file_info();
               if (fileInfo != NULL)
                  {
                 // Mark the file info object as being compiler generated instead of part of a transformation. If
                 // it were part of a transformation the the unparser would be forced to output the associated code.
                    fileInfo->setCompilerGenerated();
                  }
             }
        }
   }

