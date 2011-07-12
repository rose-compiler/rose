#include "valueGraph.h"
#include <rose.h>
#include <utilities/utilities.h>
#include <slicing/backstrokeCFG.h>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <boost/unordered_map.hpp>
#include <normalizations/expNormalization.h>

using namespace std;
using namespace boost;

#define foreach BOOST_FOREACH

std::set<SgMemberFunctionDeclaration*> Backstroke::FunctionCallNode::functionsToReverse;
std::ofstream Backstroke::FunctionCallNode::os("fileList.txt");

vector<string> serializeMemberFunction(SgMemberFunctionDeclaration* memFuncDecl)
{
    vector<string> strings;
    
    SgDeclarationStatement* firstDecl = memFuncDecl->get_firstNondefiningDeclaration();
    if (!firstDecl) firstDecl = memFuncDecl;
    
    string className = memFuncDecl->get_class_scope()->get_declaration()->get_name().str();
    strings.push_back(className);
    strings.push_back(memFuncDecl->get_name().str());
    strings.push_back(boost::lexical_cast<string>(firstDecl->get_file_info()->get_line()));
    
#if 0
    const SgInitializedNamePtrList& args = memFuncDecl->get_args();
    strings.push_back(boost::lexical_cast<string>(args.size()));
    foreach (SgInitializedName* arg, args)
        strings.push_back(arg->get_type()->get_mangled().str());
#endif
    
    return strings;
}

void readFunctionInfo(const string& filename, set<vector<string> >& functionsToReverse)
{
    ifstream funcListReader(filename.c_str());
    string className, funcName;
    //int argNum;
    int lineNum;
    while (funcListReader >> className >> funcName >> lineNum)
    {
        vector<string> strings;
        strings.push_back(className);
        strings.push_back(funcName);
        strings.push_back(boost::lexical_cast<string>(lineNum));
        
#if 0
        for (int i = 0; i < argNum; ++i)
        {
            string argName;
            funcListReader >> argName;
            strings.push_back(argName);
        }
#endif
        
        functionsToReverse.insert(strings);
    }
    funcListReader.close();
}

//bool checkMemberFunctionWithStrings(SgMemberFunctionDeclaration* memFuncDecl, const vector<string>& strings)
//{
//}

void generateFunctionList(const set<pair<string, string> >& eventList, ostream& os)
{
    SgProject* project = SageInterface::getProject();
    ClassHierarchyWrapper classHierarchy(project);
    
    set<SgFunctionDeclaration*> funcDecls;
    
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
    
    boost::unordered_map<string, SgFunctionDefinition*> nameToFuncDef;
    foreach (SgFunctionDefinition* funcDef, allFuncDefs)
    {
        nameToFuncDef[funcDef->get_declaration()->get_mangled_name()] = funcDef;
    }
    
    while (!funcDefsUnprocessed.empty())
    {
        SgFunctionDefinition* funcDef  = funcDefsUnprocessed.top();
        funcDefsUnprocessed.pop();
        
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
            cout << funcCall->unparseToString() << " " << funcCallNode.canBeReversed << endl;
            if (funcCallNode.canBeReversed)
            {
                vector<SgFunctionDeclaration*> decls;

                CallTargetSet::getDeclarationsForExpression(funcCall, &classHierarchy, decls, true);
                funcDecls.insert(decls.begin(), decls.end());
                
                foreach (SgFunctionDeclaration* decl, decls)
                {
                    string name = decl->get_mangled_name().str();
                    if (nameToFuncDef.count(name))
                        funcDefsUnprocessed.push(nameToFuncDef[name]);
                }
            }
        }

        funcDecls.insert(funcDef->get_declaration());
    }
    
    set<string> funcNames;
    set<vector<string> > writtenFuncs;
    foreach (SgFunctionDeclaration* funcDecl, funcDecls)
    {
        SgMemberFunctionDeclaration* memFuncDecl = isSgMemberFunctionDeclaration(funcDecl);
        if (!memFuncDecl)
            continue;
        
        vector<string> strings = serializeMemberFunction(memFuncDecl);
        
        if (writtenFuncs.count(strings))
            continue;
        writtenFuncs.insert(strings);
        
        foreach (const string& str, strings)
            os << str << ' ';
        os << '\n';
        
        funcNames.insert(funcDecl->get_mangled_name().str());
    }
        
    foreach (SgFunctionDeclaration* funcDecl, funcDecls)
    {
        SgDeclarationStatement* firstDecl = 
            funcDecl->get_firstNondefiningDeclaration();
        if (firstDecl == NULL)
            firstDecl = funcDecl;
        SgClassDefinition* classDef = SageInterface::getEnclosingClassDefinition(firstDecl);
        cout << (classDef ? classDef->get_declaration()->get_name() : "") << 
                " : " << funcDecl->get_name().str() << '\n';
    }
    cout << funcNames.size() << endl;
}

