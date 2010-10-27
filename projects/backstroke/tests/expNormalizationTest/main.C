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

	set<SgFunctionDeclaration*> decls;

    foreach (SgFunctionDeclaration* func_decl, func_decls)
	{
		func_decl = isSgFunctionDeclaration(func_decl->get_definingDeclaration());

		// Make sure every function is normalized once.
		if (decls.count(func_decl) > 0)
			continue;
		decls.insert(func_decl);

		if (func_decl && func_decl->get_file_info()->isSameFile(source_file))
		{
			SgFunctionDeclaration* normalized_decl = BackstrokeNorm::normalizeEvent(func_decl);
			if (normalized_decl)
				insertStatementAfter(func_decl, normalized_decl);
		}
	}

    AstTests::runAllTests(project);
    return backend(project);
}


