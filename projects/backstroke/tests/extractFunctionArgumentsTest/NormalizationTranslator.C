#include "rose.h"
#include "normalizations/ExtractFunctionArguments.h"
#include "restrictedLanguage/LanguageRestrictions.h"
#include "utilities/cppDefinesAndNamespaces.h"

int main(int argc, char** argv)
{
	SgProject* project = frontend(argc, argv);
	ROSE_ASSERT(project != NULL);
	AstTests::runAllTests(project);

	ExtractFunctionArguments::NormalizeTree(project);

	AstTests::runAllTests(project);
	return backend(project);
}
