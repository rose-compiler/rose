#include "valueGraph.h"
#include <rose.h>
#include <utilities/utilities.h>
#include <slicing/backstrokeCFG.h>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <normalizations/expNormalization.h>

using namespace std;
using namespace boost;

#define foreach BOOST_FOREACH

std::set<SgMemberFunctionDeclaration*> Backstroke::FunctionCallNode::functionsToReverse;
std::ofstream Backstroke::FunctionCallNode::os("fileList.txt");


int main(int argc, char *argv[])
{
    // Build the AST used by ROSE
    SgProject* project = frontend(argc, argv);
    
    ClassHierarchyWrapper chWrapper(project);

#if 1
    // Draw the AST.
    CppToDotTranslator c;
    c.translate(argc,argv);
#endif
    set<string> eventList;
    eventList.insert("Handle");
#if 1
    eventList.insert("Timeout");
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
    eventList.insert("Notify");
    eventList.insert("DataIndication");
    eventList.insert("SendPending");
#endif
    
    set<SgFunctionDefinition*> funcDefs;
    set<SgFunctionDeclaration*> funcDecls;
    
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
                vector<SgFunctionDefinition*> defs;
                CallTargetSet::getDefinitionsForExpression(
                    funcCallNode.getFunctionCallExp(), &chWrapper, defs);
                foreach (SgFunctionDefinition* def, defs)
                    funcDefsUnprocessed.push(def);
                
                vector<SgFunctionDeclaration*> decls;
                CallTargetSet::getDeclarationsForExpression(
                    funcCallNode.getFunctionCallExp(), &chWrapper, decls, true);
                funcDecls.insert(decls.begin(), decls.end());
                funcDecls.insert(funcDef->get_declaration());
                
#if 0
                SgFunctionDeclaration* decl = 
                        isSgFunctionDeclaration(funcCallNode.funcDecl->get_definingDeclaration());
                if (decl)
                {
                    if (SgFunctionDefinition* def = decl->get_definition())
                        funcDefsUnprocessed.push(def);
                }
#endif
            }
        }

        funcDefs.insert(funcDef);
    }
    
    cout << "Functions being reversed:\n";
    foreach (SgFunctionDefinition* funcDef, funcDefs)
        cout << funcDef->get_declaration()->get_name() << '\n';
    //getchar();
    
    
    /***********************************************************************************************/
    // Reverse all functions.
    Backstroke::reverseFunctions(funcDefs);
    /***********************************************************************************************/
    
    

    cout << "\n\nDone!\n\n";
    
    cout << "Functions need to be reversed: \n";
    foreach (SgMemberFunctionDeclaration* decl, Backstroke::FunctionCallNode::functionsToReverse)
    {
        cout << decl->get_name() << '\n';
    }
    cout << "\n\n";

    //return backend(project);
    project->unparse();
    
    
    // For GTNetS, copy all generated files to SRC directory.
    foreach (const string& fileName, project->get_sourceFileNameList())
    {
        boost::filesystem::path p(fileName);
        
        string dir = p.parent_path().string();
        string name = p.filename().string();
        
        string command = "cp " + dir + "/rose_" + name + " " + dir + "../SRC/" + name;
        
        cout << command << endl;
        boost::filesystem::copy_file("rose_" + name, dir + "/../SRC/" + name, 
                boost::filesystem::copy_option::overwrite_if_exists);
        //system(command.c_str());
    }
    
    
    // Copy the header file as a source file then do the transformation then copy it back.
    //foreach (SgFunctionDefinition* funcDef, funcDefs)
    set<SgFunctionDeclaration*> funcDeclsToProcess;
    foreach (SgFunctionDeclaration* funcDecl, funcDecls)
    {
        //cout << funcDef->unparseToString() << endl;
        SgFunctionDeclaration* decl = isSgFunctionDeclaration(funcDecl->get_firstNondefiningDeclaration());
        // If funcDecl is NULL, this function is inlined.
        if (!decl) decl = funcDecl;
        funcDeclsToProcess.insert(decl);
    }
    
    ofstream osHeaders("headers.txt");
    foreach (SgFunctionDeclaration* funcDecl, funcDeclsToProcess)
    {
        cout << funcDecl->get_file_info()->get_filenameString() << endl;
        cout << funcDecl->get_name() << endl;
        osHeaders << funcDecl->get_file_info()->get_filenameString() << ' ' 
                << funcDecl->get_name() << '\n';
    }
    osHeaders.close();
    
    ::system("./headerUnparser/unparseHeader headers.txt");
}
