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
    //mergeAST(project);
    
    ClassHierarchyWrapper classHierarchy(project);

#if 0
    // Draw the AST.
    CppToDotTranslator c;
    c.translate(argc,argv);
#endif
    set<pair<string, string> > eventList;
    eventList.insert(make_pair("WirelessLink", "Handle"));
#if 0
    eventList.insert(make_pair("WirelessLink", "Copy"));
    eventList.insert(make_pair("LinkReal", "Handle"));
    eventList.insert(make_pair("InterfaceWireless", "Notify"));
    eventList.insert(make_pair("Interface", "Notify"));
    eventList.insert(make_pair("InterfaceReal", "Notify"));
    eventList.insert(make_pair("L2Proto802_11", "Notify"));
    eventList.insert(make_pair("TCP", "Notify"));
    eventList.insert(make_pair("L2Proto802_11", "Handle"));
    eventList.insert(make_pair("LinkReal", "Transmit"));
    eventList.insert(make_pair("Timer", "Handle"));
    eventList.insert(make_pair("Node", "TracePDU"));
    
    
    eventList.insert(make_pair("InterfaceReal", "AddNotify"));
    eventList.insert(make_pair("L4Protocol", "RequestNotification"));
    eventList.insert(make_pair("IPV4", "DataRequest"));
    eventList.insert(make_pair("NodeReal", "GetQueue"));
    eventList.insert(make_pair("Node", "TracePDU"));
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

        SgDeclarationStatement* firstDecl = 
            funcDef->get_declaration()->get_firstNondefiningDeclaration();
        if (firstDecl == NULL)
            firstDecl = funcDef->get_declaration();
        
        SgClassDefinition* classDef = SageInterface::getEnclosingClassDefinition(firstDecl);
        if (!classDef) 
            continue;
        
        string className = classDef->get_declaration()->get_name();
        string funcName = funcDef->get_declaration()->get_name();
        //cout << "FUNC:\t" << funcName << endl;
        if (eventList.count(make_pair(className, funcName)) > 0)
        {
            funcDefsUnprocessed.push(funcDef);
            //funcDefs.insert(funcDef);
        }
    }
    
    // Search all events and find all functions which should be reversed.
 
    set<string> funcDefsAdded;
    vector<SgFunctionDefinition*> allFuncDefs = 
            BackstrokeUtility::querySubTree<SgFunctionDefinition>(project);
    
    while (!funcDefsUnprocessed.empty())
    {
        SgFunctionDefinition* funcDef  = funcDefsUnprocessed.top();
        funcDefsUnprocessed.pop();

        if (funcDefs.count(funcDef))
            continue;
        
        string mangledName = funcDef->get_declaration()->get_mangled_name();
        if (funcDefsAdded.count(mangledName))
            continue;
        funcDefsAdded.insert(mangledName);
        
        cout << "\nSearching function " << funcDef->get_declaration()->get_name() << "\n\n";

        vector<SgFunctionCallExp*> funcCalls = 
                BackstrokeUtility::querySubTree<SgFunctionCallExp>(funcDef);
        foreach (SgFunctionCallExp* funcCall, funcCalls)
        {
            Backstroke::FunctionCallNode funcCallNode(funcCall);
            if (funcCallNode.canBeReversed)
            {
                //cout << funcCall->unparseToString() << endl;
#if 1
                vector<SgFunctionDefinition*> defs;
                CallTargetSet::getDefinitionsForExpression(funcCall, &classHierarchy, defs);
                foreach (SgFunctionDefinition* def, defs)
                    funcDefsUnprocessed.push(def);
#endif
                vector<SgFunctionDeclaration*> decls;
                if (funcCallNode.isVirtual)
                {
                    CallTargetSet::getDeclarationsForExpression(funcCall, &classHierarchy, decls, true);
                    funcDecls.insert(decls.begin(), decls.end());
                    funcDecls.insert(funcDef->get_declaration());
                }
                else
                {
                    funcDecls.insert(funcCallNode.funcDecl);
                    decls.push_back(funcCallNode.funcDecl);
                }
               
                //cout << decls.size() << endl;

#if 1
                foreach (SgFunctionDefinition* def, allFuncDefs)
                {
                    foreach (SgFunctionDeclaration* decl, decls)
                    {
                        if (def->get_declaration()->get_mangled_name() == decl->get_mangled_name())
                            funcDefsUnprocessed.push(def);
                    }
                }
#endif
                
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
        funcDecls.insert(funcDef->get_declaration());
    }
    
    cout << "Functions being reversed:\n";
    foreach (SgFunctionDefinition* funcDef, funcDefs)
        cout << funcDef->get_declaration()->get_name() << '\n';
    
    cout << "\n\n";
    
#if 0
    foreach (SgFunctionDeclaration* funcDecl, funcDecls)
        cout << funcDecl->get_name() << '\n';
#endif
    getchar();
    
    
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
    foreach (SgFunctionDefinition* funcDef, funcDefs)
    //foreach (SgFunctionDeclaration* funcDecl, funcDeclsToProcess)
    {
        SgFunctionDeclaration* funcDecl = funcDef->get_declaration();
        cout << funcDecl->get_file_info()->get_filenameString() << endl;
        cout << funcDecl->get_name().str() << endl;
        osHeaders << funcDecl->get_file_info()->get_filenameString() << ' ' 
                << funcDecl->get_name().str() << '\n';
    }
    osHeaders.close();
    
    ::system("./headerUnparser/unparseHeader headers.txt");
}
