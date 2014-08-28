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
#if 0
     // DQ (5/20/2014): This results in an untolerable amount of output spew from Boost test codes.
     // Check IsNormalizable()
        std::cout << "\n e.IsNormalizable:" << e.IsNormalizable(function);
     // Check IsNormalized()
        std::cout << "\n e.IsNormalized:" << e.IsNormalized(function);
#endif
     // Normalize now...
        e.NormalizeTree(function);

     // Also call GetTemporariesIntroduced
        e.GetTemporariesIntroduced();

#if 0
     // DQ (5/20/2014): This results in an untolerable amount of output spew from Boost test codes.
     // Check IsNormalizable()
        std::cout << "\n e.IsNormalizable:" << e.IsNormalizable(function);
     // Check IsNormalized()
        std::cout << "\n e.IsNormalized:" << e.IsNormalized(function);
#endif
      }

	SageInterface::fixVariableReferences(project);
	AstTests::runAllTests(project);
	return backend(project);
}
