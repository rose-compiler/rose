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
     // DQ (1/4/2018): Comment out to isolate the bug in test2018_04.C.
     // Normalize now...
        e.NormalizeTree(function);
     // printf ("Commented out call to NormalizeTree(); \n");

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

     // DQ (1/4/2018): Comment out to isolate the bug in test2018_04.C.
        SageInterface::fixVariableReferences(project);
     // printf ("Commented out call to SageInterface::fixVariableReferences(project); \n");

	AstTests::runAllTests(project);
	return backend(project);
}
