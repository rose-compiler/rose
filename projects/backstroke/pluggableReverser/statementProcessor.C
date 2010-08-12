#include "statementProcessor.h"
#include <boost/tuple/tuple.hpp>
#include <boost/lexical_cast.hpp>
#include "utilities/CPPDefinesAndNamespaces.h"

using namespace SageInterface;
using namespace SageBuilder;

StatementReversalVec BasicStatementProcessor::process(SgStatement* stmt, const VariableVersionTable& var_table)
{
    if (isSgExprStatement(stmt))
        return processExprStatement(stmt, var_table);

    else if (isSgVariableDeclaration(stmt))
        return processVariableDeclaration(stmt, var_table);

    else if (isSgBasicBlock(stmt))
        return processBasicBlock(stmt, var_table);

    return StatementReversalVec();
}


StatementReversalVec BasicStatementProcessor::processExprStatement(SgStatement* stmt, const VariableVersionTable& var_table)
{
    SgExprStatement* exp_stmt = isSgExprStatement(stmt);
    ROSE_ASSERT(exp_stmt);
    
    ExpressionReversalVec exps = processExpression(
            exp_stmt->get_expression(), var_table, false);

    ROSE_ASSERT(!exps.empty());

    StatementReversalVec stmts;
    foreach (ExpressionReversal& exp_obj, exps)
    {
        SgStatement *fwd_stmt = NULL, *rvs_stmt = NULL;

        if (exp_obj.fwd_exp)
            fwd_stmt = buildExprStatement(exp_obj.fwd_exp);
        if (exp_obj.rvs_exp)
            rvs_stmt = buildExprStatement(exp_obj.rvs_exp);

        // Use the variable version table output by expression processor.
        stmts.push_back(StatementReversal(fwd_stmt, rvs_stmt, exp_obj.var_table, exp_obj.cost));
    }
    return stmts;
}

StatementReversalVec BasicStatementProcessor::processVariableDeclaration(SgStatement* stmt, const VariableVersionTable& var_table)
{
    SgVariableDeclaration* var_decl = isSgVariableDeclaration(stmt);
    ROSE_ASSERT(var_decl);

    StatementReversalVec outputs;

    // Note the store and restore of local variables are processd in
    // basic block, not here. We just forward the declaration to forward
    // event function.

    // FIXME copyStatement also copies preprocessing info
    outputs.push_back(StatementReversal(copyStatement(var_decl), NULL, var_table));

    //outputs.push_back(InstrumentedStatement(NULL, NULL, var_table));
    //outputs.push_back(pushAndPopLocalVar(var_decl));

    // FIXME  other cases
    
    return outputs;
}

