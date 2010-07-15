#include "processorPool.h"
#include "statementHandler.h"
#include "storage.h"
#include <boost/tuple/tuple.hpp>
#include <boost/lexical_cast.hpp>
#include "utilities/CPPDefinesAndNamespaces.h"

using namespace SageInterface;
using namespace SageBuilder;

StmtPairs processBasicStatement(SgStatement* stmt)
{
    if (SgExprStatement* exp_stmt = isSgExprStatement(stmt))
        return processExprStatement(exp_stmt);

    if (SgVariableDeclaration* var_decl = isSgVariableDeclaration(stmt))
        return processVariableDeclaration(var_decl);

    if (SgBasicBlock* block = isSgBasicBlock(stmt))
        return processBasicBlock(block);

    return StmtPairs();
}

StmtPairs processFunctionDeclaration(SgFunctionDeclaration* func_decl)
{
    SgBasicBlock* body = func_decl->get_definition()->get_body();
    StmtPairs bodies = processStatement(body);
    StmtPairs outputs;

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

StmtPairs processExprStatement(SgExprStatement* exp_stmt)
{
    ExpPairs exps = processExpression(exp_stmt->get_expression());
    StmtPairs stmts;
    foreach (ExpPair exp_pair, exps)
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

StmtPairs processVariableDeclaration(SgVariableDeclaration* var_decl)
{
    StmtPairs outputs;

    // Note the store and restore of local variables are handled in 
    // basic block, not here. We just forward the declaration to forward
    // event function.
    outputs.push_back(StmtPair(copyStatement(var_decl), NULL));
    //outputs.push_back(pushAndPopLocalVar(var_decl));

    // FIXME  other cases
    
    return outputs;
}

StmtPairs processBasicBlock(SgBasicBlock* body)
{
    vector<StmtPairs > all_stmts;
    StmtPairs outputs;

    

    // Store all results of transformation of local variable declarations.
    vector<StmtPairs > var_decl_output;

    foreach(SgStatement* s, body->get_statements())
    {
        all_stmts.push_back(processStatement(s));

        // Here we consider to store and restore local variables.
        if (SgVariableDeclaration* var_decl = isSgVariableDeclaration(s))
        {
            const SgInitializedNamePtrList& names = var_decl->get_variables();
            ROSE_ASSERT(names.size() == 1);
            SgInitializedName* init_name = names[0];

            // Here we use a trick to make the new variable reference have the body processed
            // as its parent, which will let pushVal function get the correct stack.
            SgVarRefExp* var_stored = buildVarRefExp(init_name);
            var_stored->set_parent(body);
            // Store the value of local variables at the end of the basic block.
            SgStatement* store_var = buildExprStatement(pushVal(var_stored));

            // Retrieve the value which is used to initialize that local variable.
            SgStatement* decl_var = buildVariableDeclaration(
                    init_name->get_name(),
                    init_name->get_type(),
                    buildAssignInitializer(popVal(var_stored)));

            // Stores all transformations of a local variable declaration. 
            StmtPairs results;

            // The first transformation is store and restore it.
            results.push_back(StmtPair(store_var, decl_var));

            // The second transformation is not to store it.
            SgStatement* just_decl = buildVariableDeclaration(
                            init_name->get_name(),
                            init_name->get_type());
            results.push_back(StmtPair(NULL, just_decl));

            var_decl_output.push_back(results);
        }
    }
    // Since all store of local variable should be put at the end of the block, those 
    // transformations should also be appended at the end.
    all_stmts.insert(all_stmts.end(), var_decl_output.begin(), var_decl_output.end());

    // The following Index structure is used to traverse a vector of vectors.
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

    // List all combinations of transformed statements.
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

StmtPairs processIfStmt(SgIfStmt* if_stmt)
{
    SgStatement *fwd_true_body, *fwd_false_body;
    SgStatement *rvs_true_body, *rvs_false_body;

    SgStatement* true_body = if_stmt->get_true_body();
    SgStatement* false_body = if_stmt->get_false_body();

    // Here we have do decide whether to store the flag. We don't have to store
    // the flag if the value of that flag will not change after the if statement.
    // Otherwise, we will push the flag at the end of if statement.

    // After normalization, we require that the condition part of if statement
    // does not need to be reversed. In other word, the expression of if condition
    // does not modify any value.

}
