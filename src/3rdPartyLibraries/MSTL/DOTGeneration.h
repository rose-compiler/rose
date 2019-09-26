// Author: Markus Schordan
// $Id: DOTGeneration.h,v 1.5 2008/01/08 02:56:19 dquinlan Exp $

#ifndef DOTGENERATION_H
#define DOTGENERATION_H

#include <set>

#include "AstProcessing.h"
#include "DOTRepresentation.h"

class DOTInheritedAttribute : public SgInheritedAttribute 
   {
     public:
       // DQ (5/3/2006): Used to filter out compiler generated IR nodes 
       // from "rose_edg_required_macros_and_functions.h"
       // These are specific gnu compatability details.
          bool skipSubTree;

       // DQ (12/20/2018): Skip template instantiations that can make the DOT files too large to generate or look at easily.
          bool skipTemplateInstantiations;

          DOTInheritedAttribute() : skipSubTree(false), skipTemplateInstantiations(false), tdTracePos(0), tdbuTracePos(0) {}
          DOTRepresentation<SgNode*>::TraceType tdTracePos; // tdTrace value at node
          DOTRepresentation<SgNode*>::TraceType tdbuTracePos; // tdbuTrace value at node
   };



class DOTSynthesizedAttribute : public SgSynthesizedAttribute {
 public:
  SgNode* node;
  std::string dotstring; // deprecated
  DOTSynthesizedAttribute():node(0),defaultVal(true) { dotstring="";};
  ~DOTSynthesizedAttribute() {};
  DOTSynthesizedAttribute(SgNode* n):node(n),defaultVal(false) { dotstring=""; };
  bool isDefault() {return defaultVal;}
 private:
  bool defaultVal;
};

template<class Node>
class DOTGeneration : public SgTopDownBottomUpProcessing<DOTInheritedAttribute, DOTSynthesizedAttribute >
   {
     public:
       // use this enums as arguments when generating DOT output to specify
       // the traversal type.
          enum traversalType {PREORDER, POSTORDER, TOPDOWN ,BOTTOMUP ,TOPDOWNBOTTOMUP};
          DOTGeneration();
       // generates a visualization of an AST subtree with root node node. The filename must be specified.
       // DQ (10/30/2003) Make generate a virtual function
       // MS 10/22/2015: this function is never overwritten, but overloaded in subclasses. Let's keep it non-virtual for that reason.
          void generate(std::string filename, Node node, traversalType tt=TOPDOWNBOTTOMUP);
          virtual DOTInheritedAttribute evaluateInheritedAttribute(Node node, DOTInheritedAttribute ia);
          virtual DOTSynthesizedAttribute evaluateSynthesizedAttribute(Node node, DOTInheritedAttribute ia, SubTreeSynthesizedAttributes l);

       // this virtual function allows to customize the dot output for each node by overriding it.
          virtual std::string additionalNodeInfo(Node node);

       // DQ (11/1/2003) added mechanism to add options (to add color, etc.)
          virtual std::string additionalNodeOptions(Node node);

       // DQ (3/5/2007) added mechanism to add options and info (to add color, etc.)
          virtual std::string additionalEdgeOptions (Node from, Node to, std::string label);
          virtual std::string additionalEdgeInfo    (Node from, Node to, std::string label);

          void init();
          traversalType traversal;
          DOTRepresentation<Node> dotrep;
          typename DOTRepresentation<Node>::TraceType tdTrace;
          typename DOTRepresentation<Node>::TraceType buTrace;
          typename DOTRepresentation<Node>::TraceType tdbuTrace;

     protected:
          std::string filename;
          std::string filenamePostfix;
          std::set<Node> visitedNodes;

     private:
   };

// Since we don't include rose_config.h we can use these
// #ifdef HAVE_EXPLICIT_TEMPLATE_INSTANTIATION 
   #include "DOTGenerationImpl.h" 
// #endif 

#endif
