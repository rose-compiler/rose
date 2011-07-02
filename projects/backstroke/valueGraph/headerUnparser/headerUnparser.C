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


int main(int argc, char** argv)
{
    set<vector<string> > functionsToReverse;
    readFunctionInfo("funcList.txt", functionsToReverse);

    
    ifstream headerIS(argv[1]);
    
    set<string> headers;
    
    string headerName;
    while (headerIS >> headerName)
    {
        headers.insert(headerName);
        cout << headerName << "\n";
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
    
    set<SgFunctionDeclaration*> processedFuncDecls;
    vector<SgFunctionDeclaration*> functionDecls = 
                BackstrokeUtility::querySubTree<SgFunctionDeclaration>(project);
    foreach (SgFunctionDeclaration* decl, functionDecls)
    {
        SgFunctionDeclaration* firstDecl = 
                isSgFunctionDeclaration(decl->get_firstNondefiningDeclaration());
        if (!firstDecl)
            firstDecl = decl;
        
        if (processedFuncDecls.count(firstDecl))
            continue;
        
        if (firstDecl->get_specialFunctionModifier().isDestructor() ||
                firstDecl->get_specialFunctionModifier().isConstructor())
            continue;
        
        SgMemberFunctionDeclaration* memFuncDecl = isSgMemberFunctionDeclaration(firstDecl);
        if (!memFuncDecl)
            continue;
                
        if (!functionsToReverse.count(serializeMemberFunction(memFuncDecl)))
            continue;
        
        SgFunctionDefinition* funcDef = isSgFunctionDefinition(firstDecl->get_definition());
        if (funcDef)
            funcDefs.insert(funcDef);
        else
        {
            if (firstDecl->get_functionModifier().isPureVirtual())
                buildThreeFuncDeclWithEmptyBody(firstDecl);
            else
                buildThreeFuncDecl(firstDecl);
        }
        
        processedFuncDecls.insert(firstDecl);
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
      
        if (exists(tempFileName1))
            remove(tempFileName1);
        if (exists(tempFileName2))
            remove(tempFileName2);
    }
    
    set<string> funcReversed;
    foreach (SgFunctionDeclaration* decl, processedFuncDecls)
        funcReversed.insert(decl->get_mangled_name());
    foreach (SgFunctionDefinition* def, funcDefs)
        funcReversed.insert(def->get_declaration()->get_mangled_name());
    
    cout << "\nThere are " << funcReversed.size() << " functions reversed.\n";
}
