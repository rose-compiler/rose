/*
 * 
 *
 *  Created on: Jul 4, 2010
 *      Author: didem
 */

#ifndef STENCILANALYSIS_H
#define STENCILANALYSIS_H

#include "rose.h"
#include "DefUseAnalysis.h"

#include "../../types/MintTypes.h"
#include <map>
#include <set>
#include <algorithm>

#include <string>

using namespace SageBuilder;
using namespace SageInterface;
using namespace std;

class StencilAnalysis
{

public:
  StencilAnalysis();
  virtual ~StencilAnalysis();

  static void computeAccessFreq(MintInitNameMapExpList_t& arrRefList, int,
				MintArrFreqPairList_t& candidateVarsShared,
				MintArrFreqPairList_t& candidateVarsReg
				);

  static int performHigherOrderAnalysis(SgBasicBlock* basicBlock, 
					const SgInitializedName* array, int num_planes);
  
  static void performCornerStencilsAnalysis(SgBasicBlock* basicBlock, 
						   const SgInitializedName* array,
						   bool& plane_xy /*false*/, 
						   bool& plane_xz /*false*/, 
						   bool& plane_yz /*false*/ );
  
  static bool isShareableReference(const std::vector<SgExpression*> subscripts, 
					 const bool corner_yz //=FALSE
					 ); 

  static int howManyPlanesInSharedMemory(const MintForClauses_t& clauseList, const SgType* type);

private:
  static int getSharingCategory(const std::vector<SgExpression*> subscripts);

  static void selectCandidates(MintArrFreqPairList_t& candidateVarsShared,
			       std::vector<SgInitializedName*> arrayList, 
			       int* planes1, int* planes3 ,
			       int* indexList1, int* indexList3, int num_planes);


    

};

#endif 
