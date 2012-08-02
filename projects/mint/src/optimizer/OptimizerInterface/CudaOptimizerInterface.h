/*
 * CudaOptimizer.h
 *
 *  Created on: Jul 4, 2010
 *      Author: didem
 */

#ifndef OPTIMIZERINTERFACE_H_
#define OPTIMIZERINTERFACE_H_

#include "rose.h"
#include "DefUseAnalysis.h"

#include <map>
#include <set>
#include <algorithm>

#include "../../types/MintTypes.h"

using namespace std;
using namespace SageBuilder;
using namespace SageInterface;


class CudaOptimizerInterface
{

public:
	CudaOptimizerInterface();
	virtual ~CudaOptimizerInterface();

	static SgVariableSymbol* getSymbolFromName(SgBasicBlock* block, string varStr);
  
	static SgScopeStatement*  findScopeForVarDeclaration(SgFunctionDefinition* func, 
							     SgInitializedName*& candidateVar);

	static SgInitializedName* createSharedMemoryBlock(SgFunctionDeclaration* kernel, 
							  const SgInitializedName* iname,
							  const int dim, 
							  const int order);


        static SgExpression* convertGlobalMemoryIndexIntoLocalIndex(SgExpression* exp, 
								    string index, 
								    SgBasicBlock* kernel);
  
	static vector<SgScopeStatement*> getAllScopes(SgBasicBlock* func_body, SgInitializedName*& candidateVar);

	static bool isWrittenBeforeRead(SgFunctionDeclaration* func,
					SgInitializedName* iname);					

	static SgScopeStatement* findCommonParentScope(SgScopeStatement* topScope, 
						       SgScopeStatement* s1,
						       SgScopeStatement* s2);

	static SgPntrArrRefExp* createGlobalMemoryReference(SgScopeStatement* varScope, 
							    SgInitializedName* candidateVar, 
							    int updown=0);

	static SgPntrArrRefExp* createGlobalMemoryRefWithOffset(SgScopeStatement* varScope, 
								SgInitializedName* candidateVar, 
								SgExpression* offset);

	static bool isSubScope(SgScopeStatement* parent, SgScopeStatement* child);

	static bool isReadOnly(const std::set<SgInitializedName*>& readOnlyVars, 
			       SgInitializedName* i_name);

	static void removeOtherSharedBlocks(SgFunctionDeclaration* kernel);


	static SgPntrArrRefExp* createSharedMemoryReference(SgScopeStatement* varScope, 
							    SgInitializedName* sh_block, 
							    SgExpression* first, 
							    SgExpression* second=NULL, 
							    SgExpression* third=NULL);

};

#endif 
