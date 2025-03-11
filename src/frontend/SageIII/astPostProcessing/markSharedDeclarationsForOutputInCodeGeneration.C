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
   }

MarkSharedDeclarationsInheritedAttribute::MarkSharedDeclarationsInheritedAttribute()
   : currentFile(nullptr)
   {
   }

MarkSharedDeclarationsInheritedAttribute::MarkSharedDeclarationsInheritedAttribute( const MarkSharedDeclarationsInheritedAttribute & X )
   {
     currentFile   = X.currentFile;
     unparseFileIdList = X.unparseFileIdList;
   }

MarkSharedDeclarationsForOutputInCodeGeneration::MarkSharedDeclarationsForOutputInCodeGeneration()
   {
   }

MarkSharedDeclarationsInheritedAttribute
MarkSharedDeclarationsForOutputInCodeGeneration::evaluateInheritedAttribute(SgNode* node, MarkSharedDeclarationsInheritedAttribute inheritedAttribute)
   {
     MarkSharedDeclarationsInheritedAttribute return_inheritedAttribute(inheritedAttribute);

  // DQ (2/25/2019): Record the associated file id's of the source files.
     SgSourceFile* sourceFile = isSgSourceFile(node);
     if (sourceFile != nullptr)
        {
          ASSERT_not_null(sourceFile->get_file_info());
          int file_id = sourceFile->get_file_info()->get_file_id();

          fileNodeSet.insert(file_id);

          return_inheritedAttribute.currentFile = sourceFile;
        }
       else
        {
          sourceFile = inheritedAttribute.currentFile;

          return_inheritedAttribute.currentFile = sourceFile;
        }

  // Only make statements (skip expressions since compiler generated casts are not output!)
  // We should not be borrowing the compiler generated flag to mark IR statement nodes for output by the unparser!
     SgStatement* statement = isSgStatement(node);
     if (statement != nullptr)
        {
          ASSERT_not_null(statement->get_file_info());
          if (statement->get_file_info()->get_fileIDsToUnparse().empty() == false)
             {
               return_inheritedAttribute.unparseFileIdList = statement->get_file_info()->get_fileIDsToUnparse();
             }
            else
             {
               if (inheritedAttribute.unparseFileIdList.empty() == false)
                  {
                    statement->get_file_info()->get_fileIDsToUnparse() = inheritedAttribute.unparseFileIdList;
                  }
             }

        }

     return return_inheritedAttribute;
   }



