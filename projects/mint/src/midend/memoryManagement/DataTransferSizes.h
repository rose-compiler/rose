/*!
 * Cuda Memory Transfer Size Functions
 *
 *  
 *  
 * 
 */

#include "ASTtools.hh"

#include <string>
#include <map>
#include <vector>

#include "../../types/MintTypes.h"

using namespace std;
using namespace SageBuilder;
using namespace SageInterface;


class DataTransferSizes
{

 public:
  
  static void findTransferSizes(SgNode* node, 
				MintSymSizesMap_t& trfSizes);

  static void printTransferSizes(MintSymSizesMap_t& trfSizes);

 private:
  static SgBasicBlock*  nestedForLoops(SgNode* for_loop);

  static SgBasicBlock* processLoops(SgForStatement* cur_loop, SgBasicBlock* outerBlock, int loopNo);

  
  static void forLoopIndexRanges(SgNode*for_loops, 
				 MintInitNameExpMap_t& index_ranges); 

  static SgExpression* getFirstIndexExpressionInArray(SgNode* arrayNode, string& indexExpStr);

  static SgVarRefExp* getArrayRef(SgNode* arrayNode);

  static void addTransferSizeToThisSymbol(string sizeExpStr, SgVarRefExp* arrayRefExp, 
					  MintSymSizesMap_t& trfSizes);

  static string getSizeExpressionFromIndexExpression(string indexExpStr, SgExpression* indexExp,
						     MintInitNameExpMap_t& index_ranges);
};


