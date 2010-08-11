#include "eventProcessor.h"
#include <boost/lexical_cast.hpp>
#include <utilities/Utilities.h>

#include <VariableRenaming.h>

#include <utilities/CPPDefinesAndNamespaces.h>


using namespace SageInterface;
using namespace SageBuilder;

SgExpression* ProcessorBase::pushVal(SgExpression* exp, SgType* type)
{
    return event_processor_->pushVal(exp, type);
}

SgExpression* ProcessorBase::popVal(SgType* type)
{
    return event_processor_->popVal(type);
}

InstrumentedExpressionVec ProcessorBase::processExpression(const ExpressionPackage& exp_pkg)
{
    return event_processor_->processExpression(exp_pkg);
}

InstrumentedStatementVec ProcessorBase::processStatement(const StatementPackage& stmt_pkg)
{
    return event_processor_->processStatement(stmt_pkg);
}

bool ProcessorBase::isStateVariable(SgExpression* exp)
{
    return event_processor_->isStateVariable(exp);
}

InstrumentedExpressionVec EventProcessor::processExpression(const ExpressionPackage& exp_pkg)
{
    InstrumentedExpressionVec output;

    // If two results have the same variable table, we remove the one which has the higher cost.
    
    foreach (ExpressionProcessor* exp_processor, exp_processors_)
    {
        InstrumentedExpressionVec result = exp_processor->process(exp_pkg);

        foreach (const InstrumentedExpression& exp1, result)
        {
            bool discard = false;
            for (size_t i = 0; i < output.size(); ++i)
            {
                InstrumentedExpression& exp2 = output[i];
                if (exp1.var_table == exp2.var_table) 
                {
                    if (exp1.cost > exp2.cost)
                    {
                        discard = true;
                        deepDelete(exp1.fwd_exp);
                        deepDelete(exp1.rvs_exp);
                        break;
                    }
                    else if (exp1.cost < exp2.cost)
                    {
                        deepDelete(exp2.fwd_exp);
                        deepDelete(exp2.rvs_exp);
                        output.erase(output.begin() + i);
                        --i;
                    }
                }
            }

            if (!discard)
                output.push_back(exp1);
        }
        //output.insert(output.end(), result.begin(), result.end());
    }
    return output;
}

InstrumentedStatementVec EventProcessor::processStatement(const StatementPackage& stmt_pkg)
{
    InstrumentedStatementVec output;

    // Here we update the variable version table, remove those which are not used any more.
    // Note that we process statements in reverse order in any scope, a variable is not used means
    // it is not used in statements above this point. For example,
    //  1   int t = 0;
    //  2   a = b;
    //  3   c = t;
    // after processing statement 3, we can remove t from the variable version table because it's not
    // useful for our transformation anymore.

    vector<SgExpression*> vars = VariableVersionTable::getAllVariables(stmt_pkg.stmt);
    vector<SgExpression*> vars_to_remove;
#if 0
    foreach (SgExpression* var, vars)
    {
        if (stmt_pkg.var_table.isUsingFirstUse(var))
            vars_to_remove.push_back(var);
    }
#endif

    foreach (StatementProcessor* stmt_processor, stmt_processors_)
    {
        InstrumentedStatementVec result = stmt_processor->process(stmt_pkg);
        foreach (InstrumentedStatement& stmt1, result)
        {
            // Remove those variables from variable version table if they are not useful anymore.
            foreach (SgExpression* var, vars_to_remove)
            {
                stmt1.var_table.removeVariable(var);
            }

            // If two results have the same variable table, we remove the one which has the higher cost.

            bool discard = false;
            for (size_t i = 0; i < output.size(); ++i)
            {
                InstrumentedStatement& stmt2 = output[i];
                if (stmt1.var_table == stmt2.var_table) 
                {
                    if (stmt1.cost > stmt2.cost)
                    {
                        discard = true;
                        deepDelete(stmt1.fwd_stmt);
                        deepDelete(stmt1.rvs_stmt);
                        break;
                    }
                    else if (stmt1.cost < stmt2.cost)
                    {
                        deepDelete(stmt2.fwd_stmt);
                        deepDelete(stmt2.rvs_stmt);
                        output.erase(output.begin() + i);
                        --i;
                    }
                }
            }

            if (!discard)
                output.push_back(stmt1);
        }
        //output.insert(output.end(), result.begin(), result.end());
    }
    return output;
}

