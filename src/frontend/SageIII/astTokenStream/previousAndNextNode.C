// DQ (10/5/2014): This is more strict now that we include rose_config.h in the sage3basic.h.
// #include "rose.h"
#include "sage3basic.h"

#include "frontierDetection.h"

#include "previousAndNextNode.h"

using namespace std;


#if 1
// This is the data structure used to store the previous and next IR node.
// One of these data structors is used for each IR node in a map of all
// the IR nodes in the AST.
PreviousAndNextNodeData::PreviousAndNextNodeData(SgNode* previous, SgNode* next)
   : previous(previous), next(next)
   {
   }
#endif

#if 0
PreviousAndNextNodeTraversal::PreviousAndNextNodeTraversal()
   {
   }

void 
PreviousAndNextNodeTraversal::preOrderVisit(SgNode *astNode)
   {
  // This sets up the next entry for each IR node.
  // first we traverse the entry and put a map element into the map
  // then we look of the map element and fill in the next node.

     static SgNode *previousNode = NULL;

     printf ("In PreviousAndNextNodeTraversal::preOrderVisit(): astNode = %p = %s \n",astNode,astNode != NULL ? astNode->class_name().c_str() : "null");
     printf ("   --- previousNode = %p = %s \n",previousNode,previousNode != NULL ? previousNode->class_name().c_str() : "null");

  // Lookup the previous IR node and fill in it's connection to the current IR node.
     if (previousNode != NULL)
        {
          std::map<SgNode*,PreviousAndNextNodeData*>::iterator i = previousAndNextNodeMap.find(previousNode);
          ROSE_ASSERT(i != previousAndNextNodeMap.end());

          ROSE_ASSERT(i->first != NULL);
          ROSE_ASSERT(i->second != NULL);
          ROSE_ASSERT(astNode != NULL);

       // i->second->previous = astNode;
          i->second->next = astNode;
        }

     PreviousAndNextNodeData* previousAndNextNodeData = new PreviousAndNextNodeData(NULL,NULL);

  // Insert a element into the map for the current IR node being traversed.
     previousAndNextNodeMap.insert(std::pair<SgNode*,PreviousAndNextNodeData*>(astNode,previousAndNextNodeData));

     if (astNode != NULL)
        {
          previousNode = astNode;
        }
       else
        {
          printf ("WARNING: In PreviousAndNextNodeTraversal::preOrderVisit(): astNode == NULL \n");
        }
   }


void 
PreviousAndNextNodeTraversal::postOrderVisit(SgNode *astNode)
   {
     static SgNode *previousNode = NULL;

     printf ("In PreviousAndNextNodeTraversal::postOrderVisit(): astNode = %p = %s \n",astNode,astNode != NULL ? astNode->class_name().c_str() : "null");
     printf ("   --- previousNode = %p = %s \n",previousNode,previousNode != NULL ? previousNode->class_name().c_str() : "null");

#if 1
  // Lookup the previous IR node and fill in it's connection to the current IR node.
     if (previousNode != NULL)
        {
          std::map<SgNode*,PreviousAndNextNodeData*>::iterator i = previousAndNextNodeMap.find(previousNode);
          ROSE_ASSERT(i != previousAndNextNodeMap.end());

          ROSE_ASSERT(astNode != NULL);

          ROSE_ASSERT(i->first != NULL);
          ROSE_ASSERT(i->second != NULL);
       // ROSE_ASSERT(i->second->previous != NULL);

       // ROSE_ASSERT(i->second->next == NULL);
          ROSE_ASSERT(i->second->previous == NULL);
       // i->second->next = astNode;
          i->second->previous = astNode;
       // ROSE_ASSERT(i->second->next != NULL);
          ROSE_ASSERT(i->second->previous != NULL);

          printf ("   --- Looking up PreviousAndNextNodeData: i->first = %p = %s i->second = %p i->second->previous = %p = %s i->second->next = %p = %s \n",
               i->first,i->first->class_name().c_str(),i->second,
               i->second->previous,i->second->previous != NULL ? i->second->previous->class_name().c_str() : "null",
               i->second->next,i->second->next != NULL ? i->second->next->class_name().c_str() : "null");
        }
#endif

     if (astNode != NULL)
        {
          previousNode = astNode;
        }
       else
        {
          printf ("WARNING: In PreviousAndNextNodeTraversal::postOrderVisit(): astNode == NULL \n");
        }
   }


void 
PreviousAndNextNodeTraversal::atTraversalStart()
   {
     printf ("In PreviousAndNextNodeTraversal::atTraversalStart() \n");
   }

