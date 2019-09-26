// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "astPostProcessing/markSharedDeclarationsForOutputInCodeGeneration.h"

void
markSharedDeclarationsForOutputInCodeGeneration( SgNode* node )
   {
  // DQ (7/11/2005): since this is called multiple times from different places
  // as part of a nested traversal, we should skip the redundent timings.
  // DQ (7/7/2005): Introduce tracking of performance of ROSE.
  // TimingPerformance timer ("(mark as compiler generated code to be unparsed) time (sec) = ");

     MarkSharedDeclarationsInheritedAttribute inheritedAttribute;

  // This simplifies how the traversal is called!
     MarkSharedDeclarationsForOutputInCodeGeneration astFixupTraversal;

     astFixupTraversal.traverse(node,inheritedAttribute);

#if 0
  // DQ (2/27/2019): Adding debugging support.
     printf ("In markSharedDeclarationsForOutputInCodeGeneration(): Output all shared AST noded: \n");
     SageInterface::outputSharedNodes(node);
     printf ("In markSharedDeclarationsForOutputInCodeGeneration(): DONE: Output all shared AST noded: \n");
#endif
   }


MarkSharedDeclarationsInheritedAttribute::MarkSharedDeclarationsInheritedAttribute()
   : currentFile(NULL) 
   {
   }

MarkSharedDeclarationsInheritedAttribute::MarkSharedDeclarationsInheritedAttribute( const MarkSharedDeclarationsInheritedAttribute & X )
   {
     currentFile   = X.currentFile;

  // SgFileIdList unparseFileIdList;
     unparseFileIdList = X.unparseFileIdList;
   }


MarkSharedDeclarationsForOutputInCodeGeneration::MarkSharedDeclarationsForOutputInCodeGeneration()
   {
   // currentSourceFile = NULL;
   }


// void MarkSharedDeclarationsForOutputInCodeGeneration::visit(SgNode* node)
MarkSharedDeclarationsInheritedAttribute
MarkSharedDeclarationsForOutputInCodeGeneration::evaluateInheritedAttribute(SgNode* node, MarkSharedDeclarationsInheritedAttribute inheritedAttribute)
   {
     MarkSharedDeclarationsInheritedAttribute return_inheritedAttribute(inheritedAttribute);

  // DQ (2/25/2019): Record the associated file id's of the source files.
     SgSourceFile* sourceFile = isSgSourceFile(node);
     if (sourceFile != NULL)
        {
#if 0
          printf ("MarkSharedDeclarationsForOutputInCodeGeneration::visit(): sourceFile->get_sourceFileNameWithoutPath() = %s \n",sourceFile->get_sourceFileNameWithoutPath().c_str());
#endif
          ROSE_ASSERT(sourceFile->get_file_info() != NULL);
          int file_id = sourceFile->get_file_info()->get_file_id();

          fileNodeSet.insert(file_id);

          return_inheritedAttribute.currentFile = sourceFile;
#if 0
          printf (" --- file_id            = %d \n",file_id);
          printf (" --- fileNodeSet.size() = %zu \n",fileNodeSet.size());
#endif
        }
       else
        {
          sourceFile = inheritedAttribute.currentFile;

          return_inheritedAttribute.currentFile = sourceFile;
        }

  // Only make statements (skip expressions since compiler generated casts are not output!)
  // We should not be borrowing the compiler generated flag to mark IR statement nodes for output by the unparser!
     SgStatement* statement = isSgStatement(node);
     if (statement != NULL)
        {
#if 0
       // printf ("In MarkSharedDeclarationsForOutputInCodeGeneration::visit(): Attempting to mark %s (%s) for output in code generation \n",node->class_name().c_str(),SageInterface::get_name(statement).c_str());
          printf ("In MarkSharedDeclarationsForOutputInCodeGeneration::visit(): Attempting to mark %s for output in code generation \n",node->class_name().c_str());
#endif
          ROSE_ASSERT(statement->get_file_info() != NULL);
       // statement->get_file_info()->setOutputInCodeGeneration();
          if (statement->get_file_info()->get_fileIDsToUnparse().empty() == false)
             {
#if 0
               printf ("statement->get_file_info()->get_fileIDsToUnparse().size() = %zu \n",statement->get_file_info()->get_fileIDsToUnparse().size());
#endif
               return_inheritedAttribute.unparseFileIdList = statement->get_file_info()->get_fileIDsToUnparse();
             }
            else
             {
               if (inheritedAttribute.unparseFileIdList.empty() == false)
                  {
#if 0
                    printf ("inheritedAttribute.unparseFileIdList.size() = %zu \n",inheritedAttribute.unparseFileIdList.size());
#endif
                    statement->get_file_info()->get_fileIDsToUnparse() = inheritedAttribute.unparseFileIdList;
                  }
             }

#if 0
          printf (" --- statement->get_file_info()->get_fileIDsToUnparse().size() = %zu \n",statement->get_file_info()->get_fileIDsToUnparse().size());
#endif

#if 0
       // If this is a declaration then mark it's defining and non-defining declarations as well
          SgDeclarationStatement* declaration = isSgDeclarationStatement(statement);
          if (declaration != NULL)
             {
               SgDeclarationStatement* definingDeclaration    = declaration->get_definingDeclaration();

               if (definingDeclaration != NULL)
                  {
                    if (declaration == definingDeclaration)
                       {
                         printf ("Found definingDeclaration = %p = %s previousScope = %p = %s \n",definingDeclaration,definingDeclaration->class_name().c_str(),previousScope,previousScope->class_name().c_str());

                      // Check if this defining declaration has been accounted for in a source file.
                         if (definingDeclarationsSet.find(definingDeclaration) != definingDeclarationsSet.end())
                            {
                           // Make sure this is from the source file and not an included file.
                              if (definingDeclaration->get_file_info()->get_file_id() == sourceFile->get_file_info()->get_file_id())
                                 {
                                   ROSE_ASSERT(sourceFile != NULL);
                                   printf ("Mark definng declaration to be output from: sourceFile->get_sourceFileNameWithoutPath() = %s \n",sourceFile->get_sourceFileNameWithoutPath().c_str());

                                   definingDeclarationsSet.insert(definingDeclaration);
#if 1
                                   printf ("Put definingDeclaration = %p = %s into definingDeclarationsSet: size = %zu \n",
                                        definingDeclaration,definingDeclaration->class_name().c_str(),definingDeclarationsSet.size());
#endif
                                 }
                            }
                           else
                            {
#if 1
                              printf ("This defining declaration is already in the definingDeclarationsSet: definingDeclaration = %p = %s \n",definingDeclaration,definingDeclaration->class_name().c_str());
#endif
                           // This defining declaration should be marked for output from the current source file.
                              ROSE_ASSERT(sourceFile != NULL);
                              printf ("Mark definng declaration to be output from: sourceFile->get_sourceFileNameWithoutPath() = %s \n",sourceFile->get_sourceFileNameWithoutPath().c_str());



                              printf ("Marking definingDeclaration = %p \n",definingDeclaration);
                              definingDeclaration->get_file_info()->setOutputInCodeGeneration();

                            }
                       }
                  }
             }
#endif
        }

     return return_inheritedAttribute;
   }



