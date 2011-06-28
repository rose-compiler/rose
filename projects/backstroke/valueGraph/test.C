#include "valueGraph.h"
#include <utilities/utilities.h>
#include <slicing/backstrokeCFG.h>
#include <boost/lexical_cast.hpp>
#include <normalizations/expNormalization.h>

using namespace std;
using namespace boost;

#define foreach BOOST_FOREACH

std::set<SgMemberFunctionDeclaration*> Backstroke::FunctionCallNode::functionsToReverse;
std::ofstream Backstroke::FunctionCallNode::os("fileList.txt");

// This function is used to help find the last defs of all local variables. This is a word-around
// due to the problem in SSA.
void addNullStmtAtScopeEnd(SgFunctionDefinition* funcDef)
{
    vector<SgBasicBlock*> scopes = BackstrokeUtility::querySubTree<SgBasicBlock>(funcDef);
    foreach (SgBasicBlock* scope, scopes)
    {
        SageInterface::appendStatement(SageBuilder::buildNullStatement(), scope);
    }
}

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
    eventList.insert("Handle");
#if 0
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
    eventList.insert("Send");
    eventList.insert("Timeout");
    eventList.insert("Notify");
    eventList.insert("DataIndication");
    eventList.insert("SendPending");
#endif
    
    set<SgFunctionDefinition*> funcDefs;
    
    //set<SgFunctionDefinition*> 
    
    stack<SgFunctionDefinition*> funcDefsUnprocessed;

    // Process all function definition bodies for static control flow graph generation
    vector<SgNode*> functions = NodeQuery::querySubTree(project, V_SgFunctionDefinition);
    for (Rose_STL_Container<SgNode*>::const_iterator i = functions.begin(); 
            i != functions.end(); ++i)
    {
        SgFunctionDefinition* funcDef = isSgFunctionDefinition(*i);
        ROSE_ASSERT(funcDef != NULL);

        string funcName = funcDef->get_declaration()->get_name();
        //cout << "FUNC:\t" << funcName << endl;
        if (eventList.count(funcName) > 0)
        {
            funcDefsUnprocessed.push(funcDef);
            //funcDefs.insert(funcDef);
        }
    }
    
    // Search all events and find all functions which should be reversed.
 
    while (!funcDefsUnprocessed.empty())
    {
        SgFunctionDefinition* funcDef  = funcDefsUnprocessed.top();
        funcDefsUnprocessed.pop();

        if (funcDefs.count(funcDef))
            continue;
        
        cout << "\nSearching function " << funcDef->get_declaration()->get_name() << "\n\n";

        vector<SgFunctionCallExp*> funcCalls = 
                BackstrokeUtility::querySubTree<SgFunctionCallExp>(funcDef);
        foreach (SgFunctionCallExp* funcCall, funcCalls)
        {
            Backstroke::FunctionCallNode funcCallNode(funcCall);
            if (funcCallNode.canBeReversed)
            {
                SgFunctionDeclaration* decl = 
                        isSgFunctionDeclaration(funcCallNode.funcDecl->get_definingDeclaration());
                if (decl)
                {
                    if (SgFunctionDefinition* def = decl->get_definition())
                        funcDefsUnprocessed.push(def);
                }
            }
        }

        funcDefs.insert(funcDef);
    }
    
    cout << "Functions being reversed:\n";
    foreach (SgFunctionDefinition* funcDef, funcDefs)
        cout << funcDef->get_declaration()->get_name() << '\n';
    //getchar();
        
    foreach (SgFunctionDefinition* funcDef, funcDefs)
    {
        BackstrokeNorm::normalizeEvent(funcDef->get_declaration());
        // Add a null statement at the end of all scopes then we can find the last defs of variables.
        addNullStmtAtScopeEnd(funcDef);
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
        string cfgFileName = funcName + "_CFG.dot";
        string cdgFileName = funcName + "_CDG.dot";
        string vgFileName = "VG.dot";

        //if (!funcDef->get_file_info()->isSameFile(sourceFile))
        //    continue;

#if 1
        Backstroke::FullCFG fullCfg(funcDef);
        fullCfg.toDot(funcName + "_fullCFG.dot");

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
    
    cout << "Functions need to be reversed: \n";
    foreach (SgMemberFunctionDeclaration* decl, Backstroke::FunctionCallNode::functionsToReverse)
    {
        cout << decl->get_name() << '\n';
    }
    cout << "\n\n";

    return backend(project);
}
