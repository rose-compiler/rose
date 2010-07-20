#include "eventProcessor.h"
#include <boost/lexical_cast.hpp>
#include <utilities/Utilities.h>
#include <utilities/CPPDefinesAndNamespaces.h>


using namespace SageInterface;
using namespace SageBuilder;

SgExpression* ExpressionProcessor::pushVal(SgExpression* exp, SgType* type)
{
    return event_processor_->pushVal(exp, type);
}

SgExpression* ExpressionProcessor::popVal(SgType* type)
{
    return event_processor_->popVal(type);
}

ExpPairs StatementProcessor::processExpression(SgExpression* exp)
{
    return event_processor_->processExpression(exp);
}

StmtPairs StatementProcessor::processStatement(SgStatement* stmt)
{
    return event_processor_->processStatement(stmt);
}

SgExpression* StatementProcessor::pushVal(SgExpression* exp, SgType* type)
{
    return event_processor_->pushVal(exp, type);
}

SgExpression* StatementProcessor::popVal(SgType* type)
{
    return event_processor_->popVal(type);
}


ExpPairs EventProcessor::processExpression(SgExpression* exp)
{
    ExpPairs output;

    foreach (ExpressionProcessor* exp_processor, exp_processors_)
    {
        ExpPairs result = exp_processor->process(exp);
        output.insert(output.end(), result.begin(), result.end());
    }
    return output;
}

StmtPairs EventProcessor::processStatement(SgStatement* stmt)
{
    StmtPairs output;

    foreach (StatementProcessor* stmt_processor, stmt_processors_)
    {
        StmtPairs result = stmt_processor->process(stmt);
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

std::vector<SgVariableDeclaration*> EventProcessor::getAllStackDeclarations()
{
    vector<SgVariableDeclaration*> output;
    typedef std::pair<std::string, SgVariableDeclaration*> pair_t;
    foreach (pair_t decl_pair, stack_decls_)
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
    SgBasicBlock* body = event_->get_definition()->get_body();
    FuncDeclPairs outputs;

    static int ctr = 0;

    StmtPairs bodies = processStatement(body);

    foreach (StmtPair stmt_pair, bodies)
    {
        SgStatement *fwd_body, *rvs_body;
        tie(fwd_body, rvs_body) = stmt_pair;

        string ctr_str = lexical_cast<string > (ctr++);

        SgName fwd_func_name = event_->get_name() + "_forward" + ctr_str;
        SgFunctionDeclaration* fwd_func_decl =
                SageBuilder::buildDefiningFunctionDeclaration(
                    fwd_func_name, event_->get_orig_return_type(),
                    isSgFunctionParameterList(
                        SageInterface::copyStatement(event_->get_parameterList())));
        SgFunctionDefinition* fwd_func_def = fwd_func_decl->get_definition();
        fwd_func_def->set_body(isSgBasicBlock(fwd_body));
        fwd_body->set_parent(fwd_func_def);

        SgName rvs_func_name = event_->get_name() + "_reverse" + ctr_str;
        SgFunctionDeclaration* rvs_func_decl =
                SageBuilder::buildDefiningFunctionDeclaration(
                    rvs_func_name, event_->get_orig_return_type(),
                    isSgFunctionParameterList(
                        SageInterface::copyStatement(event_->get_parameterList())));
        SgFunctionDefinition* rvs_func_def = rvs_func_decl->get_definition();
        rvs_func_def->set_body(isSgBasicBlock(rvs_body));
        rvs_body->set_parent(rvs_func_def);

        outputs.push_back(FuncDeclPair(fwd_func_decl, rvs_func_decl));
    }

    return outputs;
}

