#ifndef IR_NODE_GRAPH_H
#define IR_NODE_GRAPH_H

class IntermediateRepresentationNodeGraph : public ROSE_VisitTraversal
   {
  // DQ (8/21/2013): This class supports the generation of DOT files of specific IR nodes
  // in the AST.  The goal is to support:
  //   1) Different IR nodes kinds
  //   2) AST subtrees
  //   3) arbrtraty sets of IR nodes for inclusion
  //   4) arbitrary sets of IR nodes for exclusion
  // The general goal is to generate a graph of a subset of the AST.

     public:
          std::ofstream & file;
          std::vector<VariantT> nodeKindList;

          std::set<VariantT> include_nodeKindSet;
          std::set<VariantT> exclude_nodeKindSet;

          std::set<SgNode*> include_nodeSet;
          std::set<SgNode*> exclude_nodeSet;

          IntermediateRepresentationNodeGraph(std::ofstream & file, SgProject* project, const std::vector<VariantT> & nodeKindList);

          virtual ~IntermediateRepresentationNodeGraph() {};

      //! Required traversal function
          void visit (SgNode* node);
   };

void intermediateRepresentationNodeGraph(SgProject* project, const std::vector<VariantT> & nodeKindList);

// endif for IR_NODE_GRAPH_H
#endif
