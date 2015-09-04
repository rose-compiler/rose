// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "collectAssociateNodes.h"
#include "test_support.h"
using namespace std;

set<SgNode*> getSetOfNonFrontendSpecificNodes();

// set<SgNode*> getSetOfFrontendSpecificNodes( const set<SgNode*> & requiredNodesTest )
ROSE_DLL_API
set<SgNode*>
getSetOfFrontendSpecificNodes()
   {
  // Note that this function builds two lists and uses set difference to 
  // return a third list.  The returned list are the IR nodes that can be 
  // supressed in the visualization of the AST (excluding the front-end IR 
  // nodes which only make the graphs overly complex and the relavant code 
  // much more difficult to visualize).

  // Generate a subset of the SgNode* in the memory pool (all IR nodes)
     class NodeListTraversal : public ROSE_VisitTraversal
        {
          public:
              set<SgNode*> nodeList;

              void visit (SgNode* n )
                 {
                   Sg_File_Info* fileInfo = NULL;
                   fileInfo = n->get_file_info();

                // printf ("In (gather frontend specific nodes) NodeListTraversal::visit(): n = %p = %s nodeList.size() = %ld \n",n,n->class_name().c_str(),nodeList.size());

                   if (fileInfo != NULL)
                      {
                     // printf ("fileInfo->isFrontendSpecific() = %s \n",(fileInfo->isFrontendSpecific() == true) ? "true" : "false");
                     // ROSE_ASSERT(fileInfo->isFrontendSpecific() == false);

                        if (fileInfo->isFrontendSpecific() == true)
                           {
                             nodeList.insert(n);
                             addAssociatedNodes(n,nodeList,true);
                           }
                      }
                     else
                      {
                        fileInfo = isSg_File_Info(n);
                        if (fileInfo != NULL)
                           {
                             if (fileInfo->isFrontendSpecific() == true)
                                  nodeList.insert(n);
                           }
                      }
                 }

              virtual ~NodeListTraversal() {}
        };

     NodeListTraversal t;
     t.traverseMemoryPool();

#if 0
     printf ("At base of getSetOfFrontendSpecificNodes(): t.nodeList = %" PRIuPTR " \n",t.nodeList.size());
     return t.nodeList;
#else
  // printf ("Calling compute set difference() to subtract the required IR nodes (t.nodeList = %" PRIuPTR ") \n",t.nodeList.size());

     set<SgNode*> requiredNodesTest = getSetOfNonFrontendSpecificNodes();

  // printf ("The set of getSetOfNonFrontendSpecificNodes() is: \n");
  // displaySet(requiredNodesTest,"getSetOfNonFrontendSpecificNodes()");

  // Computer the set difference between the nodes to delete and the nodes that are required!
     set<SgNode*> finalList = computeSetDifference(t.nodeList,requiredNodesTest);
  // printf ("Calling compute set difference() to subtract the required IR nodes: DONE \n");

  // printf ("At base of getSetOfFrontendSpecificNodes(): finalList = %" PRIuPTR " \n",finalList.size());
     return finalList;
#endif
   }


set<SgNode*>
getSetOfNonFrontendSpecificNodes()
   {
  // This function computes the set of IR nodes that are not front-end specific, then we will do set difference 
  // (with the list that can be considered front-end specific).  These lists have a non-null intersection because 
  // some IR nodes are shared (types for example) and we want to isolate front-end specific IR nodes that are a part of 
  // the front-end, but NOT shared with parts of the whole AST graph that are separate from the front-end.

  // Generate a subset of the SgNode* in the memory pool (all IR nodes)
     class NodeListTraversal : public ROSE_VisitTraversal
        {
          public:
              set<SgNode*> nodeList;

              void visit (SgNode* n )
                 {
                   Sg_File_Info* fileInfo = NULL;

                // DQ (11/28/2012): Added assertion.
                   ROSE_ASSERT(n != NULL);

                   fileInfo = n->get_file_info();

#if 0
                   printf ("In (gather frontend specific nodes) NodeListTraversal::visit(): n = %p = %s = %s nodeList.size() = %ld \n",n,n->class_name().c_str(),SageInterface::get_name(n).c_str(),nodeList.size());
#endif

                   if (fileInfo != NULL)
                      {
                     // printf ("fileInfo->isFrontendSpecific() = %s \n",(fileInfo->isFrontendSpecific() == true) ? "true" : "false");
                     // ROSE_ASSERT(fileInfo->isFrontendSpecific() == false);

                        if (fileInfo->isFrontendSpecific() == false)
                           {
                             nodeList.insert(n);

                          // DQ (11/28/2012): Added assertion.
                             ROSE_ASSERT(n != NULL);

                             addAssociatedNodes(n,nodeList,true);
                           }
                      }
                     else
                      {
                        fileInfo = isSg_File_Info(n);
                        if (fileInfo != NULL)
                           {
                             if (fileInfo->isFrontendSpecific() == false)
                                  nodeList.insert(n);
                           }
                      }
                 }

              virtual ~NodeListTraversal() {}
        };

     NodeListTraversal t;
     t.traverseMemoryPool();

  // printf ("At base of getSetOfFrontendSpecificNodes(): t.nodeList = %" PRIuPTR " \n",t.nodeList.size());
     return t.nodeList;
   }


