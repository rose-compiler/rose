#include "eventReverser.h"
#include "facilityBuilder.h"
#include <boost/algorithm/string.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#define foreach BOOST_FOREACH

using namespace std;
using namespace boost;
using namespace SageBuilder;
using namespace SageInterface;

class reverserTraversal : public AstSimpleProcessing
{
    public:
        reverserTraversal(DFAnalysis* du) 
            : AstSimpleProcessing(),
            defuse(du),
            events_num(0),  
            model_type(0)
    {}
        virtual void visit(SgNode* n);

        DFAnalysis* defuse;
        int events_num;
        SgClassType* model_type;
        vector<SgFunctionDeclaration*> funcs_gen;
        vector<SgFunctionDeclaration*> all_funcs;
        vector<SgStatement*> var_decls;
        vector<SgStatement*> var_inits;
        vector<string> event_names;
};


void reverserTraversal::visit(SgNode* n)
{

    if (SgFunctionDeclaration* func_decl = isSgFunctionDeclaration(n))
    {
        all_funcs.push_back(func_decl);

        string func_name = func_decl->get_name();
        if (!starts_with(func_name, "event") ||
                ends_with(func_name, "reverse") ||
                ends_with(func_name, "forward"))
            return;

        //cout << func_name << endl;
        event_names.push_back(func_name);

        EventReverser reverser(func_decl, defuse);
        vector<FuncDeclPair> func_pairs = reverser.outputFunctions();
        foreach(const FuncDeclPair& func_pair, func_pairs)
        {
            funcs_gen.push_back(func_pair.second);
            funcs_gen.push_back(func_pair.first);
        }

        // Collect all variables needed to be declared
        vector<SgStatement*> decls = reverser.getVarDeclarations();
        vector<SgStatement*> inits = reverser.getVarInitializers();

        var_decls.insert(var_decls.end(), decls.begin(), decls.end());
        var_inits.insert(var_inits.end(), inits.begin(), inits.end());

        // increase the number of events
        ++events_num;

        /* 
           pair<SgFunctionDeclaration*, SgFunctionDeclaration*> 
           func = reverseFunction(func_decl->get_definition());
           if (func.first != NULL)
           funcs.push_back(func.first);
           if (func.second != NULL)
           funcs.push_back(func.second);
           */
    }

    // Get the model structure type which will be used in other functions, like initialization.
    if (SgClassDeclaration* model_decl = isSgClassDeclaration(n))
    {
        if (model_decl->get_name() == "model")
            model_type = model_decl->get_type();
    }
}


// Put the functions generated in place
void insertFunctionInPlace(SgFunctionDeclaration* func, const vector<SgFunctionDeclaration*>& all_funcs)
{
    string func_name = func->get_name();
    replace_last(func_name, "_forward", "");
    replace_last(func_name, "_reverse", "");

    foreach (SgFunctionDeclaration* f, all_funcs)
        if (func_name == string(f->get_name()))
            insertStatementAfter(f, func);
}



