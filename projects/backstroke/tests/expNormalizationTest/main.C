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

    vector<SgFunctionDefinition*> func_defs =
            BackstrokeUtility::querySubTree<SgFunctionDefinition>(getFirstGlobalScope(project));
    foreach (SgFunctionDefinition* func_def, func_defs)
	{
		if (func_def->get_file_info()->isSameFile(source_file))
			BackstrokeNorm::normalizeEvent(func_def);
	}

    AstTests::runAllTests(project);
    return backend(project);
}


