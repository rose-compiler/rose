/*
 * 
 *
 *  Created on: 24 Feb, 2011
 *      Author: didem
 */

#ifndef MINT_CONSTANTFOLDING_H
#define MINT_CONSTANTFOLDING_H

#include "rose.h"
#include "DefUseAnalysis.h"


using namespace SageBuilder;
using namespace SageInterface;
using namespace std;

class MintConstantFolding
{

public:
  MintConstantFolding();
  virtual ~MintConstantFolding();
  
  static void constantFoldingOptimization(SgNode* node, bool internalTestingAgainstFrontend=false);

  static void constantFoldingOnArrayIndexExpressions(SgNode* node);
};

#endif 
