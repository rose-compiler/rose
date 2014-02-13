// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "astPostProcessing/fixupTemplateInstantiations.h"
#include "markCompilerGenerated.h"
void
fixupTemplateInstantiations( SgNode* node )
   {
  // DQ (7/7/2005): Introduce tracking of performance of ROSE.
     TimingPerformance timer ("Fixup template specializations:");

  // This simplifies how the traversal is called!
     FixupTemplateInstantiations declarationFixupTraversal;

  // I think the default should be preorder so that the interfaces would be more uniform
     declarationFixupTraversal.traverse(node,preorder);
   }

void
FixupTemplateInstantiations::visit (SgNode* node)
   {
     ROSE_ASSERT(node != NULL);

  // Take care of marking the whole subtree of any declarations
  // that the EDG/Sage connection marked as compiler generated.
     SgDeclarationStatement* declaration = isSgDeclarationStatement(node);

  // DQ (1/18/2014): Testcode test2012_75.c demonstrates why we need to force the function 
  // parameters to be marked as compiler generated.  Else there are errors in how comments 
  // are woven back into the AST.
  // DQ (1/18/2014): Skip function parameter lists, since they are always marked as
  // compiler generated (because we don't have source position information for them).  
  // Perhaps marking it as frontend specific would be more appropriate).
  // if (declaration != NULL)
  // if (declaration != NULL && isSgFunctionParameterList(declaration) == NULL)
     if (declaration != NULL)
        {
          if (declaration->get_file_info() == NULL)
             {
               printf ("Error: (declaration->get_file_info() == NULL) declaration = %p = %s = %s \n",declaration,declaration->class_name().c_str(),SageInterface::get_name(declaration).c_str());
             }           
          ROSE_ASSERT(declaration->get_file_info() != NULL);

       // DQ (6/17/2005): compiler generated does not imply that it will be output by the unparser (anymore)
       // Some declarations are marked as compiler generated in the EDG/Sage III translation, but the whole 
       // subtree is never marked at that point.  This step marks the whole subtree as compiler generated 
       // when just the declaration is detected as having been marked in the EDG/Sage III translation.
          if (declaration->get_file_info()->isCompilerGenerated() == true)
             {
#if 0
               std::string name = SageInterface::get_name(declaration);
               printf ("declaration = %p = %s is marked as compiler generated (mark the rest of the subtree similarly) \n",declaration,name.c_str());
#endif

            // DQ (8/10/2005): We should never mark a template declaration as compiler generated 
            // (though perhaps partial specializations could be marked as such later).
            // ROSE_ASSERT(isSgTemplateDeclaration(node) == NULL);
               if (isSgTemplateDeclaration(node) == NULL)
                  {
                 // Mark the whole declaration as compiler generated since we could not do so in the EDG/Sage III translation
#if 0
                    printf ("In FixupTemplateInstantiations::visit(): Calling markAsCompilerGenerated: declaration = %p = %s \n",declaration,declaration->class_name().c_str());
#endif
                    markAsCompilerGenerated(declaration);
                  }
             }
        }

#if 0
  // Now mark some subtrees as appropriate for output in the generated source code

  // Mark all the template class specializations as compiler generated
     SgTemplateInstantiationDecl* classDeclaration = isSgTemplateInstantiationDecl(node);
     if (classDeclaration != NULL)
        {
       // DQ (6/8/2005): This is now handled directly in the EDG/Sage III translation.
       // Not all classes should be marked as compiler generated (e.g. explicit 
       // specializations appearing in the source code (or in header files)).

       // DQ (5/27/2005): Mark the declaration and its subtree (for all SgTemplateInstantiationFunctionDecl IR nodes)
       // printf ("Marking SgTemplateInstantiationDecl = %p as compiler generated \n",classDeclaration);
       // classDeclaration->get_file_info()->display("Before marking as compiler generated");
       // printf ("Skipping marking template %p as compiler generated! \n",classDeclaration);
       // markAsCompilerGenerated(classDeclaration);

       // DQ (6/10/2005): Fixup the children of the classDeclaration which is marked as compiler 
       // generated buy the EDG/Sage connection!

       // printf ("Calling isFirstUseInSourceFile \n");
          SgSourceFile* file = TransformationSupport::getSourceFile(node);
          ROSE_ASSERT(file != NULL);
       // printf ("current file is: %s \n",file->getFileName());

          bool outputInSourceFile = isFirstUseInSourceFile(classDeclaration,file);

          if (outputInSourceFile == true)
             {
            // This might already be marked (check!)
            // ROSE_ASSERT ( classDeclaration->get_file_info()->isCompilerGenerated() == true );
            // The EDG/Sage connection only marks the class declaration as compiler generated and 
            // does not have the mechanism in place at that point to mark all children in the declaration 
            // as compiler generated.
               markForOutputInCodeGeneration(classDeclaration);

            // Make sure that the classDeclaration has not be accedentally reset to not be marked as compiler generated!
               ROSE_ASSERT ( classDeclaration->get_file_info()->isCompilerGeneratedNodeToBeUnparsed() == true );
             }
            else
             {
            // The EDG/Sage connect makes all instantiated templates to be compiler generated

            // DQ (12/21/2006): Mark this uniformally using the SgLoctedNode member function.
            // classDeclaration->get_file_info()->unsetCompilerGenerated();
               classDeclaration->unsetCompilerGenerated();
               ROSE_ASSERT ( classDeclaration->get_file_info()->isCompilerGeneratedNodeToBeUnparsed() == false );
             }

#if 0
          if (classDeclaration->get_file_info()->isCompilerGenerated() == true)
             {
            // The EDG/Sage connection only marks the class declaration as compiler generated and 
            // does not have the mechanism in place at that point to mark all children in the declaration 
            // as compiler generated.
               markForOutputInCodeGeneration(classDeclaration);

            // Make sure that the classDeclaration has not be accedentally reset to not be marked as compiler generated!
               ROSE_ASSERT ( classDeclaration->get_file_info()->isCompilerGeneratedNodeToBeUnparsed() == true );
             }
#endif
        }

  // DQ (6/20/2005): Note that if these functions are not called in the source, but call one another in 
  // the template function specializations directly then they will still for a dependence which will 
  // require them to be output (so this could lead to a file-based fixed-point iteration!).

  // These are output a static functions to avoid being defined using multiple symbols at link time.
  // This is a temporary fix until we get the better prelinker written.
     SgTemplateInstantiationFunctionDecl* functionDeclaration = isSgTemplateInstantiationFunctionDecl(node);
     if (functionDeclaration != NULL)
        {
       // DQ (5/27/2005): Mark the declaration and its subtree (for all SgTemplateInstantiationFunctionDecl IR nodes)
       // printf ("Calling isFirstUseInSourceFile \n");
          SgSourceFile* file = TransformationSupport::getSourceFile(node);
          ROSE_ASSERT(file != NULL);
       // printf ("current file is: %s \n",file->getFileName());

          bool outputInSourceFile = isFirstUseInSourceFile(functionDeclaration,file);

       // DQ (6/20/2005): IR nodes are now marked explicitly if they are to be output by the unparser.
          if (outputInSourceFile == true)
             {
               markForOutputInCodeGeneration(functionDeclaration);

            // Make sure that the classDeclaration has not be accedentally reset to not be marked as compiler generated!
               ROSE_ASSERT ( functionDeclaration->get_file_info()->isCompilerGeneratedNodeToBeUnparsed() == true );
             }
        }

  // These are inlined if they exist and so we don't have to output them as static functions
  // it is not clear if we need to output them if they are not called!
     SgTemplateInstantiationMemberFunctionDecl* memberFunctionDeclaration = isSgTemplateInstantiationMemberFunctionDecl(node);
     if (memberFunctionDeclaration != NULL)
        {
#if 0
       // DQ (5/27/2005): Mark the declaration and its subtree (for all SgTemplateInstantiationFunctionDecl IR nodes)
          markForOutputInCodeGeneration(memberFunctionDeclaration);

       // Make sure that the classDeclaration has not be accedentally reset to not be marked as compiler generated!
          ROSE_ASSERT ( memberFunctionDeclaration->get_file_info()->isCompilerGeneratedNodeToBeUnparsed() == true );
#else
          SgSourceFile* file = TransformationSupport::getSourceFile(node);
          ROSE_ASSERT(file != NULL);
       // printf ("current file is: %s \n",file->getFileName());

          bool outputInSourceFile = isFirstUseInSourceFile(memberFunctionDeclaration,file);

       // DQ (6/20/2005): IR nodes are now marked explicitly if they are to be output by the unparser.
          if (outputInSourceFile == true)
             {
            // Make sure that the classDeclaration has not be accedentally reset to not be marked as compiler generated!
               ROSE_ASSERT ( functionDeclaration->get_file_info()->isCompilerGeneratedNodeToBeUnparsed() == true );
             }
#endif
        }
#endif
   }

