#include "rose.h"
#include <ExtractFunctionArguments.h>
#include <boost/foreach.hpp>

#define foreach BOOST_FOREACH

int main(int argc, char** argv)
{
	SgProject* project = frontend(argc, argv);
	ROSE_ASSERT(project != NULL);
	AstTests::runAllTests(project);
	
	std::vector<SgFunctionDefinition*> functions = SageInterface::querySubTree<SgFunctionDefinition>(project, V_SgFunctionDefinition);
	foreach(SgFunctionDefinition* function, functions)
	{
		ExtractFunctionArguments::NormalizeTree(function);
	}

	SageInterface::fixVariableReferences(project);
	AstTests::runAllTests(project);
	return backend(project);
}