void processHeaderFiles(const set<SgFunctionDeclaration*>& funcDecls)
{
    set<string> headers;
    ofstream osHeaders("headers.txt");
    
    foreach (SgFunctionDeclaration* funcDecl, funcDecls)
    {
        SgFunctionDeclaration* firstDecl = 
            isSgFunctionDeclaration(funcDecl->get_firstNondefiningDeclaration());
        if (firstDecl == NULL)
            firstDecl = funcDecl;
        
        headers.insert(firstDecl->get_file_info()->get_filenameString());
    }
    
    int counter = 0;
    foreach (const string& header, headers)
    {
        cout << header << endl;
        osHeaders << header << '\n';
        counter++;
        
        if (counter == 10)
        {
            osHeaders.close();
            ::system("./headerUnparser/unparseHeader headers.txt");
            counter = 0;
            osHeaders.open("headers.txt");
        }
    }
    osHeaders.close();
    
    //SgProject_clearMemoryPool();
    
    ::system("./headerUnparser/unparseHeader headers.txt");
}

int main(int argc, char *argv[])
{
    int option;
    cin >> option;
        
    // Build the AST used by ROSE
    SgProject* project = frontend(argc, argv);

    if (option == 2)
    {
        //mergeAST(project);

        set<pair<string, string> > eventList;
        eventList.insert(make_pair("WirelessLink", "Handle"));

        ofstream funcListWriter("funcList.txt");
        generateFunctionList(eventList, funcListWriter);
        funcListWriter.close();
    }
    else if (option == 0)
    {
        //mergeAST(project);

        set<SgFunctionDefinition*> funcDefs;
    
        set<vector<string> > functionsToReverse;
        readFunctionInfo("funcList.txt", functionsToReverse);

        vector<SgFunctionDefinition*> allFuncDefs = 
                BackstrokeUtility::querySubTree<SgFunctionDefinition>(project);
        foreach (SgFunctionDefinition* funcDef, allFuncDefs)
        {
            SgMemberFunctionDeclaration* funcDecl = 
                    isSgMemberFunctionDeclaration(funcDef->get_declaration());
            if (!funcDecl) continue;

            if (functionsToReverse.count(serializeMemberFunction(funcDecl)))
            {
                //cout << funcDef->get_declaration()->get_name().str() << " : " <<
                //    funcName << endl;
                funcDefs.insert(funcDef);
            }
        }



        /********************************************************************************/
        // Reverse all functions.
        Backstroke::reverseFunctions(funcDefs);
        /********************************************************************************/



        cout << "\n\nDone!\n\n";

    #if 0
        cout << "Functions need to be reversed: \n";
        foreach (SgMemberFunctionDeclaration* decl, Backstroke::FunctionCallNode::functionsToReverse)
        {
            cout << decl->get_name() << '\n';
        }
        cout << "\n\n";
    #endif

        //return backend(project);
        project->unparse();


        // For GTNetS, copy all generated files to SRC directory.
        foreach (const string& fileName, project->get_sourceFileNameList())
        {
            boost::filesystem::path p(fileName);

            string dir = p.parent_path().string();
            //string name = p.filename().string();
            string name = p.string();

            string command = "cp " + dir + "/rose_" + name + " " + dir + "../SRC/" + name;

            cout << command << endl;
            boost::filesystem::copy_file("rose_" + name, dir + "/../SRC/" + name
                    /*, boost::filesystem::copy_option::overwrite_if_exists*/
            );
            //system(command.c_str());
        }
    }
    
    else if (option == 1)
    {
        set<vector<string> > functionsToReverse;
        readFunctionInfo("funcList.txt", functionsToReverse);
        
        set<SgFunctionDeclaration*> funcDecls;
        vector<SgFunctionDeclaration*> allFuncDecls = 
                BackstrokeUtility::querySubTree<SgFunctionDeclaration>(project);
        foreach (SgFunctionDeclaration* funcDecl, allFuncDecls)
        {
            SgMemberFunctionDeclaration* memFuncDecl = 
                    isSgMemberFunctionDeclaration(funcDecl);
            if (!memFuncDecl) continue;

            if (functionsToReverse.count(serializeMemberFunction(memFuncDecl)))
                funcDecls.insert(funcDecl);
        }
        processHeaderFiles(funcDecls);
    }
    
    else if (option == 3)
    {
        set<SgFunctionDefinition*> funcDefs;
        
        set<string> events;
        events.insert("Handle");
        events.insert("tmr_event_handler");
    
        vector<SgFunctionDefinition*> allFuncDefs = 
                BackstrokeUtility::querySubTree<SgFunctionDefinition>(project);
        foreach (SgFunctionDefinition* funcDef, allFuncDefs)
        {
            if (events.count(funcDef->get_declaration()->get_name().str()))
                funcDefs.insert(funcDef);
        }

        /***********************************************************************************************/
        // Reverse all functions.
        Backstroke::reverseFunctions(funcDefs);
        project->unparse();
    }
}
