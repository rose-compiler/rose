#include "stateSaver.h"
#include "utilities/Utilities.h"

#include <rose.h>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/tuple/tuple.hpp>

using namespace std;
using namespace boost;
using namespace SageInterface;
using namespace SageBuilder;

#define foreach BOOST_FOREACH

/*
 * For a scalar type, it can be modified by assignment and incrementation/decrementation.
 * For a class type, it can be modified by assignment or it's own member function calls.
 */

vector<SgExpression*> StateSaver::getAllModifiedVars()//IsStateFunc pred)
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
                if (backstroke_util::areSameVariable(var, candidate))
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

void StateSaver::buildStorage()
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

void StateSaver::buildStateSavingFunction()
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

void StateSaver::buildReverseEvent()
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




