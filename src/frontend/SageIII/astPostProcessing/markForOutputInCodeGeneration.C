// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "astPostProcessing/markForOutputInCodeGeneration.h"

void
markForOutputInCodeGeneration( SgNode* node )
   {
  // DQ (7/11/2005): since this is called multiple times from different places
  // as part of a nested traversal, we should skip the redundent timings.
  // DQ (7/7/2005): Introduce tracking of performance of ROSE.
  // TimingPerformance timer ("(mark as compiler generated code to be unparsed) time (sec) = ");

  // This simplifies how the traversal is called!
     MarkForOutputInCodeGeneration astFixupTraversal;

  // I think the default should be preorder so that the interfaces would be more uniform
     astFixupTraversal.traverse(node,preorder);
   }

void
MarkForOutputInCodeGeneration::visit(SgNode* node)
   {
  // Only make statements (skip expressions since compiler generated casts are not output!)
  // We should not be borrowing the compiler generated flag to mark IR statement nodes for output by the unparser!
     SgStatement* statement = isSgStatement(node);
     if (statement != NULL)
        {
#if 0
          printf ("In MarkForOutputInCodeGeneration::visit(): Attempting to mark %s (%s) for output in code generation \n",node->class_name().c_str(),SageInterface::get_name(statement).c_str());
#endif
          ROSE_ASSERT(statement->get_file_info() != NULL);
          statement->get_file_info()->setOutputInCodeGeneration();

       // If this is a declaration then mark it's defining and non-defining declarations as well
          SgDeclarationStatement* declaration = isSgDeclarationStatement(statement);
          if (declaration != NULL)
             {
               SgDeclarationStatement* definingDeclaration    = declaration->get_definingDeclaration();
               SgDeclarationStatement* nondefiningDeclaration = declaration->get_firstNondefiningDeclaration();

               if (definingDeclaration != NULL)
                  {
                 // printf ("Marking definingDeclaration = %p \n",definingDeclaration);
                    definingDeclaration->get_file_info()->setOutputInCodeGeneration();
                  }
               if (nondefiningDeclaration != NULL)
                  {
                 // printf ("Marking nondefiningDeclaration = %p \n",nondefiningDeclaration);
                    nondefiningDeclaration->get_file_info()->setOutputInCodeGeneration();
                  }
             }
        }
   }

