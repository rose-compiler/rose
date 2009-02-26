#ifndef ARGUMENT_FILTERER_H
#define ARGUMENT_FILTERER_H

#include "macroRewrapper.h"
#include <AstTraversal.h>

std::vector<SgNode*> queryForAllNodes(SgNode* node);
std::vector<SgNode*> queryForLine(SgNode* node, Sg_File_Info* compareFileInfo);

class NodesAtLineNumber :  public std::binary_function<SgNode*, std::pair< std::vector<SgNode*>*, Sg_File_Info*> , void* >{
  public:
	  result_type operator()(first_argument_type node, const second_argument_type accumulatedList ) const;
};
class ComparisonLinearization : public AstPreOrderTraversal {

std::vector<SgNode*> nodeToFilter;
Sg_File_Info* posOfMacroCall;

std::vector<SgNode*> skipNodeAndSubTree;


std::vector<SgNode*> orderedNodes;
public:

  enum ConstantFolding{
           TraverseOriginalExpressionTree,
           TraverseConstantFoldedExpression,
           TraverseBoth
  };


  private:
    //Variable determining which subtree to traverse
    ConstantFolding constFoldTraverse;

  public:

std::vector<SgNode*> 
get_ordered_nodes();

ComparisonLinearization(std::vector<SgNode*> argNodes, Sg_File_Info* argFileInfo);

void preOrderVisit(SgNode* node);
bool skipNode(SgNode* node);
bool skipSubTreeOfNode(SgNode* node);
};

extern bool VERBOSE_MESSAGES_OF_WAVE;

#endif
