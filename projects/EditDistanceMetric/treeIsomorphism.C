#include "rose.h"

using namespace std;

#include "treeIsomorphism.h"
#include "sequenceGeneration.h"
#include "attributeSupport.h"


bool
labelsMatch( vector< vector< SgNode* > > childLevelTraversalTraceList )
   {
     bool matchFound = true;
#if 0
     for (int i = 1; i < project->numberOfFiles(); i++)
        {
          ROSE_ASSERT(childLevelTraversalTraceList[0].size() == childLevelTraversalTraceList[i].size());
          for (size_t j = 0; j < childLevelTraversalTraceList[0].size(); j++)
             {
               SgNode* testNode      = childLevelTraversalTraceList[0][j];
               SgNode* referenceNode = childLevelTraversalTraceList[i][j];

            // Test for match on labels (later tests will evaluate for equality over selected relevant data fields).
               if (testNode->variantT() != referenceNode->variantT())
                  {
                    printf ("Label mismatch at j = %zu testNode = %p = %s referenceNode = %p = %s \n",j,testNode,testNode->class_name().c_str(),referenceNode,referenceNode->class_name().c_str());
                    matchFound = false;
                  }
             }
        }
#else
     ROSE_ASSERT(childLevelTraversalTraceList[0].size() == childLevelTraversalTraceList[1].size());
     for (size_t j = 0; j < childLevelTraversalTraceList[0].size(); j++)
        {
          SgNode* testNode      = childLevelTraversalTraceList[0][j];
          SgNode* referenceNode = childLevelTraversalTraceList[1][j];

       // Test for match on labels (later tests will evaluate for equality over selected relevant data fields).
          if (testNode->variantT() != referenceNode->variantT())
             {
               printf ("Label mismatch at j = %zu testNode = %p = %s referenceNode = %p = %s \n",j,testNode,testNode->class_name().c_str(),referenceNode,referenceNode->class_name().c_str());
               matchFound = false;
             }
        }
#endif

     return matchFound;
   }

void
matchLabels (vector< vector< SgNode* > > traversalTraceList )
   {
  // This is higher level function that calls the boolean valued labelsMatch() function.

     if (labelsMatch(traversalTraceList) == false)
        {
       // If the lables don't match then end in an error since for initial debugging I want to assume 
       // that if the number of children match then the labels match and only force the number of 
       // children being different tor trigger a recursive call.

          printf ("The labels on the AST IR nodes did NOT match \n");
          ROSE_ASSERT(false);
        }
       else
        {
          printf ("The labels on the AST IR nodes DID match \n");
        }
   }



