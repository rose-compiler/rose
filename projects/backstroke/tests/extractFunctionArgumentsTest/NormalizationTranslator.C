#include "rose.h"
#include "normalizations/ExtractFunctionArguments.h"
#include "restrictedLanguage/LanguageRestrictions.h"
#include "utilities/cppDefinesAndNamespaces.h"
#include <boost/foreach.hpp>
#include "normalizations/expNormalization.h"

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

	AstTests::runAllTests(project);
	return backend(project);
}
