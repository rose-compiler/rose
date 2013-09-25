#include "rose.h"

using namespace std;

#include "sequenceGeneration.h"

SequenceGenerationInheritedAttribute
SequenceGenerationTraversal::evaluateInheritedAttribute ( SgNode* astNode, SequenceGenerationInheritedAttribute inheritedAttribute )
   {
#if 0
     printf ("In evaluateInheritedAttribute(): astNode->class_name() = %p = %s depth = %d \n",astNode,astNode->class_name().c_str(),inheritedAttribute.treeDepth);
#endif

  // Make sure this is not a project
     if (isSgProject(astNode) != NULL)
        {
       // Everything is part of a project so comparing two projects does not currently make sense.
           printf ("Error: can compare two projects ... \n");
           ROSE_ASSERT(false);
        }

     SgFile* file = isSgFile(astNode);
     if (file != NULL)
        {
          inheritedAttribute.file = file;
       // printf ("Setting the file pointer to %p \n",file);
        }

  // Only build the trace of the AST to a predefined depth of the AST.
  // if (maxTreeDepth == -1 || inheritedAttribute.treeDepth < maxTreeDepth)
     bool defaultValues = (minTreeDepth == 0 && maxTreeDepth == -1);
     bool trace = defaultValues || (inheritedAttribute.treeDepth >= minTreeDepth && inheritedAttribute.treeDepth <= maxTreeDepth);

  // SgTreeTraversal_inFileToTraverse(SgNode* node, bool traversalConstraint, SgFile* fileToVisit)
  // bool inFile = (inheritedAttribute.file != NULL && SgTreeTraversal_inFileToTraverse(astNode, /* traversalConstraint = true */ true, inheritedAttribute.file) == true);
  // bool inFile = inheritedAttribute.file != NULL && astNode->get_file_info()->isSameFile(inheritedAttribute.file);

  // For binary files the IR nodes don't have a file info (by design)
     bool inFile = true;
     if (astNode->get_file_info() != NULL)
        {
       // Need to test for inheritedAttribute.file != NULL since we can't pass a NULL pointer to Sg_File_Info::isSameFile()
          if (inheritedAttribute.file != NULL && astNode->get_file_info() != NULL && astNode->get_file_info()->isSameFile(inheritedAttribute.file) )
             {
               inFile = true;
             }
            else
             {
            // inFile = false;
               if (inheritedAttribute.file == NULL)
                  {
                 // This is traversal has been started at a point below the SgFile then 
                 // inheritedAttribute.file == NULL and everything that can be traversed 
                 // is considered to be included.
                    inFile = true;
                  }
                 else
                  {
                    inFile = false;
                  }
             }
        }

  // printf ("trace = %s inFile = %s \n",trace ? "true" : "false", inFile ? "true" : "false");

     if (trace == true && inFile == true)
        {
#if 0
          printf ("Saving astNode = %p = %s at depth = %d file = %s \n",astNode,astNode->class_name().c_str(),
               inheritedAttribute.treeDepth,inheritedAttribute.file->getFileName().c_str());
#endif
#if 0
          printf ("Saving astNode = %p = %s at depth = %d \n",astNode,astNode->class_name().c_str(),inheritedAttribute.treeDepth);
#endif
          traversalTrace.push_back(astNode);

       // Record the depth of the IR nodes in the AST
          nodeDepth[astNode] = inheritedAttribute.treeDepth;
          nodeDepth.insert(pair<SgNode*,int>(astNode,inheritedAttribute.treeDepth));

          ROSE_ASSERT(nodeDepth.empty() == false);
        }

     return SequenceGenerationInheritedAttribute(inheritedAttribute.treeDepth + 1,inheritedAttribute.file);
   }

SequenceGenerationSynthesizedAttribute
SequenceGenerationTraversal::evaluateSynthesizedAttribute (
     SgNode* astNode,
     SequenceGenerationInheritedAttribute inheritedAttribute,
     SynthesizedAttributesList childAttributes )
   {
     SequenceGenerationSynthesizedAttribute localResult;

     return localResult;
   }

void
generateTraversalListSupport ( SgNode* node, vector< SgNode* > & traversalTrace, map< SgNode*,int > & nodeOrder, map< SgNode*,int > & nodeDepth, int minDepth , int maxDepth )
   {
  // This function supports three function:
  //   1) generateTraversalList()
  //   2) generateTraversalOrderList()
  //   3) generateTraversalDepthList()

  // Build the inherited attribute
  // Define depth == 1 to be the root of the AST (starting at the file, the SgProject is depth 0)
     SequenceGenerationInheritedAttribute inheritedAttribute(0,NULL);

  // Define the traversal, define the max depth to trace the traversal
     SequenceGenerationTraversal myTraversal(minDepth,maxDepth);

     printf ("Before traversal: minDepth = %d maxDepth = %d default values = %s \n",minDepth,maxDepth,(myTraversal.minTreeDepth == 0 && myTraversal.maxTreeDepth == -1) ? "true" : "false");

  // Call the traversal starting at the project (root) node of the AST
  // myTraversal.traverseInputFiles(project,inheritedAttribute);
     SequenceGenerationSynthesizedAttribute result = myTraversal.traverse(node,inheritedAttribute);

     printf ("After traversal: minDepth = %d maxDepth = %d default values = %s \n",minDepth,maxDepth,(myTraversal.minTreeDepth == 0 && myTraversal.maxTreeDepth == -1) ? "true" : "false");

  // Mapping of order of traversal to IR nodes traversed
     traversalTrace = myTraversal.traversalTrace;

  // Post-process to generate mapping of IR nodes to order of traversal
     vector< SgNode* >::iterator i = traversalTrace.begin();
     int counter = 0;
     while (i != traversalTrace.end())
        {
       // Mapping of IR nodes to order of traversal
          nodeOrder[*i] = counter;
          counter++;
          i++;
        }

     nodeDepth = myTraversal.nodeDepth;

#if 0
     vector< SgNode* >::iterator i = traversalTrace.begin();
     while (i != traversalTrace.end())
        {
          printf ("trace: *i = %p = %s \n",*i,(*i)->class_name().c_str());

          i++;
        }
#endif
   }




