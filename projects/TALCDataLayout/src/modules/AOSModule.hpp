/*
 * AOSModule.hpp
 *
 *  Created on: Oct 16, 2012
 *      Author: Kamal Sharma
 */

#ifndef AOS_HPP_
#define AOS_HPP_

#include <map>

#include "ModuleBase.hpp"
#include "StructUtil.hpp"

using namespace std;
using namespace SageBuilder;
using namespace SageInterface;

class AOSModule : public ModuleBase
{
public:
	AOSModule(Rose_STL_Container<string> &args, Meta *meta);
	void visit(SgProject * project);

private:
	void handleModuleOptions(Rose_STL_Container<string> &args);
	void process(SgProject * project);
	void processParameters(SgFunctionDeclaration *functionDeclaration);
	void processAllocation(SgFunctionDeclaration *functionDeclaration);
	void processReferences(SgFunctionDeclaration *functionDeclaration);

	string buildAllocationType(Field *field);
	SgType* buildParameterType(Field *field);

	void buildStructAtStart(Field *metaField, SgBasicBlock *block);
	bool isBuildStruct(Field *field);
	int getStructNo(Field *field);

	bool checkExtendedVariable(SgInitializedName *var);

	bool isVisit;
	bool extendedTypes;
	map<FieldBlock*,int> *structNoMap; // Local Data Struct to keep track of structs
	int sourceCodeStat; // Source Code Changes
};


#endif /* AOS_HPP_ */
