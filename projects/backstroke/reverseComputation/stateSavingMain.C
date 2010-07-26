#include "stateSaver.h"
#include "facilityBuilder.h"
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

	AstTests::runAllTests(project);
    return backend(project);
}