vector< SgNode* >
generateTraversalList ( SgNode* node, int minDepth , int maxDepth )
   {
  // Traversal that generates the tree order vector (of IR nodes)

#if 0
     vector< SgNode* > traversalTrace;

  // Build the inherited attribute
  // Define depth == 1 to be the root of the AST (starting at the file, the SgProject is depth 0)
     SequenceGenerationInheritedAttribute inheritedAttribute(0,NULL);

  // Define the traversal, define the max depth to trace the traversal
     SequenceGenerationTraversal myTraversal(minDepth,maxDepth);

     printf ("Before traversal: minDepth = %d maxDepth = %d default values = %s \n",minDepth,maxDepth,(myTraversal.minTreeDepth == 0 && myTraversal.maxTreeDepth == -1) ? "true" : "false");

  // Call the traversal starting at the project (root) node of the AST
  // myTraversal.traverseInputFiles(project,inheritedAttribute);
     SequenceGenerationSynthesizedAttribute result = myTraversal.traverse(node,inheritedAttribute);

     printf ("After traversal: minDepth = %d maxDepth = %d default values = %s \n",minDepth,maxDepth,(myTraversal.minTreeDepth == 0 && myTraversal.maxTreeDepth == -1) ? "true" : "false");

  // Mapping of order of traversal to IR nodes traversed
     traversalTrace = myTraversal.traversalTrace;

  // Post-process to generate mapping of IR nodes to order of traversal
     vector< SgNode* >::iterator i = traversalTrace.begin();
     int counter = 0;
     while (i != traversalTrace.end())
        {
       // Mapping of IR nodes to order of traversal
          nodeOrder[*i] = counter;
          counter++;
          i++;
        }
#else
     vector< SgNode* >  traversalTrace;
     map< SgNode*,int > nodeOrder;
     map< SgNode*,int > nodeDepth;
     generateTraversalListSupport ( node, traversalTrace, nodeOrder, nodeDepth, minDepth , maxDepth );
#endif

#if 0
     vector< SgNode* >::iterator i = traversalTrace.begin();
     while (i != traversalTrace.end())
        {
          printf ("trace: *i = %p = %s \n",*i,(*i)->class_name().c_str());

          i++;
        }
#endif

     return traversalTrace;
   }

map< SgNode*,int >
generateTraversalOrderList ( SgNode* node, int minDepth , int maxDepth )
   {
  // Traversal that generates the tree order map of IR nodes to integer order

     vector< SgNode* > traversalTrace;
     map< SgNode*,int > nodeOrder;
     map< SgNode*,int > nodeDepth;

     generateTraversalListSupport ( node, traversalTrace, nodeOrder, nodeDepth, minDepth , maxDepth );

     return nodeOrder;
   }

map< SgNode*,int >
generateTraversalDepthList ( SgNode* node, int minDepth , int maxDepth )
   {
  // Traversal that generates the tree order map of depts of each IR node

     vector< SgNode* > traversalTrace;
     map< SgNode*,int > nodeOrder;
     map< SgNode*,int > nodeDepth;

     generateTraversalListSupport ( node, traversalTrace, nodeOrder, nodeDepth, minDepth , maxDepth );

     return nodeDepth;
   }



vector< vector< SgNode* > >
generateTraversalTraceList ( SgNode* subtree1, SgNode* subtree2, int minDepth , int maxDepth )
   {
     vector< vector< SgNode* > > temp_traversalTraceList;
#if 0
     for (int i = 0; i < project->numberOfFiles(); i++)
        {
          vector< SgNode* > traversalTrace;

          traversalTrace = generateTraversalList (project->get_fileList()[i], minDepth, maxDepth );

          temp_traversalTraceList.push_back(traversalTrace);

          printf ("traversalTrace.size() = %zu \n",traversalTrace.size());
        }
#else
     vector< SgNode* > traversalTrace_1 = generateTraversalList (subtree1, minDepth, maxDepth );
     vector< SgNode* > traversalTrace_2 = generateTraversalList (subtree2, minDepth, maxDepth );

     temp_traversalTraceList.push_back(traversalTrace_1);
     temp_traversalTraceList.push_back(traversalTrace_2);
#endif

     return temp_traversalTraceList;
   }

