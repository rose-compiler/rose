#include "processorPool.h"
#include "statementHandler.h"
#include <boost/tuple/tuple.hpp>
#include <boost/lexical_cast.hpp>
#include "utilities/CPPDefinesAndNamespaces.h"

using namespace SageInterface;
using namespace SageBuilder;

vector<StmtPair> processBasicStatement(SgStatement* stmt)
{
    if (SgExprStatement* exp_stmt = isSgExprStatement(stmt))
        return processExprStatement(exp_stmt);

    if (SgVariableDeclaration* var_decl = isSgVariableDeclaration(stmt))
        return processVariableDeclaration(var_decl);

    if (SgBasicBlock* block = isSgBasicBlock(stmt))
        return processBasicBlock(block);

    return vector<StmtPair>();
}

vector<StmtPair> processFunctionDeclaration(SgFunctionDeclaration* func_decl)
{
    SgBasicBlock* body = func_decl->get_definition()->get_body();
    vector<StmtPair> bodies = processStatement(body);
    vector<StmtPair> outputs;

    static int ctr = 0;

    foreach (StmtPair stmt_pair, bodies)
    {
        SgStatement *fwd_body, *rvs_body;
        tie(fwd_body, rvs_body) = stmt_pair;

        string ctr_str = lexical_cast<string>(ctr++);

        SgName fwd_func_name = func_decl->get_name() + "_forward" + ctr_str;
        SgFunctionDeclaration* fwd_func_decl = 
            buildDefiningFunctionDeclaration(fwd_func_name, func_decl->get_orig_return_type(), 
                    isSgFunctionParameterList(copyStatement(func_decl->get_parameterList())));
        SgFunctionDefinition* fwd_func_def = fwd_func_decl->get_definition();
        fwd_func_def->set_body(isSgBasicBlock(fwd_body));
        fwd_body->set_parent(fwd_func_def);

        SgName rvs_func_name = func_decl->get_name() + "_reverse" + ctr_str;
        SgFunctionDeclaration* rvs_func_decl = 
            buildDefiningFunctionDeclaration(rvs_func_name, func_decl->get_orig_return_type(), 
                    isSgFunctionParameterList(copyStatement(func_decl->get_parameterList()))); 
        SgFunctionDefinition* rvs_func_def = rvs_func_decl->get_definition();
        rvs_func_def->set_body(isSgBasicBlock(rvs_body));
        rvs_body->set_parent(rvs_func_def);

        outputs.push_back(StmtPair(fwd_func_decl, rvs_func_decl));
    }

    return outputs;
}

vector<StmtPair> processExprStatement(SgExprStatement* exp_stmt)
{
    vector<ExpPair> exps = processExpression(exp_stmt->get_expression());
    vector<StmtPair> stmts;
    foreach(ExpPair exp_pair, exps)
    {
        SgExpression *fwd_exp, *rvs_exp;
        SgStatement *fwd_stmt = NULL, *rvs_stmt = NULL;

        tie(fwd_exp, rvs_exp) = exp_pair;
        if (fwd_exp)
            fwd_stmt = buildExprStatement(fwd_exp);
        if (rvs_exp)
            rvs_stmt = buildExprStatement(rvs_exp);
        stmts.push_back(StmtPair(fwd_stmt, rvs_stmt));
    }
    return stmts;
}

vector<StmtPair> processVariableDeclaration(SgVariableDeclaration* var_decl)
{
    vector<StmtPair> outputs;
    //outputs.push_back(pushAndPopLocalVar(var_decl));

    // FIXME  other cases
    
    return outputs;
}

vector<StmtPair> processBasicBlock(SgBasicBlock* body)
{
    vector<vector<StmtPair> > all_stmts;
    vector<StmtPair> outputs;

    foreach(SgStatement* s, body->get_statements())
        all_stmts.push_back(processStatement(s));

    struct Index
    {
        vector<int> index;
        vector<int> index_max;

        bool forward()
        {
            for (int i = index.size()-1; i >= 0; --i)
            {
                if (index[i] < index_max[i])
                {
                    ++index[i];
                    return false;
                }
                else
                    index[i] = 0;
            }
            return true;
        }
    };

    Index idx;
    // Initialize the index.
    size_t size = all_stmts.size();
    idx.index = vector<int>(size, 0);
    idx.index_max.resize(size);
    for (size_t i = 0; i < size; ++i)
        idx.index_max[i] = all_stmts[i].size() - 1;

    do
    {
        SgBasicBlock* fwd_body = buildBasicBlock();
        SgBasicBlock* rvs_body = buildBasicBlock();

        for (size_t i = 0; i < idx.index.size(); ++i)
        {
            // In case that the size is 0.
            if (all_stmts[i].empty())
                continue;

            SgStatement *fwd_stmt, *rvs_stmt;

            ROSE_ASSERT(i < all_stmts.size());
            ROSE_ASSERT(static_cast<size_t>(idx.index[i]) < all_stmts[i].size());

            tie(fwd_stmt, rvs_stmt) = all_stmts[i][idx.index[i]];

            if (fwd_stmt)
            {
                ROSE_ASSERT(isSgStatement(fwd_stmt));
                fwd_body->append_statement(fwd_stmt);
            }
            if (rvs_stmt)
            {
                ROSE_ASSERT(isSgStatement(rvs_stmt));
                rvs_body->prepend_statement(rvs_stmt);
            }
        }

        // Check if the combination is valid based on SSA.
        // FIXME
        //if (checkValidity(rvs_body))
            outputs.push_back(StmtPair(fwd_body, rvs_body));
    } 
    while (!idx.forward());

    return outputs;
}
