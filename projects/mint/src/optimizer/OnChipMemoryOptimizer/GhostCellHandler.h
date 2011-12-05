/*
 * 
 *
 *  Created on: March 6, 2011
 *      Author: didem
 */

#ifndef GHOSTCELLHANDLER_H_
#define GHOSTCELLHANDLER_H_

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

class GhostCellHandler
{

public:
	GhostCellHandler();
	virtual ~GhostCellHandler();

	static void loadGhostCellsIntoSharedMemory(SgFunctionDefinition* kernel, 
						   const SgInitializedName* candidateVar,
						   SgInitializedName* sharedRef, 
						   const set<SgExpression*>& ghostCells, 
						   bool regShared, int order, bool unrollWithoutRegisters, 
						   SgStatement* sourcePosOutside, SgScopeStatement* varScope);

	static void loadGhostCellsIntoSharedMemory(SgFunctionDefinition* kernel, 
						   SgInitializedName* candidateVar,
						   SgInitializedName* sharedRef, 
						   bool regShared, int order, bool unrollWithoutRegisters, 
						   SgStatement* sourcePosOutside, SgScopeStatement* varScope);


	static void defineGhostCellVariables(SgFunctionDefinition* func_def,
					     const SgInitializedName* candidateVar, const int, const bool);

private:

	static void loadThisGhostCell(const SgInitializedName* candidateVar, 
				      SgExpression* rhs, SgExpression* lhs, const std::vector<SgExpression*>& subscripts,
				      SgFunctionDefinition* kernel, SgStatement* scope, const int order, std::vector<SgIfStmt*>& ifCondList);

	static void loadThisGhostCell(const SgInitializedName* candidateVar, 
				      SgExpression* rhs, SgExpression* lhs,
				      SgFunctionDefinition* kernel, SgStatement* scope, int order);


        static SgExpression* buildGhostCellLoadConditionExp(SgScopeStatement* kernel_body,
							   const std::vector<SgExpression*> subscripts,
							   int order);

};

#endif 
