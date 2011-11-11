/*
 * CudaOptimizer.h
 *
 *  Created on: Jul 4, 2010
 *      Author: didem
 */

#ifndef CUDAOPTIMIZER_H_
#define CUDAOPTIMIZER_H_

#include "rose.h"
#include "DefUseAnalysis.h"
#include "../types/MintTypes.h"
#include <map>
#include <set>
#include <algorithm>


//#include "KernelMergeInterface.h"
using namespace SageBuilder;
using namespace SageInterface;


class CudaOptimizer
{

public:
	CudaOptimizer();
	virtual ~CudaOptimizer();

	static void optimize(SgFunctionDeclaration* kernel, MintForClauses_t);

	static void optimize(SgFunctionDeclaration* kernel);

	//optimizes all the cuda kernels in a file
	static void optimize(SgSourceFile* file);
	static void swapLoopAndIf(SgFunctionDeclaration* kernel, MintForClauses_t);

 private:


  static void applyRegisterOpt(SgFunctionDeclaration* kernel,
			       std::set<SgInitializedName*> readOnlyVars,
			       const MintArrFreqPairList_t& candidateVarsShared,
			       const MintArrFreqPairList_t& candidateVarsReg,
			       const bool optUnrollZ) ;


  static void findCandidateVarForSharedMem(MintInitNameMapExpList_t arrRefList,
					   SgInitializedName* &candidateVar, 
					   SgInitializedName* prevCandidate, 
					   int &countMaxStencilRef,int &countNonStencilRef );

  static void applyRegisterOpt(SgFunctionDeclaration* kernel,
			       DefUseAnalysis* defuse, 
			       std::set<SgInitializedName*> readOnlyVars,
			       MintInitNameMapExpList_t arrRefList,
			       SgInitializedName* candidateVar4Shared, 
			       SgInitializedName* sec_candidateVar4Shared, 
			       bool optUnrollZ) ;

  static void applySharedMemoryOpt(SgFunctionDeclaration* kernel,
				   const std::set<SgInitializedName*> readOnlyVars,
				   const MintArrFreqPairList_t& candidateVarsShared,
				   const MintForClauses_t& clauseList);
    
  /*
  static void sharedMemoryOptimizer(SgFunctionDeclaration* kernel, 
				    std::set<SgInitializedName*>& readOnlyVars,
				    SgInitializedName* candidateVar, 
				    MintForClauses_t clauseList);
  */
  //static SgStatement* getStatementForLastRef(SgBasicBlock* body, SgInitializedName* iname);
  
  static int getNonStencilArrayRefCount(std::vector<SgExpression*> expList);
  
  
  static void slidingRowOptimizer(SgFunctionDeclaration* kernel, 
				  const std::set<SgInitializedName*>& readOnlyVars, 
				  SgInitializedName* candidateVar, MintForClauses_t clauseList,int num_planes, 
				  int order, bool first=true);//first this is first shared memory var


  static void getArrayReferenceCounts(SgBasicBlock* kernel_body, 
				      std::map<SgInitializedName*, int>& varCount_map);


  static void applyLoopAggregationOpt(SgFunctionDeclaration* kernel,				     
				      const std::set<SgInitializedName*> readOnlyVars,
				      const MintArrFreqPairList_t& candidateVarsShared,
				      const MintArrFreqPairList_t& candidateVarsReg,
				      const MintForClauses_t& clauseList);
    

};

#endif /* CUDAOPTIMIZER_H_ */