int fixVariableReferences2(SgNode* root)
{
    ROSE_ASSERT(root);
    int counter=0;
    Rose_STL_Container<SgNode*> nodeList;

    SgVarRefExp* varRef=NULL;
    Rose_STL_Container<SgNode*> reflist = NodeQuery::querySubTree(root, V_SgVarRefExp);
    for (Rose_STL_Container<SgNode*>::iterator i=reflist.begin();i!=reflist.end();i++)
    {
        //cout << get_name(isSgVarRefExp(*i)) << endl;
        varRef= isSgVarRefExp(*i);
        ROSE_ASSERT(varRef->get_symbol());
        SgInitializedName* initname= varRef->get_symbol()->get_declaration();
        //ROSE_ASSERT(initname);

        if (initname->get_type()==SgTypeUnknown::createType())
            //    if ((initname->get_scope()==NULL) && (initname->get_type()==SgTypeUnknown::createType()))
        {
            SgName varName=initname->get_name();
            SgSymbol* realSymbol = NULL;
            //cout << varName << endl;

            // CH (5/7/2010): Before searching SgVarRefExp objects, we should first deal with class/structure
            // members. Or else, it is possible that we assign the wrong symbol to those members if there is another
            // variable with the same name in parent scopes. Those members include normal member referenced using . or ->
            // operators, and static members using :: operators.
            //
            if (SgArrowExp* arrowExp = isSgArrowExp(varRef->get_parent()))
            {
                if (varRef == arrowExp->get_rhs_operand())
                {
                    // make sure the lhs operand has been fixed
                    counter += fixVariableReferences2(arrowExp->get_lhs_operand());

                    SgPointerType* ptrType = isSgPointerType(arrowExp->get_lhs_operand()->get_type());
                    ROSE_ASSERT(ptrType);
                    SgClassType* clsType = isSgClassType(ptrType->get_base_type());
                    ROSE_ASSERT(clsType);
                    SgClassDeclaration* decl = isSgClassDeclaration(clsType->get_declaration());
                    decl = isSgClassDeclaration(decl->get_definingDeclaration());
                    ROSE_ASSERT(decl);
                    realSymbol = lookupSymbolInParentScopes(varName, decl->get_definition());
                }
                else
                    realSymbol = lookupSymbolInParentScopes(varName,getScope(varRef));
            }
            else if (SgDotExp* dotExp = isSgDotExp(varRef->get_parent()))
            {
                if (varRef == dotExp->get_rhs_operand())
                {
                    // make sure the lhs operand has been fixed
                    counter += fixVariableReferences2(dotExp->get_lhs_operand());

                    SgClassType* clsType = isSgClassType(dotExp->get_lhs_operand()->get_type());
                    ROSE_ASSERT(clsType);
                    SgClassDeclaration* decl = isSgClassDeclaration(clsType->get_declaration());
                    decl = isSgClassDeclaration(decl->get_definingDeclaration());
                    ROSE_ASSERT(decl);
                    realSymbol = lookupSymbolInParentScopes(varName, decl->get_definition());
                }
                else
                    realSymbol = lookupSymbolInParentScopes(varName,getScope(varRef));
            }
            else
                realSymbol = lookupSymbolInParentScopes(varName,getScope(varRef));

            // should find a real symbol at this final fixing stage!
            // This function can be called any time, not just final fixing stage
            if (realSymbol==NULL) 
            {
                cerr<<"Error: cannot find a symbol for "<<varName.getString()<<endl;
                ROSE_ASSERT(realSymbol);
            }
            else {
                // release placeholder initname and symbol
                ROSE_ASSERT(realSymbol!=(varRef->get_symbol()));

                bool flag = false;

                SgSymbol* symbol_to_delete = varRef->get_symbol();
                varRef->set_symbol(isSgVariableSymbol(realSymbol));
                counter ++;

                if (nodeList.empty())
                {
                    VariantVector vv(V_SgVarRefExp);
                    nodeList = NodeQuery::queryMemoryPool(vv);
                }
                foreach(SgNode* node, nodeList)
                {
                    if (SgVarRefExp* var = isSgVarRefExp(node))
                    {
                        if (var->get_symbol() == symbol_to_delete)
                        {
                            flag = true;
                            break;
                        }
                    }
                }
                if (!flag)
                {
                    //cout <<initname->get_name().str() << endl;
                    delete initname; // TODO deleteTree(), release File_Info nodes etc.
                    delete symbol_to_delete;
                }
            }
        }
    } // end for
    return counter;
}

int main( int argc, char * argv[] )
{
    vector<string> args(argv, argv+argc);
    bool klee = CommandlineProcessing::isOption(args, "-backstroke:", "klee", true);
    SgProject* project = frontend(args);
    DFAnalysis* defuse = NULL;//new DefUseAnalysis(project);

    reverserTraversal reverser(defuse);

    SgGlobal *globalScope = getFirstGlobalScope(project);
    string includes = "#include \"rctypes.h\"\n"
        "#include <stdio.h>\n"
        "#include <stdlib.h>\n"
        "#include <time.h>\n"
        "#include <assert.h>\n"
        "#include <memory.h>\n";
    if (klee)
      includes += "#include <klee.h>\n";
    addTextForUnparser(globalScope,includes,AstUnparseAttribute::e_before); 

    pushScopeStack(isSgScopeStatement(globalScope));

    reverser.traverseInputFiles(project,preorder);
    cout << "Traverse complete\n";

    ROSE_ASSERT(reverser.model_type);


    for (size_t i = 0; i < reverser.var_decls.size(); ++i)
        prependStatement(reverser.var_decls[i]);
    for (size_t i = 0; i < reverser.funcs_gen.size(); ++i)
        insertFunctionInPlace(reverser.funcs_gen[i], reverser.all_funcs);

    appendStatement(buildInitializationFunction(reverser.model_type));
    appendStatement(buildCompareFunction(reverser.model_type));
    appendStatement(buildMainFunction(reverser.var_inits, reverser.event_names, klee));


    popScopeStack();

    // Write or find a function to clear all nodes in memory pool who don't have parents.
#if 1
    cout << "Start to fix variables references\n";
    
	//SageInterface::fixVariableReferences(globalScope);
	fixVariableReferences2(globalScope);
    cout << "Fix finished\n";
#endif
    AstTests::runAllTests(project);

    return backend(project);
}


