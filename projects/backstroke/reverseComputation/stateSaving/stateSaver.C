#include "utilities.h"

#include <rose.h>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/tuple/tuple.hpp>

using namespace std;
using namespace boost;
using namespace SageInterface;
using namespace SageBuilder;

#define foreach BOOST_FOREACH


/* For a scalar type, it can be modified by assignment and incrementation/decrementation.
 * For a class type, it can be modified by assignment or it's own member function calls.
 *
 *
 */


class StateSaver
{
    string event_name_;
    SgFunctionDeclaration* event_;
    SgInitializedName* model_obj_;
    SgClassDeclaration* storage_decl_;
    string storage_name_;
    SgVariableDeclaration* storage_obj_;
    SgFunctionDeclaration* state_saving_func_;
    SgFunctionDeclaration* rvs_event_;

public:
    StateSaver(SgFunctionDeclaration* func, SgInitializedName* model)
        : event_(func), model_obj_(model)
    {}

    tuple<SgClassDeclaration*,
        SgVariableDeclaration*,
        SgFunctionDeclaration*,
        SgFunctionDeclaration*> 
    getOutput() const 
    { return make_tuple(storage_decl_, storage_obj_, state_saving_func_, rvs_event_); }

    typedef bool (*IsStateFunc)(SgExpression*);

    vector<SgExpression*> getAllModifiedVars()//IsStateFunc pred)
    {
        vector<SgExpression*> modified_vars;

        Rose_STL_Container<SgNode*> exp_list = NodeQuery::querySubTree(event_->get_definition(), V_SgExpression);
        foreach (SgNode* node, exp_list)
        {
            SgExpression* candidate = NULL;

            if (isSgPlusPlusOp(node) || isSgMinusMinusOp(node))
            {
                candidate = isSgUnaryOp(node)->get_operand();
            }

            else if(isSgAssignOp(node) ||
                    isSgPlusAssignOp(node) ||
                    isSgMinusAssignOp(node) ||
                    isSgMultAssignOp(node) ||
                    isSgDivAssignOp(node) ||
                    isSgModAssignOp(node) ||
                    isSgIorAssignOp(node) ||
                    isSgAndAssignOp(node) ||
                    isSgXorAssignOp(node) ||
                    isSgLshiftAssignOp(node) ||
                    isSgRshiftAssignOp(node))
            {
                candidate = isSgBinaryOp(node)->get_lhs_operand();
            }

            if (SgFunctionCallExp* func_call = isSgFunctionCallExp(node))
            {
                SgExpressionPtrList exp_list = func_call->get_args()->get_expressions();
                SgInitializedNamePtrList init_name_list = func_call->getAssociatedFunctionDeclaration()->get_args();
                for (size_t i = 0; i < exp_list.size(); ++i)
                {
                    SgExpression* par = exp_list[i];
                    //if (pred(par))// && isPointerType(init_name_list[i]->get_type()))
                    {
                    }
                }
            }

            // Find the real variable which is modified in case of (a, b) = 1 or a[0] = 1.
            while(1)
            {
                if (SgPntrArrRefExp* arr_ref = isSgPntrArrRefExp(candidate))
                {
                    candidate = arr_ref->get_lhs_operand();
                    continue;
                }
                if (SgCommaOpExp* comma_op = isSgCommaOpExp(candidate))
                {
                    candidate = comma_op->get_lhs_operand();
                    continue;
                }
                break;
            }

            if (candidate)// && pred(candidate))
            {
                bool is_new = true;
                foreach (SgExpression* var, modified_vars)
                {
                    if (areSameVariable(var, candidate))
                    {
                        is_new = false;
                        break;
                    }
                }
                if (is_new)
                    modified_vars.push_back(candidate);
            }
        }

        return modified_vars;
    }

