// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "fixupTemplateDeclarations.h"
#include "AstFixup.h"

// DQ (8/10/2005): Test and fixup any template declaration that is mistakenly marked as compiler-generated.
void fixupTemplateDeclarations( SgNode* node )
   {
  // DQ (7/7/2005): Introduce tracking of performance of ROSE.
     TimingPerformance timer ("Fixup template declarations:");
  // This simplifies how the traversal is called!
     FixupTemplateDeclarations astFixupTraversal;

  // I think the default should be preorder so that the interfaces would be more uniform
     astFixupTraversal.traverse(node,preorder);
   }

void
FixupTemplateDeclarations::visit ( SgNode* node )
   {
  // DQ (8/10/2005): Unmarked any template declaration marked as compiler generated!

  // printf ("node = %s \n",node->sage_class_name());

     switch (node->variantT())
        {
          case V_SgTemplateDeclaration:
             {
               SgTemplateDeclaration* templateDeclaration = isSgTemplateDeclaration(node);
               ROSE_ASSERT (templateDeclaration != NULL);
               if (templateDeclaration->get_file_info()->isCompilerGenerated() == true)
                  {
#if PRINT_DEVELOPER_WARNINGS
                    printf ("AST Fixup: This template declaration should not be marked as compiler generated, fixing it ...\n");
                    templateDeclaration->get_file_info()->display("debug");
#endif
                 // DQ (10/21/2006): This call to unsetCompilerGenerated() was placed inside the if-endif above by accident.
                 // templateDeclaration->get_file_info()->unsetCompilerGenerated();
                    templateDeclaration->unsetCompilerGenerated();
                  }
               ROSE_ASSERT(templateDeclaration->get_file_info()->isCompilerGenerated() == false);
#if 0
            // DQ (10/21/2007): Parents have not been set yet, so this is too early to mark friend template function declarations here.
#endif
               break;
             }

          case V_SgClassDeclaration:
             {
               SgClassDeclaration* s = isSgClassDeclaration(node);
               ROSE_ASSERT (s != NULL);

               if ( (s->isForward() == true) && (s->get_definition() != NULL) )
                  {
#if 0
                    printf ("Found a forward class declaration with valid pointer to a definition (reset definition to NULL) \n");
#endif

                 // DQ (3/3/2007): I would like to allow definitions to be pointed to and for s->isForward() 
                 // to be all that is required to make a forward declaration!  At least for classes, but 
                 // later for functions as well.
                 // s->set_definition(NULL);
                 // printf ("FixupTemplateDeclarations::visit(): Commented out explicit setting of definition to NULL in class declaration marked as forward declaration. \n");
                  }
               break;
             }

          case V_SgFunctionDeclaration:
          case V_SgMemberFunctionDeclaration:
             {
               SgFunctionDeclaration* s = isSgFunctionDeclaration(node);
               ROSE_ASSERT (s != NULL);

               if ( (s->isForward() == true) && (s->get_definition() != NULL) )
                  {
#if 0
                    printf ("Found a forward function declaration with valid pointer to a definition (reset definition to NULL) \n");
#endif
                    s->set_definition(NULL);
                  }
               break;
             }

          case V_SgTemplateInstantiationDecl:
             {
               SgTemplateInstantiationDecl* s = isSgTemplateInstantiationDecl(node);
               ROSE_ASSERT (s != NULL);
               ROSE_ASSERT (s->get_templateDeclaration() != NULL);

            // printf ("In FixupTemplateDeclarations::visit(): SgTemplateInstantiationDecl = %p \n",s);
            // s->get_startOfConstruct()->display("In FixupTemplateDeclarations::visit(): SgTemplateInstantiationDecl: debug");

            // DQ (6/17/2005): Template declarations should not be marked as compiler generated 
            // (only the instatiations are posibily marked as compiler generated).
               if (s->get_templateDeclaration()->get_file_info()->isCompilerGenerated() == true)
                  {
                    printf ("AST Fixup: This template declaration should not be compiler generated \n");
                    s->get_templateDeclaration()->get_file_info()->display("debug");
                 // s->get_templateDeclaration()->get_file_info()->unsetCompilerGenerated();
                    s->get_templateDeclaration()->unsetCompilerGenerated();
                  }
               ROSE_ASSERT (s->get_templateDeclaration()->get_file_info()->isCompilerGenerated() == false);

               if ( (s->isForward() == true) && (s->get_definition() != NULL) )
                  {
#if 0
                    printf ("Found a forward class template declaration with valid pointer to a definition (reset definition to NULL) \n");
#endif
                    s->set_definition((SgTemplateInstantiationDefn*)NULL);
                  }
               break;
             }

          case V_SgTemplateInstantiationFunctionDecl:
             {
               SgTemplateInstantiationFunctionDecl* s = isSgTemplateInstantiationFunctionDecl(node);
               ROSE_ASSERT (s != NULL);
            // DQ (5/8/2004): templateName() removed
            // ROSE_ASSERT (s->get_templateName().str() != NULL);
               ROSE_ASSERT (s->get_templateDeclaration() != NULL);

            // DQ (6/17/2005): Template declarations should not be marked as compiler generated 
            // (only the instatiations are posibily marked as compiler generated).
               if (s->get_templateDeclaration()->get_file_info()->isCompilerGenerated() == true)
                  {
// #if PRINT_DEVELOPER_WARNINGS
                    printf ("AST Fixup: This function template instantiation declaration should not be compiler generated \n");
                    s->get_templateDeclaration()->get_file_info()->display("debug");
// #endif
                 // DQ (12/21/2006): This call to unsetCompilerGenerated() was placed inside the if-endif above by accident.
                 // s->get_templateDeclaration()->get_file_info()->unsetCompilerGenerated();
                    s->get_templateDeclaration()->unsetCompilerGenerated();
                  }
               ROSE_ASSERT (s->get_templateDeclaration()->get_file_info()->isCompilerGenerated() == false);

               if ( (s->isForward() == true) && (s->get_definition() != NULL) )
                  {
#if 0
                    printf ("Found a forward function template declaration with valid pointer to a definition (reset definition to NULL) \n");
#endif
                    s->set_definition(NULL);
                  }
               break;
             }

          case V_SgTemplateInstantiationMemberFunctionDecl:
             {
               SgTemplateInstantiationMemberFunctionDecl* s = isSgTemplateInstantiationMemberFunctionDecl(node);
               ROSE_ASSERT (s != NULL);
            // DQ (5/8/2004): templateName() removed
            // ROSE_ASSERT (s->get_templateName().str() != NULL);
               ROSE_ASSERT (s->get_templateDeclaration() != NULL);

            // DQ (6/17/2005): Template declarations should not be marked as compiler generated 
            // (only the instatiations are posibily marked as compiler generated).
               if (s->get_templateDeclaration()->get_file_info()->isCompilerGenerated() == true)
                  {
// #if PRINT_DEVELOPER_WARNINGS
                    printf ("AST Fixup: This member function template instantiation declaration should not be compiler generated \n");
                    s->get_templateDeclaration()->get_file_info()->display("debug");
// #endif
                 // DQ (10/21/2006): This call to unsetCompilerGenerated() was placed inside the if-endif above by accident.
                 // s->get_templateDeclaration()->get_file_info()->unsetCompilerGenerated();
                    s->get_templateDeclaration()->unsetCompilerGenerated();
                  }
               ROSE_ASSERT (s->get_templateDeclaration()->get_file_info()->isCompilerGenerated() == false);

               if ( (s->isForward() == true) && (s->get_definition() != NULL) )
                  {
#if 0
                    printf ("Found a forward member function template declaration with valid pointer to definition (reset definition to NULL) \n");
#endif
                    s->set_definition(NULL);
                  }
               break;
             }

          default:
             {
            // no other cases appear to be a problem!
             }
        }

  // DQ (5/11/2006): Function parameter declarations seem to often be set incorrectly (reset them to a better value)
     SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(node);
     if (functionDeclaration != NULL)
        {
#if 0
          printf ("Fixup Declarations: function name = %s \n",functionDeclaration->get_name().str());
#endif
          SgFunctionParameterList * functionParameterList = functionDeclaration->get_parameterList();
       // SgInitializedNamePtrList & list = functionDeclaration->get_args();
          SgInitializedNamePtrList & list = functionParameterList->get_args();
          SgInitializedNamePtrList::iterator i = list.begin();
          while (i != list.end())
             {
               SgInitializedName* declaration = *i;
               ROSE_ASSERT(declaration != NULL);
#if 0
               printf ("function parameter = %s id = %d = %s \n",
                    (*i)->get_name().str(),(*i)->get_file_info()->get_file_id(),(*i)->get_file_info()->get_filenameString().c_str());
               (*i)->get_file_info()->display("function parameter");
#endif
               if (declaration->get_file_info() == NULL)
                  {
                    printf ("Found a SgInitializedName without a valid Sg_File_Info pointer: declaration = %p = %s \n",declaration,declaration->get_name().str());
                  }
               ROSE_ASSERT(declaration->get_file_info() != NULL);
               if (declaration->get_file_info()->get_file_id() < 0) // == Sg_File_Info::NULL_FILE_ID)
                  {
#if 0
                    functionParameterList->get_startOfConstruct()->display("startOfConstruct");
                    functionParameterList->get_endOfConstruct()->display("endOfConstruct");
#endif
                    if (functionParameterList->get_startOfConstruct()->isSameFile(functionParameterList->get_endOfConstruct()) == true)
                       {
                      // Delete NULL_FILE version the and build a new Sg_File_Info based on the one from the function parameter list.
                         delete declaration->get_file_info();
                         declaration->set_file_info(new Sg_File_Info(*(functionParameterList->get_startOfConstruct())));
                       }
                      else
                       {
                         printf ("Error: start of function declaration not in same file as end of declaration \n");
                         printf ("Error: function parameter declaration is listed as NULL_FILE (reset) \n");

                         functionParameterList->get_startOfConstruct()->display("Error: start of function declaration not in same file as end of declaration (debug startOfConstruct)");
                         functionParameterList->get_endOfConstruct()->display("Error: start of function declaration not in same file as end of declaration (debug endOfConstruct)");
                         ROSE_ASSERT(false);
                       }
                  }

               i++;
             }
        }
   }



