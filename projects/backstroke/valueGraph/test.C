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

#if 1
    // Draw the AST.
    CppToDotTranslator c;
    c.translate(argc,argv);
#endif
    set<string> eventList;
#if 1
    eventList.insert("Handle");
    eventList.insert("StartApp");
    eventList.insert("StopApp");
    eventList.insert("TransmitComplete");
    eventList.insert("SendNotification");
    eventList.insert("WirelessTxStart");
    eventList.insert("WirelessTxEnd");
    eventList.insert("PacketRxStart");
    eventList.insert("PacketRxEnd");
    eventList.insert("GetL2Proto");
    eventList.insert("Busy");
    //eventList.insert("Send");
    eventList.insert("Timeout");
    eventList.insert("Notify");
#endif
    eventList.insert("DataIndication");
    eventList.insert("SendPending");
    
    vector<SgFunctionDefinition*> funcDefs;

    // Process all function definition bodies for static control flow graph generation
    Rose_STL_Container<SgNode*> functions = NodeQuery::querySubTree(project, V_SgFunctionDefinition);
    for (Rose_STL_Container<SgNode*>::const_iterator i = functions.begin(); i != functions.end(); ++i)
    {
        SgFunctionDefinition* funcDef = isSgFunctionDefinition(*i);
        ROSE_ASSERT(funcDef != NULL);

        string funcName = funcDef->get_declaration()->get_name();
        //cout << "FUNC:\t" << funcName << endl;
        if (eventList.count(funcName) > 0)
        {
            BackstrokeNorm::normalizeEvent(funcDef->get_declaration());
            funcDefs.push_back(funcDef);
        }
    }

    StaticSingleAssignment* ssa = new StaticSingleAssignment(SageInterface::getProject());
    ssa->run(true);

    set<SgGlobal*> globalScopes;

    foreach (SgFunctionDefinition* funcDef, funcDefs)
    {
        string funcName = funcDef->get_declaration()->get_name();
        globalScopes.insert(SageInterface::getGlobalScope(funcDef));

        cout << "\nNow processing " << funcName << "\tfrom\n";
        cout << funcDef->get_file_info()->get_filenameString() << "\n\n";

        //string cfgFileName = "CFG" + boost::lexical_cast<string > (counter) + ".dot";
        //string vgFileName = "VG" + boost::lexical_cast<string > (counter) + ".dot";
        string cfgFileName = "CFG.dot";
        string cdgFileName = "CDG.dot";
        string vgFileName = "VG.dot";

        //if (!funcDef->get_file_info()->isSameFile(sourceFile))
        //    continue;

#if 1
        Backstroke::FullCFG fullCfg(funcDef);
        fullCfg.toDot("fullCFG.dot");

        Backstroke::BackstrokeCFG cfg(funcDef);
        cfg.toDot(cfgFileName);
        
        Backstroke::BackstrokeCDG cdg(cfg);
        cdg.toDot(cdgFileName);

//        Backstroke::FilteredCFG filteredCFG(funcDef);
//        filteredCFG.toDot("filteredCFG.dot");
//
//        Backstroke::FullCFG fullCFG(funcDef);
//        fullCFG.toDot("fullCFG.dot");
#endif
        
#if 1
        //reverser.buildValueGraph();
        Backstroke::EventReverser reverser(ssa);
        reverser.reverseEvent(funcDef);
#endif

        reverser.valueGraphToDot(vgFileName);

        cout << "\nFunction " << funcName << " is processed.\n\n";
    }

    cout << "The number of global scopes is: " << globalScopes.size() << endl;
    
    // Prepend includes to test files.
    foreach (SgGlobal* globalScope, globalScopes)
        SageInterface::insertHeader("rctypes.h", PreprocessingInfo::after, false, globalScope);

    //AstTests::runAllTests(project);

    cout << "\n\nDone!\n\n";

    return backend(project);
}
