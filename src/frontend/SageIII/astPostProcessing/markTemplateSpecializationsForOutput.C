// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "markTemplateSpecializationsForOutput.h"
#include "markTemplateInstantiationsForOutput.h"

#ifdef ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
   #include "transformationSupport.h"
#endif

void
markTemplateSpecializationsForOutput( SgNode* node )
   {
  // This function marks template specialization to be output.

     ROSE_ASSERT(node != NULL);

#if 0
     printf ("In markTemplateSpecializationsForOutput(): This function marks template specialization to be output node = %p = %s \n",node,node->class_name().c_str());
#endif

  // DQ (7/7/2005): Introduce tracking of performance of ROSE.
     TimingPerformance timer ("Mark template specializations for output:");

  // DQ (8/2/2005): Added better handling of AST fragments where template handling is not required!
  // DQ (7/29/2005): Added support with Qing for AST framents that occure in the ASTInterface classes.
     SgSourceFile* file = NULL;

     if (node->get_parent() == NULL)
        {
#if 0
          printf ("In markTemplateSpecializationsForOutput(): Detected AST fragement not associated with primary AST, ignore template handling ... \n");
#endif
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

                 // printf ("Calling markTemplateInstantiationsForOutput() for file = %p = %s \n",file,file->class_name().c_str());
                    markTemplateInstantiationsForOutput(file);
                 // printf ("DONE: Calling markTemplateInstantiationsForOutput() for file = %p = %s \n",file,file->class_name().c_str());
                  }
             }
        }
       else
        {
#if 0
          printf ("In markTemplateSpecializationsForOutput(): Getting the SgFile from the AST node \n");
#endif
          file = TransformationSupport::getSourceFile(node);
       // When processing templates we need to get the SgFile so that we can check the command line options.
       // ROSE_ASSERT(file != NULL);

       // printf ("current file is: %s \n",(file != NULL) ? file->getFileName() : "NULL SgFile pointer");

       // Note: since the currentFile must be valid, any template specializations in code fragements 
       // unattached to the primary AST will not be marked for output.  Later post processing, once 
       // the fragment is attached to the primary AST, will catch these cases.
          if (file != NULL)
             {
            // This simplifies how the traversal is called!
               MarkTemplateSpecializationsForOutput declarationFixupTraversal(file);

            // This inherited attribute is used for all traversals (within the iterative approach we define)
               MarkTemplateSpecializationsForOutputInheritedAttribute inheritedAttribute;

            // This will be called iteratively so that we can do a fixed point iteration
               declarationFixupTraversal.traverse(node,inheritedAttribute);
             }
        }
   }

MarkTemplateSpecializationsForOutputInheritedAttribute::MarkTemplateSpecializationsForOutputInheritedAttribute()
   {
     insideDeclarationToOutput = false;
   }

MarkTemplateSpecializationsForOutput::MarkTemplateSpecializationsForOutput (SgSourceFile* inputFile)
   {
  // Save the file in the traversal class so that we can access the backend specific 
  // template instatiation control flags easily.
  // ROSE_ASSERT(inputFile != NULL);
     currentFile = inputFile;
   }

MarkTemplateSpecializationsForOutputInheritedAttribute
MarkTemplateSpecializationsForOutput::evaluateInheritedAttribute ( 
   SgNode* node,
   MarkTemplateSpecializationsForOutputInheritedAttribute inheritedAttribute )
   {
     static int staticCounter = 0;
     MarkTemplateSpecializationsForOutputInheritedAttribute returnAttribute = inheritedAttribute;

#if 0
     printf ("In MarkTemplateSpecializationsForOutput::evaluateInheritedAttribute() node = %p = %s \n",node,node->class_name().c_str());
#endif

  // Mark this explicitly as false to turn off effect of SgGlobal turning it on
     returnAttribute.insideDeclarationToOutput = false;

  // Note: since the currentFile must be valid, any template specializations in code fragements 
  // unattached to the primary AST will not be marked for output.  Later post processing, once 
  // the fragment is attached to the primary AST, will catch these cases.

     Sg_File_Info* fileInfo = node->get_file_info();
     if ( currentFile != NULL && fileInfo != NULL )
        {
       // If this is marked for output then record this in the inherited attribute to be returned
          if ( fileInfo->isOutputInCodeGeneration() == true )
             {
//               printf ("Skipping nodes already marked to be unparsed = %p = %s \n",node,node->class_name().c_str());
            // returnAttribute.insideDeclarationToOutput = true;
            // printf ("Found compiler generated IR node to be unparsed = %s \n",node->sage_class_name());
             }
            else
             {
//               printf ("Evaluate nodes for if then need be marked to be unparsed = %p = %s \n",node,node->class_name().c_str());

            // Maybe SgGlobal should return false for hasPositionInSource()?
               if (fileInfo->hasPositionInSource() == true)
                  {
                 // This node has a position is some source code so we can check if it is part of the current file!
#if 0
                    printf ("In evaluateInheritedAttribute(): currentFile = %s IR node from %s at line %d \n",currentFile->getFileName(),fileInfo->get_filename(),fileInfo->get_line());
#endif
                    if ( (fileInfo->isSameFile(currentFile) == true) && (isSgGlobal(node) == NULL) )
                       {
                      // This is a node from the current file!
                         returnAttribute.insideDeclarationToOutput = true;
#if 0
                         printf ("Found IR node %s from source file = %s at %d \n",node->sage_class_name(),fileInfo->get_filename(),fileInfo->get_line());
#endif
#if 0
                         if (staticCounter > 1)
                            {
                              printf ("Exiting as a test ... \n");
                              ROSE_ABORT();
                            }
#endif
                         staticCounter++;
                       }
                  }
                 else
                  {
//                    printf ("In MarkTemplateSpecializationsForOutput evaluation fileInfo->hasPositionInSource() == false \n");
                  }
             }
        }

  // This marks the SgTemplateInstantiationMemberFunctionDecl objects
     SgTemplateInstantiationMemberFunctionDecl* templateInstantiationMemberFunctionDeclaration = isSgTemplateInstantiationMemberFunctionDecl(node);
     if ( templateInstantiationMemberFunctionDeclaration != NULL && templateInstantiationMemberFunctionDeclaration->isSpecialization() == true )
        {
          if (returnAttribute.insideDeclarationToOutput == true)
             {
#if 0
               printf ("Marking to be output in code generation: templateInstantiationMemberFunctionDeclaration = %p = %s \n",
                    templateInstantiationMemberFunctionDeclaration,
                    templateInstantiationMemberFunctionDeclaration->get_qualified_name().str());
#endif
            // templateInstantiationMemberFunctionDeclaration->get_file_info()->setOutputInCodeGeneration();
               templateInstantiationMemberFunctionDeclaration->setOutputInCodeGeneration();
             }
        }

     return returnAttribute;
   }

