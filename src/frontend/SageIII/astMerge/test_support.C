// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "collectAssociateNodes.h"
#include "test_support.h"
#include "merge_support.h"

using namespace std;

void
TestUniqueNameGenerationTraversal::visit ( SgNode* node)
   {
     ROSE_ASSERT(node != NULL);
#if 0
     printf ("TestUniqueNameGenerationTraversal::visit: node = %p = %s \n",node,node->class_name().c_str());
#endif


#if 0
     printf ("TestUniqueNameGenerationTraversal::visit(): node = %p = %s key = %s \n",node,node->class_name().c_str(),key.c_str());
#endif

  // ROSE_ASSERT(key.empty() == false);
   }

void
testUniqueNameGenerationTraversal()
   {
  // DQ (2/2/2007): Introduce tracking of performance of within AST merge
     TimingPerformance timer ("Test the construction of mangled names:");

     TestUniqueNameGenerationTraversal t;
     t.traverseMemoryPool();
   }


void
ReportUnsharedDeclarationsTraversal::visit ( SgNode* node)
   {
     ROSE_ASSERT(node != NULL);
#if 0
     printf ("ReportUnsharedDeclarationsTraversal::visit: node = %p = %s \n",node,node->class_name().c_str());
#endif

     SgDeclarationStatement* declaration = isSgDeclarationStatement(node);
     if (declaration != NULL)
        {
          bool skipTest = isSgFunctionParameterList(declaration) || isSgNamespaceDeclarationStatement(declaration) || isSgCtorInitializerList(declaration);
          if (skipTest == false && declaration->get_startOfConstruct()->isShared() == false)
             {
               printf ("Found a declaration which is not shared declaration = %p = %s = %s \n",declaration,declaration->class_name().c_str(),SageInterface::get_name(declaration).c_str());
             }
        }

#if 0
     SgSupport* support = isSgSupport(node);
     if (support != NULL)
        {
          bool skipTest = !(isSgTemplateArgument(support));
          if (skipTest == false && support->get_file_info()->isShared() == false)
             {
               printf ("Found a support IR node which is not shared = %p = %s = %s \n",support,support->class_name().c_str(),SageInterface::get_name(support).c_str());
             }
        }
#endif

#if 0
     printf ("ReportUnsharedDeclarationsTraversal::visit(): node = %p = %s \n",node,node->class_name().c_str());
#endif
   }

void
reportUnsharedDeclarationsTraversal()
   {
  // DQ (2/10/2007): Introduce tracking of performance of within AST merge
     TimingPerformance timer ("Report any unshared declarations:");

  // Header for output of report
     printf ("List of IR nodes that are not shared in the merged AST: \n");

     ReportUnsharedDeclarationsTraversal t;
     t.traverseMemoryPool();

     printf ("\nNote: Some IR nodes are never shared this list is: \n");
     printf ("     SgFunctionParameterList \n");
     printf ("     SgNamespaceDeclarationStatement \n");
     printf ("     SgCtorInitializerList \n");
   }



set<SgNode*>
computeSetDifference(const set<SgNode*> & listToDelete, const set<SgNode*> & requiredNodesTest)
   {
     set<SgNode*> setDifferenceResult;

  // printf ("In computeSetDifference(): listToDelete.size() = %" PRIuPTR " requiredNodesTest.size() = %" PRIuPTR " \n",listToDelete.size(),requiredNodesTest.size());

  // Use set difference to compute the list of IR nodes to be deleted 
  // (we might want to do this to compute those to graph as well).
     set<SgNode*>::iterator setDifferenceResultIterator = setDifferenceResult.begin();

  // ostream_iterator<SgNode*> required_insert_iter(cout," ");

  // Build an STL insert iterator using the return value container and an iterator for that container set the the start of the container.
     insert_iterator<set<SgNode*> > required_insert_iter(setDifferenceResult,setDifferenceResultIterator);

  // Call the STL algorithm with the input sets and an insert iterator for the return value (setDifferenceResult).
     set_difference (listToDelete.begin(),listToDelete.end(),requiredNodesTest.begin(),requiredNodesTest.end(),required_insert_iter);

     return setDifferenceResult;
   }

set<SgNode*>
computeSetIntersection(const set<SgNode*> & listToDelete, const set<SgNode*> & requiredNodesTest)
   {
     set<SgNode*> setIntersectionResult;

  // printf ("In computeSetIntersection(): listToDelete.size() = %" PRIuPTR " requiredNodesTest.size() = %" PRIuPTR " \n",listToDelete.size(),requiredNodesTest.size());

  // What is the intersection! Since these nodes could be a problem when they are evaluated (SgClassType get_mangled() required definition
  // but only the SgClassType was removed in the set difference and not the SgClassDeclaration).
     set<SgNode*>::iterator setIntersectionResultIterator = setIntersectionResult.begin();
     insert_iterator<set<SgNode*> > required_insert_iter(setIntersectionResult,setIntersectionResultIterator);
     set_intersection (listToDelete.begin(),listToDelete.end(),requiredNodesTest.begin(),requiredNodesTest.end(),required_insert_iter);

     return setIntersectionResult;
   }

