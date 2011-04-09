#include "valueGraph.h"
#include <slicing/backstrokeCFG.h>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

#define foreach BOOST_FOREACH

int main(int argc, char *argv[])
{
    // Build the AST used by ROSE
    SgProject* project = frontend(argc, argv);
    SgSourceFile* sourceFile = isSgSourceFile((*project)[0]);

    int counter = 0;

    // Process all function definition bodies for static control flow graph generation
    Rose_STL_Container<SgNode*> functions = NodeQuery::querySubTree(project, V_SgFunctionDefinition);
    for (Rose_STL_Container<SgNode*>::const_iterator i = functions.begin(); i != functions.end(); ++i)
    {
        //string cfgFileName = "CFG" + boost::lexical_cast<string > (counter) + ".dot";
        //string vgFileName = "VG" + boost::lexical_cast<string > (counter) + ".dot";
        string cfgFileName = "CFG.dot";
        string vgFileName = "VG.dot";

        SgFunctionDefinition* funcDef = isSgFunctionDefinition(*i);
        ROSE_ASSERT(funcDef != NULL);

        if (!funcDef->get_file_info()->isSameFile(sourceFile))
            continue;

        Backstroke::BackstrokeCFG cfg(funcDef);
        cfg.toDot(cfgFileName);



        Backstroke::EventReverser reverser(funcDef);
        reverser.generateCode();

        reverser.valueGraphToDot(vgFileName);

        cout << "Function " << counter << " is processed.\n";
        ++counter;

        break;
    }
    // Prepend includes to test files.
    SgGlobal* globalScope = SageInterface::getFirstGlobalScope(project);
    SageInterface::insertHeader("rctypes.h", PreprocessingInfo::after, false, globalScope);

    return backend(project);
}