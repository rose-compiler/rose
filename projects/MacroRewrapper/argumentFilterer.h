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



class ComparisonLinearizationAttribute
   {
     public:
       std::vector<SgNode*> nodes;
       ComparisonLinearizationAttribute();
   };


class ComparisonLinearization : public SgBottomUpProcessing<ComparisonLinearizationAttribute>
   {
     private:
       std::vector<SgNode*> nodes;
       std::vector<SgNode*> nodeToFilter;
       Sg_File_Info* posOfMacroCall;


       ComparisonLinearizationAttribute evaluateSynthesizedAttribute (
           SgNode* astNode,
           SubTreeSynthesizedAttributes synthesizedAttributeList );

//       bool skipNode(SgNode* node);


     public:
       ComparisonLinearization(std::vector<SgNode*> argNodes, Sg_File_Info* argFileInfo);



          std::vector<SgNode*> 
            get_ordered_nodes();

   };







extern bool VERBOSE_MESSAGES_OF_WAVE;

#endif
