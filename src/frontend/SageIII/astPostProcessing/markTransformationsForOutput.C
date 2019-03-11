// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "markTransformationsForOutput.h"

void
markTransformationsForOutput( SgNode* node )
   {
  // This function marks the subtrees of any IR node marked as a transformation.
  // This is required since the unparsing will check for either transformation 
  // or IR nodes marked explicitly for output (typically compiler generated IR nodes).

  // DQ (7/7/2005): Introduce tracking of performance of ROSE.
     TimingPerformance timer ("Mark transformations for output:");

     MarkTransformationsForOutput declarationFixupTraversal;

  // This inherited attribute is used for all traversals (within the iterative approach we define)
     MarkTransformationsForOutputInheritedAttribute inheritedAttribute;

  // This will be called iteratively so that we can do a fixed point iteration
     declarationFixupTraversal.traverse(node,inheritedAttribute);
   }

MarkTransformationsForOutputInheritedAttribute
MarkTransformationsForOutput::evaluateInheritedAttribute ( 
   SgNode* node,
   MarkTransformationsForOutputInheritedAttribute inheritedAttribute )
   {
  // Build a return value
     MarkTransformationsForOutputInheritedAttribute returnAttribute = inheritedAttribute;

  // Mark this explicitly as false to turn off effect of SgGlobal turning it on (???)
  // returnAttribute.insideTransformationToOutput = false;

     Sg_File_Info* fileInfo = node->get_file_info();
     if ( fileInfo != NULL )
        {
#if 0
          SgStatement* stmt = isSgStatement(node);
          if (stmt != NULL)
             {
               printf ("MarkTransformationsForOutput: stmt = %p = %s = %s \n",stmt,stmt->class_name().c_str(),SageInterface::get_name(stmt).c_str());
             }
#endif
#if 0
          printf ("MarkTransformationsForOutput: node = %p = %s = %s fileinfo = %p insideTransformationToOutput = %s fileInfo->isTransformation() = %s \n",node,node->class_name().c_str(),SageInterface::get_name(node).c_str(),fileInfo,returnAttribute.insideTransformationToOutput ? "true" : "false",fileInfo->isTransformation() ? "true" : "false");
#endif
          if (returnAttribute.insideTransformationToOutput == true)
             {
               // Liao 4/32/2011
               // We want to keep the original file info for PragmaDeclaration of the following code:
               // for () // similarly for if-stmt, etc
               //   #pragma omp 
                //   single-statement; 
               // the frontend will fix the for loop's body with a basic block, which has transformation generated file info

               bool bkeep= false;
               if (isSgPragmaDeclaration(node)) 
                   if (isSgBasicBlock(node->get_parent()))
                   {
                     SgLocatedNode * grand_parent = isSgLocatedNode(node->get_parent()->get_parent());
                    // if (isSgForStatement(node->get_parent()->get_parent())
                     //    || isSgIfStmt(node->get_parent()->get_parent()))
                     if (grand_parent)
                     {
                       if (grand_parent->get_file_info()->isTransformation()==false) 
                         bkeep = true;
                     }
                   }

            // Mark the IR node as a transformation if it is not already marked.
               if ( fileInfo->isTransformation() == false && !bkeep)
                  {
#if 0
                    printf ("MarkTransformationsForOutput: Reset the fileInfo to be a transformation \n");
#endif
                 // Mark the IR node as a transformation.

                 // DQ (3/20/2015): Mark this as a transformation uniformally for both the startOfConstruct and endOfConstruct seperately.
                 // Note: there is not uniform API at SgLocatedNode for marking the startOfConstruct and endOfConstruct together.
                 // fileInfo->setTransformation();
                    SgLocatedNode* locatedNode = isSgLocatedNode(node);
                    if (locatedNode != NULL)
                       {
                         ROSE_ASSERT(locatedNode->get_startOfConstruct() != NULL);
                         locatedNode->get_startOfConstruct()->setTransformation();
                         ROSE_ASSERT(locatedNode->get_endOfConstruct() != NULL);
                         locatedNode->get_endOfConstruct()->setTransformation();
                       }
                      else
                       {
                      // I don't think this case will be executed.
                         fileInfo->setTransformation();
                       }

                 // DQ (3/8/2006): Not clear if we always want to do this!
                 // printf ("In MarkTransformationsForOutput: Also mark transformation for output node = %s \n",node->class_name().c_str());
                    fileInfo->setOutputInCodeGeneration();
                  }
             }
            else
             {
            // If this is root of a subtree the check the IR node and mark 
            // the inherited attribute that we return.
               if ( fileInfo->isTransformation() == true )
                  {
                 // Mark the inherited attribute so that this whole subtree 
                 // (from this IR node down) will be marked as a transformation.
#if 0
                    printf ("MarkTransformationsForOutput: Reset the subtree to be a transformation \n");
#endif
                    returnAttribute.insideTransformationToOutput = true;
#if 1
                 // DQ (5/16/2017): Test if this is the cause of the fortran code generation issue (for the outliner).  It IS!
                 // DQ (5/17/2017): Liao helped with a better fix, which was to not build a prototype in the AST where the 
                 // outliner is being used on Frotran code.
                 // DQ (5/9/2017): We also have to mark the root of the subtree.
                    SgLocatedNode* locatedNode = isSgLocatedNode(node);
                    if (locatedNode != NULL)
                       {
                      // DQ (5/9/2017): Use the updated (simpler) API for this.
                      // ROSE_ASSERT(locatedNode->get_startOfConstruct() != NULL);
                      // locatedNode->get_startOfConstruct()->setTransformation();
                      // ROSE_ASSERT(locatedNode->get_endOfConstruct() != NULL);
                      // locatedNode->get_endOfConstruct()->setTransformation();
                         locatedNode->setTransformation();
                         locatedNode->setOutputInCodeGeneration();
                       }
#endif
                  }
             }

#if 1
       // DQ (8/16/2011): Debugging support added to output file info as part of the source position support for Java.
       // if (SgProject::get_verbose() > 3)
          if (SgProject::get_verbose() > 3)
             {
            // DQ (8/16/2011): Debugging output to make sure that we get all of the IR nodes set properly for Java.
               if (fileInfo->isTransformation() == true)
                  {
                    printf ("MarkTransformationsForOutput (isTransformation() == true): node = %p = %s = %s fileinfo = %p insideTransformationToOutput = %s \n",node,node->class_name().c_str(),SageInterface::get_name(node).c_str(),fileInfo,returnAttribute.insideTransformationToOutput ? "true" : "false");
                  }

               if (fileInfo->isCompilerGenerated() == true)
                  {
                    printf ("MarkTransformationsForOutput (isCompilerGenerated() == true): node = %p = %s = %s fileinfo = %p \n",node,node->class_name().c_str(),SageInterface::get_name(node).c_str(),fileInfo);
                  }

               if (fileInfo->isFrontendSpecific() == true)
                  {
                    printf ("MarkTransformationsForOutput (isFrontendSpecific() == true): node = %p = %s = %s fileinfo = %p \n",node,node->class_name().c_str(),SageInterface::get_name(node).c_str(),fileInfo);
                  }

               if (fileInfo->isCompilerGenerated() == false && fileInfo->isTransformation() == false)
                  {
                    printf ("MarkTransformationsForOutput (normal): node = %p = %s = %s fileinfo = %p \n",node,node->class_name().c_str(),SageInterface::get_name(node).c_str(),fileInfo);
                  }
             }
#endif
        }

     return returnAttribute;
   }