StatementReversalVec BasicStatementProcessor::processBasicBlock(SgStatement* stmt, const VariableVersionTable& var_table)
{
    SgBasicBlock* body = isSgBasicBlock(stmt);
    ROSE_ASSERT(body);
    
    // Use two vectors to store intermediate results.
    StatementReversalVec queue[2];
    vector<SgStatement*> to_delete;
    vector<SgInitializedName*> local_vars;

    int i = 0;
    queue[i].push_back(StatementReversal(buildBasicBlock(), buildBasicBlock(), var_table));

    // Deal with variable declarations first, since they will affect the variable version table.
    // For each variable declared in this basic block, we choose storing or not storing it at the end.
    foreach (SgStatement* stmt, body->get_statements())
    {
        if (SgVariableDeclaration* var_decl = isSgVariableDeclaration(stmt))
        {
            const SgInitializedNamePtrList& names = var_decl->get_variables();
            ROSE_ASSERT(names.size() == 1);
            SgInitializedName* init_name = names[0];

            // Collect all local variables here, which we will use later.
            local_vars.push_back(init_name);

            foreach (StatementReversal obj, queue[i])
            {

                /*******************************************************************************/
                // The first transformation is restore this local variable and restore it
                // at the beginning of the reverse basic block. Note that this variable already
                // has the final version unless we modify it.
                StatementReversal new_obj1 = obj.clone();

                ROSE_ASSERT(isSgBasicBlock(new_obj1.fwd_stmt));
                ROSE_ASSERT(isSgBasicBlock(new_obj1.rvs_stmt));

                // Store the value of local variables at the end of the basic block.
                SgVarRefExp* var_stored = buildVarRefExp(init_name->get_name());
                SgStatement* store_var = buildExprStatement(
                        pushVal(var_stored, init_name->get_type()));

                // Retrieve the value which is used to initialize that local variable.
                SgVariableDeclaration* decl_restore_var = buildVariableDeclaration(
                        init_name->get_name(),
                        init_name->get_type(),
                        buildAssignInitializer(popVal(init_name->get_type())),
                        isSgBasicBlock(new_obj1.rvs_stmt));

                appendStatement(store_var, isSgBasicBlock(new_obj1.fwd_stmt));
                ROSE_ASSERT(store_var->get_parent() == new_obj1.fwd_stmt);

                appendStatement(decl_restore_var, isSgBasicBlock(new_obj1.rvs_stmt));
                ROSE_ASSERT(decl_restore_var->get_parent() == new_obj1.rvs_stmt);

                /****** Update the cost. ******/
                new_obj1.cost.increaseStoreCount();

                /*******************************************************************************/
                // The second transformation is not to store it. We have to set its version NULL.
                StatementReversal new_obj2 = obj.clone();

                ROSE_ASSERT(isSgBasicBlock(new_obj2.rvs_stmt));

                // The second transformation is not to store it.
                SgStatement* just_decl = buildVariableDeclaration(
                        init_name->get_name(),
                        init_name->get_type(),
                        NULL, isSgBasicBlock(new_obj2.rvs_stmt));

                appendStatement(just_decl, isSgBasicBlock(new_obj2.rvs_stmt));
                ROSE_ASSERT(just_decl->get_parent() == new_obj2.rvs_stmt);

                /****** Update the variable version table. ******/
                new_obj2.var_table.setNullVersion(init_name);

                queue[1-i].push_back(new_obj1);
                queue[1-i].push_back(new_obj2);
            }

            foreach (StatementReversal& obj, queue[i])
            {
                to_delete.push_back(obj.fwd_stmt);
                to_delete.push_back(obj.rvs_stmt);
                //delete obj.fwd_stmt;
                //delete obj.rvs_stmt;
            }
            queue[i].clear();
            // Switch the index between 0 and 1.
            i = 1 - i;
        }
    }

    reverse_foreach (SgStatement* stmt, body->get_statements())
    {
        foreach (StatementReversal& obj, queue[i])
        {
            StatementReversalVec result = processStatement(stmt, obj.var_table);
            
            ROSE_ASSERT(!result.empty());

            foreach (StatementReversal& res, result)
            {
                // Currently, we cannot directly deep copy variable declarations. So we rebuild another one
                // with the same name, type and initializer.

                StatementReversal new_obj = obj.clone();

                ROSE_ASSERT(isSgBasicBlock(new_obj.fwd_stmt));
                ROSE_ASSERT(isSgBasicBlock(new_obj.rvs_stmt));

                if (res.fwd_stmt)
                {
                    prependStatement(res.fwd_stmt, isSgBasicBlock(new_obj.fwd_stmt));
                    //fixVariableReferences(isSgBasicBlock(new_obj.fwd_stmt));
                    //fixStatement(res.fwd_stmt, isSgBasicBlock(new_obj.fwd_stmt));
                }
                if (res.rvs_stmt)
                {
                    appendStatement(res.rvs_stmt, isSgBasicBlock(new_obj.rvs_stmt));
                    //fixVariableReferences(isSgBasicBlock(new_obj.rvs_stmt));
                    //fixStatement(res.rvs_stmt, isSgBasicBlock(new_obj.rvs_stmt));
                }

                /****** Update the variable version table and cost. ******/
                new_obj.var_table = res.var_table;
                new_obj.cost += res.cost;

                //fixVariableReferences(new_obj.fwd_stmt);
                //fixVariableReferences(new_obj.rvs_stmt);

                queue[1-i].push_back(new_obj);
            }
        }
        
        foreach (StatementReversal& obj, queue[i])
        {
            to_delete.push_back(obj.fwd_stmt);
            to_delete.push_back(obj.rvs_stmt);
            //delete obj.fwd_stmt;
            //delete obj.rvs_stmt;
        }
        queue[i].clear();
        // Switch the index between 0 and 1.
        i = 1 - i;
    }


    // Remove all local variables from variable version table since we will not use them anymore. 
    // This is helpful to prune branches by comparing variable version tables. 
    foreach (StatementReversal& stmt, queue[i])
    {
        foreach (SgInitializedName* var, local_vars)
            stmt.var_table.removeVariable(var);
    }


    // Since we build a varref before building its declaration, we may use the following function to fix them.
    //foreach (InstrumentedStatement& obj, queue[i])
    //{
        //cout << "Fixed: " << fixVariableReferences(obj.fwd_stmt) << endl;
        //fixVariableReferences(obj.rvs_stmt);
    //}

    foreach (SgStatement* stmt, to_delete)
        deepDelete(stmt);

    return queue[i];

#if 0
    StatementReversalVec outputs;

    vector<StmtPairs > all_stmts;

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

            SgVarRefExp* var_stored = buildVarRefExp(init_name);
            // Store the value of local variables at the end of the basic block.
            SgStatement* store_var = buildExprStatement(
                    pushVal(var_stored, var_stored->get_type()));

            // Retrieve the value which is used to initialize that local variable.
            SgStatement* decl_var = buildVariableDeclaration(
                    init_name->get_name(),
                    init_name->get_type(),
                    buildAssignInitializer(popVal(var_stored->get_type())));

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

    // If this basic block is an empty one, just return two empty basic blocks.
    if (outputs.empty())
        outputs.push_back(StmtPair(buildBasicBlock(), buildBasicBlock()));


    return outputs;

                StatementReversal new_obj2 = obj; // = obj.clone();
                new_obj2.fwd_stmt = copyStatement(obj.fwd_stmt);
                new_obj2.rvs_stmt = buildBasicBlock();

                foreach(SgStatement* s, isSgBasicBlock(obj.rvs_stmt)->get_statements())
                {
                    if (SgVariableDeclaration * decl = isSgVariableDeclaration(s))
                    {
                        SgInitializer* new_init = NULL;
                        SgInitializer* init = decl->get_variables()[0]->get_initializer();
                        if (init)
                            new_init = isSgInitializer(copyExpression(init));
                        isSgBasicBlock(new_obj2.rvs_stmt)->append_statement(
                                buildVariableDeclaration(
                                decl->get_variables()[0]->get_name(),
                                decl->get_variables()[0]->get_type(),
                                new_init));
                    } else
                        isSgBasicBlock(new_obj2.rvs_stmt)->append_statement(copyStatement(s));
                }
#endif
}


StatementReversalVec ReturnStatementProcessor::process(SgStatement* stmt, const VariableVersionTable& var_table)
{
	//The forward of a return statement is a return; the reverse is a no-op.
	if (SgReturnStmt * return_stmt = isSgReturnStmt(stmt))
	{
		StatementReversalVec stmts;
		stmts.push_back(StatementReversal(SageInterface::copyStatement(return_stmt), NULL, var_table));
		return stmts;
	}

	return StatementReversalVec();
}

