#include "eventProcessor.h"
#include <boost/lexical_cast.hpp>
#include <utilities/Utilities.h>

#include <VariableRenaming.h>

#include <utilities/CPPDefinesAndNamespaces.h>


using namespace SageInterface;
using namespace SageBuilder;

ExpressionObjectVec ExpressionProcessor::processExpression(SgExpression* exp, const VariableVersionTable& var_table)
{
    return event_processor_->processExpression(exp, var_table);
}

SgExpression* ExpressionProcessor::pushVal(SgExpression* exp, SgType* type)
{
    return event_processor_->pushVal(exp, type);
}

SgExpression* ExpressionProcessor::popVal(SgType* type)
{
    return event_processor_->popVal(type);
}

ExpressionObjectVec StatementProcessor::processExpression(SgExpression* exp, const VariableVersionTable& var_table)
{
    return event_processor_->processExpression(exp, var_table);
}

StatementObjectVec StatementProcessor::processStatement(SgStatement* stmt, const VariableVersionTable& var_table)
{
    return event_processor_->processStatement(stmt, var_table);
}

SgExpression* StatementProcessor::pushVal(SgExpression* exp, SgType* type)
{
    return event_processor_->pushVal(exp, type);
}

SgExpression* StatementProcessor::popVal(SgType* type)
{
    return event_processor_->popVal(type);
}


ExpressionObjectVec EventProcessor::processExpression(SgExpression* exp, const VariableVersionTable& var_table)
{
    ExpressionObjectVec output;

    foreach (ExpressionProcessor* exp_processor, exp_processors_)
    {
        ExpressionObjectVec result = exp_processor->process(exp, var_table);
        output.insert(output.end(), result.begin(), result.end());
    }
    return output;
}

StatementObjectVec EventProcessor::processStatement(SgStatement* stmt, const VariableVersionTable& var_table)
{
    StatementObjectVec output;

    foreach (StatementProcessor* stmt_processor, stmt_processors_)
    {
        StatementObjectVec result = stmt_processor->process(stmt, var_table);
        output.insert(output.end(), result.begin(), result.end());
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
    return buildFunctionCallExp(
            "push", type,
            buildExprListExp(
                getStackVar(type),
                copyExpression(exp)));
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

    static int ctr = 0;

    StatementObjectVec bodies = processStatement(body, var_table);

    foreach (StatementObject& stmt_obj, bodies)
    {
        string ctr_str = lexical_cast<string > (ctr++);

        SgName fwd_func_name = event_->get_name() + "_forward" + ctr_str;
        SgFunctionDeclaration* fwd_func_decl =
                buildDefiningFunctionDeclaration(
                    fwd_func_name, event_->get_orig_return_type(),
                    isSgFunctionParameterList(copyStatement(event_->get_parameterList())));
        SgFunctionDefinition* fwd_func_def = fwd_func_decl->get_definition();
        fwd_func_def->set_body(isSgBasicBlock(stmt_obj.fwd_stmt));
        stmt_obj.fwd_stmt->set_parent(fwd_func_def);

        SgName rvs_func_name = event_->get_name() + "_reverse" + ctr_str;
        SgFunctionDeclaration* rvs_func_decl =
                buildDefiningFunctionDeclaration(
                    rvs_func_name, event_->get_orig_return_type(),
                    isSgFunctionParameterList(copyStatement(event_->get_parameterList())));
        SgFunctionDefinition* rvs_func_def = rvs_func_decl->get_definition();
        rvs_func_def->set_body(isSgBasicBlock(stmt_obj.rvs_stmt));
        stmt_obj.rvs_stmt->set_parent(rvs_func_def);

        outputs.push_back(FuncDeclPair(fwd_func_decl, rvs_func_decl));
    }

    return outputs;
}

