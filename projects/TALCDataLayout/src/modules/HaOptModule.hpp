/*
 * HaOptModule.hpp
 *
 *  Created on: Oct 16, 2012
 *      Author: Kamal Sharma
 */

#ifndef HAOPT_HPP_
#define HAOPT_HPP_

#include "Common.hpp"
#include "ModuleBase.hpp"
#include "HaOptUtil.hpp"

using namespace std;
using namespace SageBuilder;
using namespace SageInterface;

class HaOptModule : public ModuleBase
{
public:
	HaOptModule(Rose_STL_Container<string> &args, Meta *meta);
	void visit(SgProject * project);

private:
	void handleModuleOptions(Rose_STL_Container<string> &args);

	void process(SgProject *project);
	void processParameters(SgFunctionDeclaration *functionDeclaration);
	void processVariableDecls(SgFunctionDeclaration *functionDeclaration);
	void processPragma(SgProject *project);

	bool isVisit;
	bool applyRestrict;
	CompilerTypes::TYPE compiler;
};


#endif /* HAOPT_HPP_ */
