//#include "rose.h"

// include file for transformation specification support
// include "specification.h"
// include "globalTraverse.h"

// include "query.h"

// string class used if compiler does not contain a C++ string class
// include <roseString.h>

#include "nodeQuery.h"
#define DEBUG_NODEQUERY 0 
// #include "arrayTransformationSupport.h"

// This is where we specify that types should be traversed
#define TRAVERSE_TYPES 1

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;


namespace NodeQuery{

void
printNodeList ( const Rose_STL_Container<SgNode*> & localList )
   {
  // Supporting function for querySolverGrammarElementFromVariantVector
     int counter = 0;
     printf ("Output node list: \n");
     for (Rose_STL_Container<SgNode*>::const_iterator i = localList.begin(); i != localList.end(); i++)
        {
       // printf ("Adding node to list! \n");
          printf ("   list element #%d = %s \n",counter,(*i)->sage_class_name());
          counter++;
        }
   }



/* BEGIN INTERFACE NAMESPACE NODEQUERY2 */

//! push astNode into nodeList if its variantT type match one of those from targetVariantVector
void
pushNewNode ( NodeQuerySynthesizedAttributeType* nodeList, 
              const VariantVector & targetVariantVector,
              SgNode * astNode )
   {
  // Supporting function for querySolverGrammarElementFromVariantVector

  // Allow input of a NULL pointer but don't add it to the list
     if (astNode != NULL)
        {
          for (vector<VariantT>::const_iterator i = targetVariantVector.begin(); i != targetVariantVector.end(); i++)
             {
            // printf ("Loop over target node vector: node = %s \n",getVariantName(*i).c_str());
               if (astNode->variantT() == *i)
                  {
                  //printf ("Adding node to list! \n");
                    nodeList->push_back(astNode);
                  }
             }
        }
   }

void
mergeList ( NodeQuerySynthesizedAttributeType & nodeList, const Rose_STL_Container<SgNode*> & localList )
   {
  // Supporting function for querySolverGrammarElementFromVariantVector
     unsigned localListSize = localList.size();
     unsigned nodeListSize  = nodeList.size();
     for (Rose_STL_Container<SgNode*>::const_iterator i = localList.begin(); i != localList.end(); i++)
        {
       // printf ("Adding node to list (%s) \n",(*i)->sage_class_name());
          nodeList.push_back(*i);
        }
     ROSE_ASSERT (nodeList.size() == nodeListSize+localListSize);
   }



// DQ (4/7/2004): Added to support more general lookup of data in the AST (vector of variants)
void* querySolverGrammarElementFromVariantVector ( SgNode * astNode, VariantVector targetVariantVector,  NodeQuerySynthesizedAttributeType* returnNodeList )
   {
  // This function extracts type nodes that would not be traversed so that they can
  // accumulated to a list.  The specific nodes collected into the list is controlled
  // by targetVariantVector.

     ROSE_ASSERT (astNode != NULL);

#if 0
     printf ("Inside of void* querySolverGrammarElementFromVariantVector() astNode = %p = %s \n",astNode,astNode->class_name().c_str());
#endif

     Rose_STL_Container<SgNode*> nodesToVisitTraverseOnlyOnce;

     pushNewNode (returnNodeList,targetVariantVector,astNode);

     vector<SgNode*>               succContainer      = astNode->get_traversalSuccessorContainer();
     vector<pair<SgNode*,string> > allNodesInSubtree  = astNode->returnDataMemberPointers();

#if 0
     printf ("succContainer.size()     = %zu \n",succContainer.size());
     printf ("allNodesInSubtree.size() = %zu \n",allNodesInSubtree.size());
#endif

     if ( succContainer.size() != allNodesInSubtree.size() )
        {
          for (vector<pair<SgNode*,string> >::iterator iItr = allNodesInSubtree.begin(); iItr!= allNodesInSubtree.end(); ++iItr )
             {
#if 0
               if ( iItr->first != NULL  )
                    printf ("iItr->first = %p = %s \n",iItr->first,iItr->first->class_name().c_str());
#endif
               SgType* type = isSgType(iItr->first);
               if ( type != NULL  )
                  {
                 // DQ (1/13/2011): If we have not already seen this entry then we have to chase down possible nested types.
                 // if (std::find(succContainer.begin(),succContainer.end(),iItr->first) == succContainer.end() )
                    if (std::find(succContainer.begin(),succContainer.end(),type) == succContainer.end() )
                       {
                      // Are there any other places where nested types can be found...?
                      // if ( isSgPointerType(iItr->first) != NULL  || isSgArrayType(iItr->first) != NULL || isSgReferenceType(iItr->first) != NULL || isSgTypedefType(iItr->first) != NULL || isSgFunctionType(iItr->first) != NULL || isSgModifierType(iItr->first) != NULL)
                      // if (type->containsInternalTypes() == true)
                         if (type->containsInternalTypes() == true)
                            {
#if 0
                              printf ("If we have not already seen this entry then we have to chase down possible nested types. \n");
                           // ROSE_ASSERT(false);
#endif

                              Rose_STL_Container<SgType*> typeVector = type->getInternalTypes();
#if 0
                              printf ("----- typeVector.size() = %zu \n",typeVector.size());
#endif
                              Rose_STL_Container<SgType*>::iterator i = typeVector.begin();
                              while(i != typeVector.end())
                                 {
#if 0
                                   printf ("----- internal type = %s \n",(*i)->class_name().c_str());
#endif
                                // Add this type to the return list of types.
                                   pushNewNode (returnNodeList,targetVariantVector,*i);

                                   i++;
                                 }
                            }

                      // pushNewNode (returnNodeList,targetVariantVector,iItr->first);
                         pushNewNode (returnNodeList,targetVariantVector,type);
                       }
                  }
             }
        }

     return NULL;
   } /* End function querySolverUnionFields() */

NodeQuerySynthesizedAttributeType
querySolverGrammarElementFromVariantVector ( 
   SgNode * astNode, 
   VariantVector targetVariantVector )
   {
  // This function extracts type nodes that would not be traversed so that they can
  // accumulated to a list.  The specific nodes collected into the list is controlled
  // by targetVariantVector.

     ROSE_ASSERT (astNode != NULL);
     NodeQuerySynthesizedAttributeType returnNodeList;

     Rose_STL_Container<SgNode*> nodesToVisitTraverseOnlyOnce;

     pushNewNode (&returnNodeList,targetVariantVector,astNode);

     vector<SgNode*>               succContainer      = astNode->get_traversalSuccessorContainer();
     vector<pair<SgNode*,string> > allNodesInSubtree  = astNode->returnDataMemberPointers();

     if( succContainer.size() != allNodesInSubtree.size() )
     for(vector<pair<SgNode*,string> >::iterator iItr = allNodesInSubtree.begin(); iItr!= allNodesInSubtree.end();
         ++iItr )
       if( isSgType(iItr->first) != NULL  )
         if(std::find(succContainer.begin(),succContainer.end(),iItr->first) == succContainer.end() )
           pushNewNode (&returnNodeList,targetVariantVector,iItr->first);

     
     return returnNodeList;
   } /* End function querySolverUnionFields() */


}//END NAMESPACE NODEQUERY

