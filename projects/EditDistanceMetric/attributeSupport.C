#include "rose.h"

using namespace std;

// #include "treeIsomorphism.h"
// #include "sequenceGeneration.h"

#include "attributeSupport.h"

// *************************************************
//                IsomorphicNode
// *************************************************

// DOT graph support for attributes to add additional edges to AST dot graphs
// (useful for debugging)
vector<AstAttribute::AttributeEdgeInfo>
IsomorphicNode::additionalEdgeInfo()
   {
     vector<AstAttribute::AttributeEdgeInfo> v;

#if 1
     if (thisNode != NULL && thatNode != NULL)
        {
          AstAttribute::AttributeEdgeInfo edgeToUnmatchedSubtree (thisNode,thatNode,"associated isomorphic subtree"," wieght=10.0 arrowsize=7.0 style=\"setlinewidth(7)\" constraint=false color=deepskyblue ");
          v.push_back(edgeToUnmatchedSubtree);
        }
#endif
     return v;
   }

std::vector<AstAttribute::AttributeNodeInfo>
IsomorphicNode::additionalNodeInfo()
   {
     vector<AstAttribute::AttributeNodeInfo> v;

     return v;
   }

AstAttribute*
IsomorphicNode::copy() const
   {
     return new IsomorphicNode(*this);
   }

// DOT graph support for attributes to color AST IR nodes in AST dot graphs (useful for debugging)
string
IsomorphicNode::additionalNodeOptions()
   {
     return "fillcolor=\"deepskyblue\",style=filled";
   }

// *************************************************
//                NonIsomorphicNode
// *************************************************

// DOT graph support for attributes to add additional edges to AST dot graphs
// (useful for debugging)
vector<AstAttribute::AttributeEdgeInfo>
NonIsomorphicNode::additionalEdgeInfo()
   {
     vector<AstAttribute::AttributeEdgeInfo> v;

#if 1
     if (thisNode != NULL && thatNode != NULL)
        {
          AstAttribute::AttributeEdgeInfo edgeToUnmatchedSubtree (thisNode,thatNode,"associated non-isomorphic subtree"," wieght=10.0 arrowsize=7.0 style=\"setlinewidth(7)\" constraint=false color=orange ");
          v.push_back(edgeToUnmatchedSubtree);
        }
#endif
     return v;
   }

std::vector<AstAttribute::AttributeNodeInfo>
NonIsomorphicNode::additionalNodeInfo()
   {
     vector<AstAttribute::AttributeNodeInfo> v;

     return v;
   }

AstAttribute*
NonIsomorphicNode::copy() const
   {
     return new NonIsomorphicNode(*this);
   }

// DOT graph support for attributes to color AST IR nodes in AST dot graphs (useful for debugging)
string
NonIsomorphicNode::additionalNodeOptions()
   {
  // return "fillcolor=\"deepskyblue\",style=filled";
     return "fillcolor=\"orange\",style=filled";
   }

