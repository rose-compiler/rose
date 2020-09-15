// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "processTemplateHandlingOptions.h"
#include "markTemplateInstantiationsForOutput.h"
// #include "Cxx_Grammar.h"
// #include "AstNodePtrs.h"

// [MS]: required because we use the STL find algorithm
// DQ (12/7/2003): use platform independent macro defined in config.h
// #include STL_ALGO_HEADER_FILE

// Permited to include AST testing
// #include "AstTraversal.h"

#ifdef ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
   #include "transformationSupport.h"
#endif

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

void processTemplateHandlingOptions( SgNode* node )
   {
  // DQ (7/7/2005): Introduce tracking of performance of ROSE.
     TimingPerformance timer ("Fixup templateHandlingOptions:");

     ROSE_ASSERT(node != NULL);
     SgFile* file = TransformationSupport::getFile(node);

     if (file == NULL)
        {
       // printf ("Detected AST fragement not associated with primary AST, ignore template handling ... \n");

          SgProject *project = isSgProject(node);
          if (project != NULL)
             {
            // GB (9/4/2009): Added this case for handling SgProject nodes. We do
            // this simply by iterating over the list of files in the project and
            // calling this function recursively. This is only one level of
            // recursion since files are not nested.
               SgFilePtrList &files = project->get_fileList();
               SgFilePtrList::iterator fIterator;
               for (fIterator = files.begin(); fIterator != files.end(); ++fIterator)
                  {
                    SgFile *file = *fIterator;
                    ROSE_ASSERT(file != NULL);
                    markTemplateInstantiationsForOutput(file);
                  }
             }
        }
       else
        {
          bool buildImplicitTemplates       = (file != NULL) && (file->get_no_implicit_templates() == false);
          bool buildImplicitInlineTemplates = (file != NULL) && (file->get_no_implicit_inline_templates() == false);
#if 0
          printf ("buildImplicitTemplates       = %s \n",buildImplicitTemplates ? "true" : "false");
          printf ("buildImplicitInlineTemplates = %s \n",buildImplicitInlineTemplates ? "true" : "false");
#endif
       // This simplifies how the traversal is called!
          ProcessTemplateHandlingOptions astFixupTraversal(file,buildImplicitTemplates,buildImplicitInlineTemplates);

       // I think the default should be preorder so that the interfaces would be more uniform
          astFixupTraversal.traverse(node,preorder);
        }
   }

ProcessTemplateHandlingOptions::
ProcessTemplateHandlingOptions ( SgFile* file, bool buildImplicitTemplates, bool buildImplicitInlineTemplates )
   : file(file),buildImplicitTemplates(buildImplicitTemplates),buildImplicitInlineTemplates(buildImplicitInlineTemplates)
   {
  // Constructor
#if 0
     printf ("In ProcessTemplateHandlingOptions(): current file                 = %p \n",file);
     printf ("In ProcessTemplateHandlingOptions(): buildImplicitTemplates       = %s \n",buildImplicitTemplates ? "true" : "false");
     printf ("In ProcessTemplateHandlingOptions(): buildImplicitInlineTemplates = %s \n",buildImplicitInlineTemplates ? "true" : "false");
#endif
   }


void
ProcessTemplateHandlingOptions::visit ( SgNode* node )
   {
  // DQ (8/19/2005): Unmark implicit template instantiations that would be output

  // printf ("node = %s \n",node->sage_class_name());

     switch (node->variantT())
        {
          case V_SgTemplateInstantiationDecl:
             {
               SgTemplateInstantiationDecl* s = isSgTemplateInstantiationDecl(node);
               if (buildImplicitTemplates == false)
                  {
                     ROSE_ASSERT(s != NULL);
                     Sg_File_Info* fInfo = s->get_file_info();
                     ROSE_ASSERT(fInfo != NULL);
                 // Mark all template class instantiations except specialized template 
                 // instantiations to NOT be output in code generation
                    if (fInfo->isOutputInCodeGeneration() == true && s->isSpecialization() == false)
                       {
                         printf ("AST Fixup: This template class will not be output within code generation \n");
                         fInfo->unsetOutputInCodeGeneration();
                       }
                    ROSE_ASSERT (fInfo->isOutputInCodeGeneration() == false || s->isSpecialization() == true);
                  }
               break;
             }

          case V_SgTemplateInstantiationFunctionDecl:
             {
               SgTemplateInstantiationFunctionDecl* s = isSgTemplateInstantiationFunctionDecl(node);
               if (buildImplicitTemplates == false)
                  {
                 // Mark all template function instantiations except specialized template 
                 // instantiations to NOT be output in code generation
                    if (s->get_file_info()->isOutputInCodeGeneration() == true && s->isSpecialization() == false)
                       {
                         printf ("AST Fixup: This template function will not be output within code generation \n");
                         s->get_file_info()->unsetOutputInCodeGeneration();
                       }
                    ROSE_ASSERT (s->get_file_info()->isOutputInCodeGeneration() == false || s->isSpecialization() == true);
                  }
               break;
             }

          case V_SgTemplateInstantiationMemberFunctionDecl:
             {
               SgTemplateInstantiationMemberFunctionDecl* s = isSgTemplateInstantiationMemberFunctionDecl(node);
               if (buildImplicitTemplates == false && buildImplicitInlineTemplates == false)
                  {
                 // Mark all template member function instantiations except specialized template 
                 // instantiations to NOT be output in code generation
                    if (s->get_file_info()->isOutputInCodeGeneration() == true && s->isSpecialization() == false)
                       {
                         printf ("AST Fixup: This template member function will not be output within code generation \n");
                         s->get_file_info()->unsetOutputInCodeGeneration();
                       }
                    ROSE_ASSERT (s->get_file_info()->isOutputInCodeGeneration() == false || s->isSpecialization() == true);
                  }
               break;
             }

          default:
             {
            // no other cases appear to be a problem!
             }
        }
   }