void
diff ( SgNode* subtree1, SgNode* subtree2 )
   {
  // This function does a top down matching of the AST to identify the largest parts of the
  // AST structure that are equivalent between two different ASTs.  It then uses the 
  // sequence alignment mechanisms to sort out the parts that are not as clearly isomophic
  // and identify the differences between collections which contain matching subcollections 
  // of ASTs.  It is not clear if this step is helpful, or if the entire problem should be
  // treated as a sequence alignment problem.

     int numberOfNodesInFile_0 = subtree1->numberOfNodesInSubtree();
     int numberOfNodesInFile_1 = subtree2->numberOfNodesInSubtree();

     bool isSameNumberOfNodes = true;
     if (numberOfNodesInFile_0 != numberOfNodesInFile_1)
        {
          printf ("These files have different numbers of IR nodes and thus are NOT isomorphic! \n");
          isSameNumberOfNodes = false;
       // ROSE_ASSERT(false);
        }

     printf ("isSameNumberOfNodes = %s \n",isSameNumberOfNodes ? "true" : "false");
     if (isSameNumberOfNodes == true)
        {
       // The files have the same number of IR nodes so look more deeply for isomorphism
          vector< vector< SgNode* > > traversalTraceList = generateTraversalTraceList (subtree1, subtree2, /* minDepth = 0 */ 0, /* infinite maxDepth */ -1 );

          matchLabels(traversalTraceList);

          AstAttribute* subtreeAttribute_0 = new IsomorphicNode(subtree1,subtree2);
          AstAttribute* subtreeAttribute_1 = new IsomorphicNode(NULL,NULL);

          ROSE_ASSERT (subtree1->attributeExists("IsomorphicNode") == false);
          ROSE_ASSERT (subtree2->attributeExists("IsomorphicNode") == false);

          subtree1->addNewAttribute("IsomorphicNode",subtreeAttribute_0);
          subtree2->addNewAttribute("IsomorphicNode",subtreeAttribute_1);

       // Define the mapping function
          map<SgNode*,SgNode*> mapping;
          for (size_t i = 0; i < traversalTraceList[1].size(); i++)
             {
               mapping[traversalTraceList[1][i]] = traversalTraceList[0][i];
             }

       // Need to finish this implementation, we have to:
       //   1) test each node for the same label (type of IR node)
       //   2) implement and use ROSETTA generate equality tests of relevant (non-pointer) data.
       //   3) check for equivalence of raw data in sections.
        }
       else
        {
       // The files have a different number of IR nodes so where is the difference
          printf ("These two subtrees are not the same, but how are they different! \n");

       // STEP 1: Find the largest AST from the root that is the same between both files.

       // int depthOfProject = project->depthOfSubtree();
          int depthOfSubtree1 = subtree1->depthOfSubtree();
          int depthOfSubtree2 = subtree2->depthOfSubtree();

          int depthOfProject = (depthOfSubtree1 < depthOfSubtree2) ? depthOfSubtree1 : depthOfSubtree2;
          printf ("Depth of shallowest subtree AST = %d depthOfSubtree1 = %d depthOfSubtree2 = %d \n",depthOfProject,depthOfSubtree1,depthOfSubtree2);

          vector< vector< SgNode* > > traversalTraceList;
          int depthOfSameSizeAST = 0;

       // This could be a while loop, but it can support debugging to see the traces at larger depths.
       // while ( depthOfSameSizeAST < depthOfProject && temp_traversalTraceList[0].size() == temp_traversalTraceList[1].size() )
          for (int depth = 0; depth < depthOfProject; depth++)
             {
            // The resulting vector will include the root, so the size with be the number of children plus one.
               vector< vector< SgNode* > > temp_traversalTraceList = generateTraversalTraceList (subtree1, subtree2, /* minDepth = 0 */ 0, depth );

               printf ("depth = %2d traversalTraceList[0].size() = %zu \n",depth,temp_traversalTraceList[0].size());
               printf ("           traversalTraceList[1].size() = %zu \n",temp_traversalTraceList[1].size());

            // If they are the same to a specific depth then save that AST trace and use it for analysis.
               if (temp_traversalTraceList[0].size() == temp_traversalTraceList[1].size())
                  {
                    depthOfSameSizeAST = depth;
                    traversalTraceList = temp_traversalTraceList;
                  }
             }

          printf ("traversalTraceList.size() (number of files) = %zu \n",traversalTraceList.size());
          for (size_t i = 0; i < traversalTraceList.size(); i++)
             {
               printf ("--- depthOfSameSizeAST = %d traversalTraceList[%zu].size() = %zu \n",depthOfSameSizeAST,i,traversalTraceList[i].size());
             }

       // STEP 2: Verify that the labels on the AST which we think is the same make it the same AST.

       // STEP 3: Now get the children at depth where the AST starts to be different.

          vector< vector< SgNode* > > childLevelTraversalTraceList = generateTraversalTraceList (subtree1, subtree2, depthOfSameSizeAST, depthOfSameSizeAST );

          matchLabels(childLevelTraversalTraceList);

       // STEP 4: Find the largest subtree from the children that has the same number of IR nodes (and verify the labels match).

       // STEP 5: Report what subtrees in the ASTs for the two files are different and analyze them seperately.

          if (depthOfSameSizeAST > 0)
             {
               int size = childLevelTraversalTraceList[0].size();
               for (int i = 0; i < size; i++)
                  {
                    SgNode* childSubtree_0 = childLevelTraversalTraceList[0][i];
                    SgNode* childSubtree_1 = childLevelTraversalTraceList[1][i];
                    printf ("Making a recursive call on subtree1 = %p = %s and subtree2 = %p = %s \n",childSubtree_0,childSubtree_0->class_name().c_str(),childSubtree_1,childSubtree_1->class_name().c_str());
                    diff ( childSubtree_0, childSubtree_1 );

                    printf ("Finished a recursive call to diff() \n");
                 // ROSE_ASSERT(false);
                  }
             }
            else
             {
               ROSE_ASSERT(depthOfSameSizeAST == 0);
               SgNode* childSubtree_0 = childLevelTraversalTraceList[0][0];
               SgNode* childSubtree_1 = childLevelTraversalTraceList[1][0];
               printf ("No more progress possible in these subtrees: subtree1 = %p = %s and subtree2 = %p = %s \n",childSubtree_0,childSubtree_0->class_name().c_str(),childSubtree_1,childSubtree_1->class_name().c_str());

               AstAttribute* subtreeAttribute_0 = new NonIsomorphicNode(childSubtree_0,childSubtree_1);
               AstAttribute* subtreeAttribute_1 = new NonIsomorphicNode(NULL,NULL);

               ROSE_ASSERT (childSubtree_0->attributeExists("NonIsomorphicNode") == false);

               childSubtree_0->addNewAttribute("NonIsomorphicNode",subtreeAttribute_0);
               childSubtree_1->addNewAttribute("NonIsomorphicNode",subtreeAttribute_1);

            // Now search the children and match them up
               vector< vector< SgNode* > > nonIsomorphicTraversalTraceList = generateTraversalTraceList (subtree1, subtree2, depthOfSameSizeAST+1, depthOfSameSizeAST+1 );
               int subTree_0_size = nonIsomorphicTraversalTraceList[0].size();
               int subTree_1_size = nonIsomorphicTraversalTraceList[1].size();

               printf ("subTree_0_size = %d subTree_1_size = %d \n",subTree_0_size,subTree_1_size);

               SgNode* smallerSubtree = (subTree_0_size < subTree_1_size) ? childSubtree_0 : childSubtree_1;
               SgNode* largerSubtree  = (subTree_0_size < subTree_1_size) ? childSubtree_1 : childSubtree_0;

               printf ("smallerSubtree = %p = %s largerSubtree = %p = %s \n",smallerSubtree,smallerSubtree->class_name().c_str(),largerSubtree,largerSubtree->class_name().c_str());
               ROSE_ASSERT(smallerSubtree->variantT() == largerSubtree->variantT());

               printf ("Calling sequenceAlignment()... \n");
               sequenceAlignment ( nonIsomorphicTraversalTraceList );
               printf ("Done with call to sequenceAlignment() \n");
             }
        }
   }




void
outputAST( SgProject* project )
   {
#if 0
  // This is not well tested yet! Fails in: bool SgTreeTraversal<InheritedAttributeType, SynthesizedAttributeType>::inFileToTraverse(SgNode*)
     printf ("Generate the pdf output of the binary AST \n");
     generatePDF ( *project );
#endif

#if 1
     printf ("Generate the dot output of the SAGE III AST \n");
  // generateDOT ( *project );
     generateDOTforMultipleFile ( *project );
     printf ("DONE: Generate the dot output of the SAGE III AST \n");
#endif

#if 0
  // DQ (6/14/2007): Added support for simpler function for generation of graph of whole AST.
     const int MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH = 2000;
     printf ("Generate whole AST graph if small enough \n");
     generateAstGraph(project,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH);
     printf ("DONE: Generate whole AST graph if small enough \n");
#endif
   }


