#include <normalizations/expNormalization.h>
#include <utilities/utilities.h>
#include <utilities/cppDefinesAndNamespaces.h>

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;

int main(int argc, char * argv[])
{
    SgProject* project = frontend(argc, argv);
	SgSourceFile* source_file = isSgSourceFile((*project)[0]);

    vector<SgFunctionDeclaration*> func_decls =
            BackstrokeUtility::querySubTree<SgFunctionDeclaration>(getFirstGlobalScope(project));

    foreach (SgFunctionDeclaration* func_decl, func_decls)
	{
		// Make sure every function is normalized once.
		if (func_decl != func_decl->get_firstNondefiningDeclaration())
			continue;
		
		// Can't normalize functions that don't have bodies
		if (func_decl->get_definingDeclaration() == NULL)
			continue;

		if (func_decl && func_decl->get_file_info()->isSameFile(source_file))
		{
			BackstrokeNorm::normalizeEvent(func_decl);
		}
	}

    AstTests::runAllTests(project);
    return backend(project);
}


