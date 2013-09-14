#include "rose.h"
#include <ExtractFunctionArguments.h>
#include <boost/foreach.hpp>
#include <iostream>

#define foreach BOOST_FOREACH

int main(int argc, char** argv)
{
	SgProject* project = frontend(argc, argv);
	ROSE_ASSERT(project != NULL);
	AstTests::runAllTests(project);
	
	std::vector<SgFunctionDefinition*> functions = SageInterface::querySubTree<SgFunctionDefinition>(project, V_SgFunctionDefinition);
	foreach(SgFunctionDefinition* function, functions)
	{
		ExtractFunctionArguments e;
                // Check IsNormalizable()
                std::cout<<"\n e.IsNormalizable:"<<e.IsNormalizable(function);
                // Check IsNormalized()
                std::cout<<"\n e.IsNormalized:"<<e.IsNormalized(function);
                // Normalize now...
		e.NormalizeTree(function);
                // Also call GetTemporariesIntroduced
                e.GetTemporariesIntroduced();
                // Check IsNormalizable()
                std::cout<<"\n e.IsNormalizable:"<<e.IsNormalizable(function);
                // Check IsNormalized()
                std::cout<<"\n e.IsNormalized:"<<e.IsNormalized(function);
	}

	SageInterface::fixVariableReferences(project);
	AstTests::runAllTests(project);
	return backend(project);
}