void 
PreviousAndNextNodeTraversal::atTraversalEnd()
   {
     printf ("In PreviousAndNextNodeTraversal::atTraversalEnd() \n");
   }




std::map<SgNode*,PreviousAndNextNodeData*>
previousAndNextNodeTraversal ( SgFile* file )
   {
     printf ("In previousAndNextNodeTraversal() \n");

     PreviousAndNextNodeTraversal traversal;

     traversal.traverse(file);

     printf ("Leaving previousAndNextNodeTraversal() \n");

     return traversal.previousAndNextNodeMap;
   }
#endif

#if 1
PreviousAndNextAttribute::PreviousAndNextAttribute(SgNode* from, SgNode* to, std::string name, std::string options)
   : from(from), to(to),name(name), options(options)
   {
   }

PreviousAndNextAttribute::PreviousAndNextAttribute(const PreviousAndNextAttribute & X)
   {
  // Copy constructor.

     from    = X.from;
     to      = X.to;
     name    = X.name;
     options = X.options;
   }


std::string
PreviousAndNextAttribute::additionalNodeOptions()
   {
     string s;
     return s;
   }

std::vector<AstAttribute::AttributeEdgeInfo>
PreviousAndNextAttribute::additionalEdgeInfo()
   {
     std::vector<AstAttribute::AttributeEdgeInfo> edgeList;
     AstAttribute::AttributeEdgeInfo edge (from,to,name,options);
     edgeList.push_back(edge);

     return edgeList;
   }

std::vector<AstAttribute::AttributeNodeInfo>
PreviousAndNextAttribute::additionalNodeInfo()
   {
     std::vector<AstAttribute::AttributeNodeInfo> nodeList;

     return nodeList;
   }


AstAttribute*
PreviousAndNextAttribute::copy()
   {
     return NULL;
   }
#endif

std::map<SgNode*,PreviousAndNextNodeData*>
computePreviousAndNextNodes(SgGlobal* globalScope, std::vector<FrontierNode*> frontierNodes)
   {
  // This is an alternative way to compute the previous/next node map using the token/AST unparsing frontier list directly.

     std::map<SgNode*,PreviousAndNextNodeData*> previousAndNextNodeMap;

     SgStatement* previousNode         = globalScope;
     SgStatement* previousPreviousNode = globalScope;
     for (size_t j = 0; j < frontierNodes.size(); j++)
        {
       // SgStatement* statement = topAttribute.frontierNodes[j];
       // ROSE_ASSERT(statement != NULL);
          FrontierNode* frontierNode = frontierNodes[j];
          ROSE_ASSERT(frontierNode != NULL);
          SgStatement* statement = frontierNode->node;
          ROSE_ASSERT(statement != NULL);
#if 0
          printf (" (%p = %s) ",statement,statement->class_name().c_str());
#endif
          PreviousAndNextNodeData* previousAndNextNodeData = new PreviousAndNextNodeData(previousPreviousNode,statement);
#if 0
          printf ("Building previousAndNextNodeData = %p previousPreviousNode = %p = %s previousNode = %p = %s statement = %p = %s \n",
               previousAndNextNodeData,previousPreviousNode,previousPreviousNode->class_name().c_str(),
               previousNode,previousNode->class_name().c_str(),statement,statement->class_name().c_str());
#endif
       // Insert a element into the map for the current IR node being traversed.
          if (previousNode != NULL)
             {
#if 0
               printf ("Insert previousAndNextNodeData = %p into previousAndNextNodeMap \n",previousAndNextNodeData);
#endif
               previousAndNextNodeMap.insert(std::pair<SgNode*,PreviousAndNextNodeData*>(previousNode,previousAndNextNodeData));
             }
            else
             {
               printf ("WARNING: previousNode == NULL: j = %" PRIuPTR " can't insert entry into previousAndNextNodeMap: statement = %p = %s \n",j,statement,statement->class_name().c_str());
             }

          previousPreviousNode = previousNode;
          previousNode = statement;
        }

  // Handle the last frontier IR node
     if (frontierNodes.empty() == false)
        {
          PreviousAndNextNodeData* previousAndNextNodeData = new PreviousAndNextNodeData(previousPreviousNode,globalScope);

       // Insert a element into the map for the current IR node being traversed.
          previousAndNextNodeMap.insert(std::pair<SgNode*,PreviousAndNextNodeData*>(previousNode,previousAndNextNodeData));
        }

     return previousAndNextNodeMap;
   }

