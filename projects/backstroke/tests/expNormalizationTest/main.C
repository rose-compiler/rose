#include <normalizations/expNormalization.h>
#include <utilities/Utilities.h>
#include <utilities/CPPDefinesAndNamespaces.h>

using namespace SageInterface;
using namespace SageBuilder;


int main(int argc, char * argv[])
{
    SgProject* project = frontend(argc, argv);

    vector<SgFunctionDefinition*> func_defs =
            backstroke_util::querySubTree<SgFunctionDefinition>(getFirstGlobalScope(project));
    foreach (SgFunctionDefinition* func_def, func_defs)
        backstroke_norm::normalizeEvent(func_def);  

    AstTests::runAllTests(project);
    return backend(project);
}


