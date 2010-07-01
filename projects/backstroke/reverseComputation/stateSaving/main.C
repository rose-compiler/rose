#include "stateSaver.h"
#include "../facilityBuilder.h"
#include <boost/tuple/tuple.hpp>
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace boost;
using namespace SageInterface;
using namespace SageBuilder;

#define foreach BOOST_FOREACH

bool isState(SgExpression*)
{
    return true;
}

class reverserTraversal : public AstSimpleProcessing
{
    public:
        reverserTraversal() 
            : AstSimpleProcessing(),
            events_num(0),  
            model_type(0)
    {}
        reverserTraversal(SgScopeStatement* s)
            : scope(s) {}

        virtual void visit(SgNode* n);

        int events_num;
        SgScopeStatement* scope;
        SgClassType* model_type;
        vector<SgFunctionDeclaration*> funcs_gen;
        vector<SgFunctionDeclaration*> all_funcs;
        vector<SgStatement*> var_decls;
        vector<SgStatement*> var_inits;
        vector<string> event_names;
        vector<SgClassDefinition*> storages; 
        vector<tuple<
            SgClassDefinition*,
            SgVariableDeclaration*,
            SgFunctionDefinition*,
            SgFunctionDefinition*> > output;

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

        pushScopeStack(scope);

        StateSaver state_saver(func_decl, func_decl->get_args()[0]);
        state_saver.buildStorage();
        state_saver.buildStateSavingFunction();
        state_saver.buildReverseEvent();

        appendStatement(state_saver.getOutput().get<0>());
        appendStatement(state_saver.getOutput().get<1>());
        appendStatement(state_saver.getOutput().get<2>());
        appendStatement(state_saver.getOutput().get<3>());

        popScopeStack();

#if 0
        vector<FuncDeclPair> func_pairs = reverser.outputFunctions();
        foreach (const FuncDeclPair& func_pair, func_pairs)
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
#endif
    }

    // Get the model structure type which will be used in other functions, like initialization.
    if (SgClassDeclaration* model_decl = isSgClassDeclaration(n))
        if (model_decl->get_qualified_name() == "::model")
            model_type = model_decl->get_type();
}


int main( int argc, char * argv[] )
{
    vector<string> args(argv, argv+argc);
    SgProject* project = frontend(args);



    SgGlobal *globalScope = getFirstGlobalScope(project);
    reverserTraversal reverser(isSgScopeStatement(globalScope));
    reverser.traverseInputFiles(project,preorder);

    pushScopeStack(isSgScopeStatement(globalScope));

    for (size_t i = 0; i < reverser.output.size(); ++i)
    {
        //appendStatement(reverser.output[i].get<0>());
        break;
        //appendStatement(reverser.output[i].get<1>());
    }

    ROSE_ASSERT(reverser.model_type);
    appendStatement(buildInitializationFunction(reverser.model_type));
    appendStatement(buildCompareFunction(reverser.model_type));
    appendStatement(buildMainFunction("event0"));

    popScopeStack();

    return backend(project);
#if 0
    ROSE_ASSERT(reverser.model_type);

    //SgStatement* init_func = buildInitializationFunction();


    for (size_t i = 0; i < reverser.var_decls.size(); ++i)
        prependStatement(reverser.var_decls[i]);
    for (size_t i = 0; i < reverser.funcs_gen.size(); ++i)
        insertFunctionInPlace(reverser.funcs_gen[i], reverser.all_funcs);
    //appendStatement(reverser.funcs[i]);

    //appendStatement(buildInitializationFunction(reverser.model_type));
    //appendStatement(buildCompareFunction(reverser.model_type));
    //appendStatement(buildMainFunction(reverser.var_inits, reverser.event_names, klee));


    popScopeStack();

    // Write or find a function to clear all nodes in memory pool who don't have parents.
#if 1
    cout << "Start to fix variables references\n";
    fixVariableReferences2(globalScope);
    cout << "Fix finished\n";

    //AstTests::runAllTests(project);
#endif
    return backend(project);

#endif
}

