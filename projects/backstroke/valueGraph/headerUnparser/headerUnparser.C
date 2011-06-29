#include <rose.h>
#include <fstream>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <utilities/utilities.h>
#include <valueGraph/valueGraph.h>

using namespace std;
using namespace boost;
using namespace boost::filesystem;
using namespace SageInterface;
using namespace SageBuilder;

#define foreach BOOST_FOREACH


std::set<SgMemberFunctionDeclaration*> Backstroke::FunctionCallNode::functionsToReverse;
std::ofstream Backstroke::FunctionCallNode::os("fileList.txt");

void buildThreeFuncDeclWithEmptyBody(SgFunctionDeclaration* funcDecl)
{
    SgName funcName = funcDecl->get_name();
    SgScopeStatement* funcScope = funcDecl->get_scope(); 
    SgType* returnType = funcDecl->get_orig_return_type();
    
    SgName fwdFuncName = funcName + "_forward";
    SgFunctionDeclaration* fwdFuncDecl = buildDefiningMemberFunctionDeclaration(
                    fwdFuncName,
                    returnType,
                    isSgFunctionParameterList(
                        copyStatement(funcDecl->get_parameterList())),
                    funcScope);

    SgName rvsFuncName = funcName + "_reverse";
    SgFunctionDeclaration* rvsFuncDecl = buildDefiningMemberFunctionDeclaration(
                    rvsFuncName,
                    returnType,
                    isSgFunctionParameterList(
                        copyStatement(funcDecl->get_parameterList())),
                    funcScope);

    SgName cmtFuncName = funcName + "_commit";
    SgFunctionDeclaration* cmtFuncDecl = buildDefiningMemberFunctionDeclaration(
                    cmtFuncName,
                    returnType,
                    buildFunctionParameterList(),
                    funcScope);

    fwdFuncDecl->get_functionModifier() = funcDecl->get_functionModifier();
    rvsFuncDecl->get_functionModifier() = funcDecl->get_functionModifier();
    cmtFuncDecl->get_functionModifier() = funcDecl->get_functionModifier();
    
    fwdFuncDecl->get_functionModifier().unsetPureVirtual();
    rvsFuncDecl->get_functionModifier().unsetPureVirtual();
    cmtFuncDecl->get_functionModifier().unsetPureVirtual();
    
    //fwdFuncDecl->get_definition()->

    SgStatement* firstFuncDecl = funcDecl;
    //SgStatement* firstFuncDecl = funcDecl->get_firstNondefiningDeclaration();
    //insertStatementAfter(firstFuncDecl, cmtFuncDecl);
    insertStatementAfter(firstFuncDecl, rvsFuncDecl);
    insertStatementAfter(firstFuncDecl, fwdFuncDecl);
}

void buildThreeFuncDecl(SgFunctionDeclaration* funcDecl)
{
    SgName funcName = funcDecl->get_name();
    SgScopeStatement* funcScope = funcDecl->get_scope(); 
    SgType* returnType = funcDecl->get_orig_return_type();
    
    SgName fwdFuncName = funcName + "_forward";
    SgFunctionDeclaration* fwdFuncDecl = buildNondefiningMemberFunctionDeclaration(
                    fwdFuncName,
                    returnType,
                    isSgFunctionParameterList(
                        copyStatement(funcDecl->get_parameterList())),
                    funcScope);

    SgName rvsFuncName = funcName + "_reverse";
    SgFunctionDeclaration* rvsFuncDecl = buildNondefiningMemberFunctionDeclaration(
                    rvsFuncName,
                    returnType,
                    isSgFunctionParameterList(
                        copyStatement(funcDecl->get_parameterList())),
                    funcScope);

    SgName cmtFuncName = funcName + "_commit";
    SgFunctionDeclaration* cmtFuncDecl = buildNondefiningMemberFunctionDeclaration(
                    cmtFuncName,
                    returnType,
                    buildFunctionParameterList(),
                    funcScope);

    fwdFuncDecl->get_functionModifier() = funcDecl->get_functionModifier();
    rvsFuncDecl->get_functionModifier() = funcDecl->get_functionModifier();
    cmtFuncDecl->get_functionModifier() = funcDecl->get_functionModifier();

    SgStatement* firstFuncDecl = funcDecl;
    //SgStatement* firstFuncDecl = funcDecl->get_firstNondefiningDeclaration();
    //insertStatementAfter(firstFuncDecl, cmtFuncDecl);
    insertStatementAfter(firstFuncDecl, rvsFuncDecl);
    insertStatementAfter(firstFuncDecl, fwdFuncDecl);
}

int main(int argc, char** argv)
{
    ifstream headerIS(argv[1]);
    
    vector<pair<string, string> > headersAndFunctions;
    set<string> headers;
    set<string> functions;
    
    string headerName, funcName;
    while (headerIS >> headerName >> funcName)
    {
        headersAndFunctions.push_back(make_pair(headerName, funcName));
        headers.insert(headerName);
        functions.insert(string(funcName.begin() + 1, funcName.end() - 1));
        
        cout << headerName << " : " << funcName << "\n";
    }
    
    vector<string> args(1, "X");
    
    // Copy the header file as a source file then do the transformation then copy it back.
    foreach (const string& header, headers)
    {
        string srcHeader = header + ".cc";
        boost::filesystem::copy_file(header, srcHeader,
                boost::filesystem::copy_option::overwrite_if_exists);
        
        args.push_back(srcHeader);
        cout << srcHeader << endl;
    }
    
    args.push_back("-I/Users/hou1/MyWork/GTNetS/SRC_original");
    args.push_back("-DNO_DEBUG");
    
    SgProject* project = frontend(args);
     
    set<SgFunctionDefinition*> funcDefs;
    
    vector<SgFunctionDeclaration*> functionDecls = 
                BackstrokeUtility::querySubTree<SgFunctionDeclaration>(project);
    foreach (SgFunctionDeclaration* decl, functionDecls)
    {
        string funcName = decl->get_name();
        if (functions.count(funcName) == 0)
            continue;
        SgFunctionDefinition* funcDef = isSgFunctionDefinition(decl->get_definition());
        if (funcDef)
            funcDefs.insert(funcDef);
        else
        {
            if (decl->get_functionModifier().isPureVirtual())
                buildThreeFuncDeclWithEmptyBody(decl);
            else if (!decl->get_specialFunctionModifier().isDestructor()
                    && !decl->get_specialFunctionModifier().isConstructor())
                buildThreeFuncDecl(decl);
        }
    }
    
    Backstroke::reverseFunctions(funcDefs);
    
    project->unparse();
    
    // Then copy the source like headers files back.
    foreach (const string& header, headers)
    {
        string tempFileName1 = header + ".cc";
        
        path p(tempFileName1);
        string dir = p.parent_path().string();
        string name = path(header).filename().string();
        
        string tempFileName2 = "rose_" + p.filename().string();
        
        boost::filesystem::copy_file(tempFileName2, dir + "/../SRC/" + name,
                boost::filesystem::copy_option::overwrite_if_exists);
      
#if 1
        if (exists(tempFileName1))
            remove(tempFileName1);
        if (exists(tempFileName2))
            remove(tempFileName2);
#endif
    }
}