    void buildStorage()
    {
        string event_name = event_->get_name();
        storage_decl_ = buildStructDeclaration(event_name + "_storage");
        SgClassDefinition* storage_def = buildClassDefinition(storage_decl_);

        foreach (SgExpression* var, getAllModifiedVars())
        {
            if (SgVarRefExp* var_ref = isSgVarRefExp(var))
            {
                SgVariableDeclaration* var_decl = buildVariableDeclaration(
                        var_ref->get_symbol()->get_name(),
                        var_ref->get_symbol()->get_type());
                appendStatement(var_decl, storage_def);
            }
            else if (SgArrowExp* arrow_op = isSgArrowExp(var))
            {
                if (SgVarRefExp* var_ref = isSgVarRefExp(arrow_op->get_rhs_operand()))
                {
                    SgVariableDeclaration* var_decl = buildVariableDeclaration(
                            var_ref->get_symbol()->get_name(),
                            var_ref->get_symbol()->get_type());
                    appendStatement(var_decl, storage_def);
                }
            }
        }

        storage_name_ = storage_decl_->get_name() + "_obj";
        storage_obj_ = buildVariableDeclaration(
                storage_name_,
                //storage_decl_->get_type());
                isSgPointerType(model_obj_->get_type())->get_base_type());
    }

    void buildStateSavingFunction()
    {
#if 1
        string state_saving_func_name = event_->get_name() + "_forward";
        state_saving_func_ = 
            buildDefiningFunctionDeclaration(state_saving_func_name, event_->get_orig_return_type(), 
                    isSgFunctionParameterList(copyStatement(event_->get_parameterList())));
        SgFunctionDefinition* state_saving_func_def = state_saving_func_->get_definition();
        //SgBasicBlock* fwd_body = isSgBasicBlock(copyStatement(event_->get_definition()->get_body()));

        SgAssignOp* copy_state = buildBinaryExpression<SgAssignOp>(
                buildVarRefExp(storage_name_),
                buildUnaryExpression<SgPointerDerefExp>(
                    buildVarRefExp(model_obj_)));
        SgBasicBlock* fwd_body = buildBasicBlock(buildExprStatement(copy_state));

        foreach (SgStatement* stmt, event_->get_definition()->get_body()->get_statements())
            fwd_body->append_statement(copyStatement(stmt));

        state_saving_func_def->set_body(fwd_body);
        fwd_body->set_parent(state_saving_func_def);
#endif
    }

    void buildReverseEvent()
    {
        string rvs_func_name = event_->get_name() + "_reverse";
        rvs_event_ = 
            buildDefiningFunctionDeclaration(rvs_func_name, event_->get_orig_return_type(), 
                    isSgFunctionParameterList(copyStatement(event_->get_parameterList())));
        SgFunctionDefinition* rvs_event_def = rvs_event_->get_definition();
        //SgBasicBlock* fwd_body = isSgBasicBlock(copyStatement(event_->get_definition()->get_body()));

        SgAssignOp* restore_state = buildBinaryExpression<SgAssignOp>(
                buildUnaryExpression<SgPointerDerefExp>(
                    buildVarRefExp(model_obj_)),
                buildVarRefExp(storage_name_));
        SgBasicBlock* rvs_body = buildBasicBlock(buildExprStatement(restore_state));

        rvs_event_def->set_body(rvs_body);
        rvs_body->set_parent(rvs_event_def);
    }



    void storeVariable(SgExpression* var)
    {
        SgType* var_type = var->get_type();
    }
};

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
        if (!istarts_with(func_name, "event") ||
                iends_with(func_name, "reverse") ||
                iends_with(func_name, "forward"))
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
    }

    // Get the model structure type which will be used in other functions, like initialization.
    if (SgClassDeclaration* model_decl = isSgClassDeclaration(n))
    {
        //if (model_decl->get_qualified_name() == "model")
        model_type = model_decl->get_type();
    }
#endif
}
}


int main( int argc, char * argv[] )
{
    vector<string> args(argv, argv+argc);
    SgProject* project = frontend(args);

    

    SgGlobal *globalScope = getFirstGlobalScope(project);
    reverserTraversal reverser(isSgScopeStatement(globalScope));
    reverser.traverseInputFiles(project,preorder);

    //pushScopeStack(isSgScopeStatement(globalScope));

    for (size_t i = 0; i < reverser.output.size(); ++i)
    {
        //appendStatement(reverser.output[i].get<0>());
        break;
        //appendStatement(reverser.output[i].get<1>());
    }
        
    
    //popScopeStack();

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