SgExpression* EventProcessor::getStackVar(SgType* type)
{
    string type_name;

    if (isSgTypeInt(type))
        type_name = "int";
    if (isSgTypeBool(type))
        type_name = "bool";
    if (isSgTypeFloat(type))
        type_name = "float";

    string stack_name = event_->get_name() + "_" + type_name + "_stack";
    if (stack_decls_.count(stack_name) == 0)
    {
        SgType* stack_type = buildStructDeclaration("std::stack<" + type_name + ">")->get_type();
        ROSE_ASSERT(stack_type);
        stack_decls_[stack_name] = buildVariableDeclaration(stack_name, stack_type);
    }

    return buildVarRefExp(stack_decls_[stack_name]->get_variables()[0]);
}

bool EventProcessor::isStateVariable(SgExpression* exp)
{
    // First, get the most lhs operand, which may be the model object.
    while (isSgBinaryOp(exp))
        exp = isSgBinaryOp(exp)->get_lhs_operand();

    SgVarRefExp* var = isSgVarRefExp(exp);
    ROSE_ASSERT(var);

    foreach (SgInitializedName* name, event_->get_args())
    {
        if (name == var->get_symbol()->get_declaration())
            if (isSgPointerType(name->get_type()) || isReferenceType(name->get_type()))
                return true;
    }

    return false;
}

std::vector<SgVariableDeclaration*> EventProcessor::getAllStackDeclarations() const
{
    vector<SgVariableDeclaration*> output;
    typedef std::pair<std::string, SgVariableDeclaration*> pair_t;
    foreach (const pair_t& decl_pair, stack_decls_)
        output.push_back(decl_pair.second);
    return output;
}

SgExpression* EventProcessor::pushVal(SgExpression* exp, SgType* type)
{
    return buildFunctionCallExp("push", type, buildExprListExp(
                getStackVar(type), exp));
}

SgExpression* EventProcessor::popVal(SgType* type)
{
    return buildFunctionCallExp("pop", type,
            buildExprListExp(getStackVar(type)));
}

FuncDeclPairs EventProcessor::processEvent()
{
    // Before processing, build a variable version table for the event function.
    VariableVersionTable var_table(event_, var_renaming_);

    SgBasicBlock* body = 
            isSgFunctionDeclaration(event_->get_definingDeclaration())->get_definition()->get_body();
    FuncDeclPairs outputs;

    SimpleCostModel cost_model;
    InstrumentedStatementVec bodies = processStatement(StatementPackage(body, var_table));

    
    static int ctr = 0;
    // Sort the generated bodies so that those with the least cost appears first.
    sort(bodies.begin(), bodies.end());

    foreach (InstrumentedStatement& stmt_obj, bodies)
    {
        fixVariableReferences(stmt_obj.fwd_stmt);
        fixVariableReferences(stmt_obj.rvs_stmt);

        string ctr_str = lexical_cast<string > (ctr++);

        SgName fwd_func_name = event_->get_name() + "_forward" + ctr_str;
        SgFunctionDeclaration* fwd_func_decl =
                buildDefiningFunctionDeclaration(
                    fwd_func_name, event_->get_orig_return_type(),
                    isSgFunctionParameterList(copyStatement(event_->get_parameterList())));
        SgFunctionDefinition* fwd_func_def = fwd_func_decl->get_definition();
        SageInterface::replaceStatement(fwd_func_def->get_body(), isSgBasicBlock(stmt_obj.fwd_stmt));

        SgName rvs_func_name = event_->get_name() + "_reverse" + ctr_str;
        SgFunctionDeclaration* rvs_func_decl =
                buildDefiningFunctionDeclaration(
                    rvs_func_name, event_->get_orig_return_type(),
                    isSgFunctionParameterList(copyStatement(event_->get_parameterList())));
        SgFunctionDefinition* rvs_func_def = rvs_func_decl->get_definition();
        SageInterface::replaceStatement(rvs_func_def->get_body(), isSgBasicBlock(stmt_obj.rvs_stmt));


        // Add the cost information as comments to generated functions.
        string comment = "Cost: " + lexical_cast<string>(stmt_obj.cost.getCost());
        attachComment(fwd_func_decl, comment);
        attachComment(rvs_func_decl, comment);


        outputs.push_back(FuncDeclPair(fwd_func_decl, rvs_func_decl));
    }

    return outputs;
}

