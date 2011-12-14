/*
 * CudaOptimizer.h
 *
 *  Created on: Jul 4, 2010
 *      Author: didem
 */

#ifndef ONCHIPMEMORYOPT_H_
#define ONCHIPMEMORYOPT_H_

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

class OnChipMemoryOpt
{

public:
  OnChipMemoryOpt();
  virtual ~OnChipMemoryOpt();
  
  static SgInitializedName* registerOptimizer(
					      const std::set<SgInitializedName*>& readOnlyVars,
					      SgFunctionDeclaration* kernel, 
					      SgInitializedName* candidateVar,
					      SgScopeStatement* varScope= NULL,
					      bool loadEvenCountZero=false, 
					      int updown = 0 );
  
  static void sharedMemoryOptimizer(SgFunctionDeclaration* kernel,
				    const std::set<SgInitializedName*>& readOnlyVars,
				    SgInitializedName* candidateVar,
				    const MintForClauses_t& clauseList, 
				    int num_planes, bool first=true, int common_order=1) ;
  
  
  static void insertSynchPoints(SgFunctionDeclaration* kernel);

 private:


	static int replaceGlobalMemRefWithRegister(SgBasicBlock* kernel, 
						    SgInitializedName* arr,
						   SgInitializedName* reg, int updown );


        static void findAllShareableReferences(const SgFunctionDefinition* kernel, 
					       const SgInitializedName* candidateVar, 
					       vector<SgExpression*>& shareableRefList,						 
					       set<SgExpression*>& ghostCells, const bool corner_yz);
						 

	static void replaceGlobalMemRefWithSharedMem(SgFunctionDefinition* kernel, 
						     const SgInitializedName* var,
						     SgInitializedName* sharedRef,
						     const vector<SgExpression*> shareableRefList, bool corner_yz, 
						     const bool unrollWithoutRegisters=false);

	static void removeUnnecessarySynchPoints(SgFunctionDeclaration* kernel,                                        
						 const SgInitializedName* sh_block)  ;

	static SgPntrArrRefExp* loadAPlaneIntoSharedMemory(SgFunctionDefinition* kernel, 
					       SgInitializedName* candidateVar, 
					       SgInitializedName* sh_block, 
					       SgScopeStatement* varScope,
					       int updown=0, // can be -1 0  1
					       bool unrollWithoutRegisters= false);

	static SgExpression* convertGlobalMemoryIndexIntoLocalIndex(SgExpression* exp, string index, SgBasicBlock* kernel);

	static void loadCenterIntoSharedMemory(SgFunctionDefinition* kernel, 
					       SgInitializedName* candidateVar, 
					       SgInitializedName* sh_block,bool);

	static bool isIndexPlusMinusConstantOp(SgExpression* exp, string index);

	static bool isShareableReference(const std::vector<SgExpression*> subscripts, 
					 const bool corner_yz = false );

	static bool isRegisterReplaceable(std::vector<SgExpression*> subscripts, 
					  int updown);

};

#endif 
