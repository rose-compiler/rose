#include <rose.h>

using namespace std;

#include <treeDiff.h>

TreeDiffInheritedAttribute
TreeDiffTraversal::evaluateInheritedAttribute ( SgNode* astNode, TreeDiffInheritedAttribute inheritedAttribute )
   {
#if 0
     printf ("inheritedAttribute.astTrace.size() = %zu astNode->class_name() = %s \n",inheritedAttribute.astTrace.size(),astNode->class_name().c_str());
#endif

     traversalTrace.push_back(astNode);

     return inheritedAttribute;
   }

TreeDiffSynthesizedAttribute
TreeDiffTraversal::evaluateSynthesizedAttribute ( SgNode* astNode, TreeDiffInheritedAttribute inheritedAttribute, SynthesizedAttributesList childAttributes )
   {
     TreeDiffSynthesizedAttribute localResult;

     return localResult;
   }

void
inorderTraversal ( SgFile* file, vector< SgNode* > & traversalTrace )
   {
  // Traversal that generates the tree order vector

  // Build the inherited attribute
     TreeDiffInheritedAttribute inheritedAttribute;

  // Define the traversal
     TreeDiffTraversal myTraversal;

  // Call the traversal starting at the project (root) node of the AST
  // myTraversal.traverseInputFiles(project,inheritedAttribute);
     TreeDiffSynthesizedAttribute result = myTraversal.traverse(file,inheritedAttribute);

     traversalTrace = myTraversal.traversalTrace;

     vector< SgNode* >::iterator i = traversalTrace.begin();
     while (i != traversalTrace.end())
        {
          printf ("trace: *i = %p = %s \n",*i,(*i)->class_name().c_str());

          i++;
        }
   }


void
treeDifferenceTraversal ( vector< vector< SgNode* > > & traversalTraceList )
   {
     int smallerTreeSize = (int) traversalTraceList[0].size() < traversalTraceList[1].size() ? traversalTraceList[0].size() : traversalTraceList[1].size();
     for (int i = 0; i < smallerTreeSize; i++)
        {
          int numberOfChildren_0 = traversalTraceList[0][i]->numberOfNodesInSubtree();
          int numberOfChildren_1 = traversalTraceList[1][i]->numberOfNodesInSubtree();
        }
      
   }
