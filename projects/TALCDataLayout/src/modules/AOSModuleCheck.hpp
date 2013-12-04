/*
 * AOSModuleCheck.hpp
 *
 *  Created on: Sep 3, 2013
 *      Author: Kamal Sharma
 */

#ifndef AOSCHECK_HPP_
#define AOSCHECK_HPP_

#include <map>
#include <vector>

#include "ModuleBase.hpp"
#include "StructUtil.hpp"

using namespace std;
using namespace SageBuilder;
using namespace SageInterface;

class AOSModuleCheck : public ModuleBase
{
public:
	AOSModuleCheck(Rose_STL_Container<string> &args, Meta *meta);
	void visit(SgProject * project);

private:
	void handleModuleOptions(Rose_STL_Container<string> &args);
	void process(SgProject * project);
	void checkParameterTypes(SgFunctionDeclaration *functionDeclaration);
	void checkAllocation(SgFunctionDeclaration *functionDeclaration);
	void checkReferences(SgFunctionDeclaration *functionDeclaration);
	void checkActualFormalParameters(SgFunctionDeclaration *functionDeclaration);
	void checkViewSpace(SgFunctionDeclaration *functionDeclaration);

	bool checkExtendedVariable(SgInitializedName *var);

	bool isVisit;
	bool extendedTypes;
	vector<SgFunctionDeclaration*> functionList;
};


#endif /* AOS_HPP_ */
