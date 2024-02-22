// Author: Markus Schordan
// $Id: DOTGeneration.C,v 1.3 2008/01/08 02:56:19 dquinlan Exp $

#ifndef DOTGENERATION_C
#define DOTGENERATION_C

// DQ (3/6/2003): added from AstProcessing.h to avoid referencing
// the traversal classes in AstFixes.h before they are defined.
//#include "sage3.h"
#include "roseInternal.h"

#include "DOTGeneration.h"

template<class Node>
DOTGeneration<Node>::DOTGeneration() {
  init();
}

template<class Node>
void
DOTGeneration<Node>::init() {
  tdTrace=0;
  buTrace=0;
  tdbuTrace=0;
  dotrep.clear();
  visitedNodes.clear();
}

template<class Node>
void
DOTGeneration<Node>::generate(std::string filename, Node node, traversalType tt) {
  init();
  traversal=tt;
  DOTInheritedAttribute ia;
  traverse(node,ia);
  dotrep.writeToFileAsGraph(filename+".dot");
}

template<class Node>
DOTInheritedAttribute
DOTGeneration<Node>::evaluateInheritedAttribute(Node node, DOTInheritedAttribute ia)
   {
     visitedNodes.insert(node);
     ia.tdbuTracePos=tdbuTrace++;
     ia.tdTracePos=tdTrace++;
     return ia;
   }

template<class Node>
DOTSynthesizedAttribute
DOTGeneration<Node>::evaluateSynthesizedAttribute(Node node, DOTInheritedAttribute ia, SubTreeSynthesizedAttributes l)
   {
     SubTreeSynthesizedAttributes::iterator iter;
     ROSE_ASSERT(node);
     std::string nodeoption;
     std::string nodelabel = std::string("\\n") + typeid(*node).name();
     nodelabel += additionalNodeInfo(node);
  // DQ (11/1/2003) added mechanism to add options (to add color, etc.)
     nodeoption += additionalNodeOptions(node);
     switch(traversal)
        {
          case TOPDOWNBOTTOMUP:
               dotrep.addNode(node,dotrep.traceFormat(ia.tdbuTracePos,tdbuTrace)+nodelabel,nodeoption);
               break;
          case PREORDER:
          case TOPDOWN:
               dotrep.addNode(node,dotrep.traceFormat(ia.tdTracePos)+nodelabel,nodeoption);
               break;
          case POSTORDER:
          case BOTTOMUP:
               dotrep.addNode(node,dotrep.traceFormat(buTrace)+nodelabel,nodeoption);
               break;
          default:
               assert(false);
        }
  
     ++tdbuTrace;
     ++buTrace;
     DOTSynthesizedAttribute d(0);

  // add edges or null values
     for(iter=l.begin();iter!=l.end();iter++)
        {
       // DQ (3/5/2007): Added addional function to add more edge information to the label and option string
          std::string edgelabel  = "";
          std::string edgeoption = "";

       // Use the label to identify the edge (in general, even though in this case the label is "")
          edgeoption += additionalEdgeOptions(node,iter->node,edgelabel);
          edgelabel  += additionalEdgeInfo   (node,iter->node,edgelabel);

          if ( (*iter).node == NULL )
             {
               dotrep.addNullValue(node,"",edgelabel,"");
             }
            else
             {
               switch(traversal)
                  {
                    case TOPDOWNBOTTOMUP:
                         dotrep.addEdge(node,edgelabel,(*iter).node,edgeoption + "dir=both");
                         break;
                    case PREORDER:
                    case TOPDOWN:
                         dotrep.addEdge(node,edgelabel,(*iter).node,edgeoption + "dir=forward");
                         break;
                    case POSTORDER:
                    case BOTTOMUP:
                         dotrep.addEdge(node,edgelabel,(*iter).node,edgeoption + "dir=back");
                         break;
                    default:
                         assert(false);
                  }
             }
        }

     d.node=node;
     ROSE_ASSERT(node!=0);
     return d;
   }

template<class Node>
std::string
DOTGeneration<Node>::additionalNodeInfo(Node)
   {
     return "";
   }

template<class Node>
std::string
DOTGeneration<Node>::additionalNodeOptions(Node)
   {
     return "";
   }

// DQ (3/5/2007): Added to support edge options (e.g. colors)
template<class Node>
std::string
DOTGeneration<Node>::additionalEdgeInfo(Node, Node, std::string)
   {
     return "";
   }

// DQ (3/5/2007): Added to support edge options (e.g. colors)
template<class Node>
std::string
DOTGeneration<Node>::additionalEdgeOptions(Node, Node, std::string)
   {
     return "";
   }

#endif
