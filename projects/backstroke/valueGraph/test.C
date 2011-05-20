#include "valueGraph.h"
#include <slicing/backstrokeCFG.h>
#include <boost/lexical_cast.hpp>
#include <normalizations/expNormalization.h>

using namespace std;
using namespace boost;

#define foreach BOOST_FOREACH

int main(int argc, char *argv[])
{
    // Build the AST used by ROSE
    SgProject* project = frontend(argc, argv);
    SgSourceFile* sourceFile = isSgSourceFile((*project)[0]);

    int counter = 0;

#if 0
    // Draw the AST.
    CppToDotTranslator c;
    c.translate(argc,argv);
#endif
    set<string> eventList;
    eventList.insert("TransmitComplete");
    eventList.insert("Handle");
    eventList.insert("TransmitComplete");
    eventList.insert("TransmitComplete");
    
    // Process all function definition bodies for static control flow graph generation
    Rose_STL_Container<SgNode*> functions = NodeQuery::querySubTree(project, V_SgFunctionDefinition);
    for (Rose_STL_Container<SgNode*>::const_iterator i = functions.begin(); i != functions.end(); ++i)
    {
        SgFunctionDefinition* funcDef = isSgFunctionDefinition(*i);
        ROSE_ASSERT(funcDef != NULL);
        if (eventList.count(funcDef->get_declaration()->get_name()) == 0)
            continue;

        //string cfgFileName = "CFG" + boost::lexical_cast<string > (counter) + ".dot";
        //string vgFileName = "VG" + boost::lexical_cast<string > (counter) + ".dot";
        string cfgFileName = "CFG.dot";
        string cdgFileName = "CDG.dot";
        string vgFileName = "VG.dot";


        if (!funcDef->get_file_info()->isSameFile(sourceFile))
            continue;

        BackstrokeNorm::normalizeEvent(funcDef->get_declaration());
        Backstroke::BackstrokeCFG cfg(funcDef);
        cfg.toDot(cfgFileName);
        
        Backstroke::BackstrokeCDG cdg(cfg);
        cdg.toDot(cdgFileName);

//        Backstroke::FilteredCFG filteredCFG(funcDef);
//        filteredCFG.toDot("filteredCFG.dot");
//
//        Backstroke::FullCFG fullCFG(funcDef);
//        fullCFG.toDot("fullCFG.dot");




        Backstroke::EventReverser reverser(funcDef);
        //reverser.buildValueGraph();
        reverser.generateCode();

        //reverser.valueGraphToDot(vgFileName);

        cout << "Function " << counter << " is processed.\n";
        ++counter;

        //break;
    }
    // Prepend includes to test files.
    SgGlobal* globalScope = SageInterface::getFirstGlobalScope(project);
    SageInterface::insertHeader("rctypes.h", PreprocessingInfo::after, false, globalScope);


    return backend(project);
}
