#include "statementProcessor.h"
#include <boost/tuple/tuple.hpp>
#include <boost/lexical_cast.hpp>
#include "utilities/CPPDefinesAndNamespaces.h"

using namespace SageInterface;
using namespace SageBuilder;

InstrumentedStatementVec BasicStatementProcessor::process(
        SgStatement* stmt, const VariableVersionTable& var_table)
{
    if (SgExprStatement* exp_stmt = isSgExprStatement(stmt))
        return processExprStatement(exp_stmt, var_table);

	else if (SgVariableDeclaration* var_decl = isSgVariableDeclaration(stmt))
        return processVariableDeclaration(var_decl, var_table);

	else if (SgBasicBlock* block = isSgBasicBlock(stmt))
        return processBasicBlock(block, var_table);

	//The forward of a return statement is a return; the reverse is a no-op.
	else if (isSgReturnStmt(stmt))
	{
		InstrumentedStatementVec results;
		results.push_back(InstrumentedStatement(SageInterface::copyStatement(stmt), NULL, var_table));
		return results;
	}
    //if (SgIfStmt* if_stmt = isSgIfStmt(stmt))
       // return processIfStmt(if_stmt, var_table);

    return InstrumentedStatementVec();
}

#if 0
StmtPairs BasicStatementProcessor::processFunctionDeclaration(SgFunctionDeclaration* func_decl)
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
#endif

InstrumentedStatementVec BasicStatementProcessor::processExprStatement(
        SgExprStatement* exp_stmt,
        const VariableVersionTable& var_table)
{
    InstrumentedExpressionVec exps = processExpression(exp_stmt->get_expression(), var_table, false);

    ROSE_ASSERT(!exps.empty());

    InstrumentedStatementVec stmts;
    foreach (InstrumentedExpression& exp_obj, exps)
    {
        SgStatement *fwd_stmt = NULL, *rvs_stmt = NULL;

        if (exp_obj.fwd_exp)
            fwd_stmt = buildExprStatement(exp_obj.fwd_exp);
        if (exp_obj.rvs_exp)
            rvs_stmt = buildExprStatement(exp_obj.rvs_exp);

        // Use the variable version table output by expression processor.
        stmts.push_back(InstrumentedStatement(fwd_stmt, rvs_stmt, exp_obj.var_table));
    }
    return stmts;
}

InstrumentedStatementVec BasicStatementProcessor::processVariableDeclaration(
        SgVariableDeclaration* var_decl,
        const VariableVersionTable& var_table)
{
    InstrumentedStatementVec outputs;

    // Note the store and restore of local variables are processd in
    // basic block, not here. We just forward the declaration to forward
    // event function.

    // FIXME copyStatement also copies preprocessing info
    outputs.push_back(InstrumentedStatement(copyStatement(var_decl), NULL, var_table));

    //outputs.push_back(InstrumentedStatement(NULL, NULL, var_table));
    //outputs.push_back(pushAndPopLocalVar(var_decl));

    // FIXME  other cases
    
    return outputs;
}

InstrumentedStatementVec BasicStatementProcessor::processBasicBlock(
        SgBasicBlock* body,
        const VariableVersionTable& var_table)
{
    // Use two vectors to store intermediate results.
    InstrumentedStatementVec queue[2];
    vector<SgStatement*> to_delete;

    int i = 0;
    queue[i].push_back(InstrumentedStatement(buildBasicBlock(), buildBasicBlock(), var_table));

    // Deal with variable declarations first, since they will affect the variable version table.
    // For each variable declared in this basic block, we choose storing or not storing it at the end.
    foreach (SgStatement* stmt, body->get_statements())
    {
        if (SgVariableDeclaration* var_decl = isSgVariableDeclaration(stmt))
        {
            foreach (InstrumentedStatement obj, queue[i])
            {
                const SgInitializedNamePtrList& names = var_decl->get_variables();
                ROSE_ASSERT(names.size() == 1);
                SgInitializedName* init_name = names[0];


                /*******************************************************************************/
                // The first transformation is restore this local variable and restore it
                // at the beginning of the reverse basic block. Note that this variable already
                // has the final version unless we modify it.
                InstrumentedStatement new_obj1 = obj.clone();

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
                //fixVariableDeclaration(decl_restore_var, isSgBasicBlock(new_obj1.rvs_stmt));

                /*******************************************************************************/
                // The second transformation is not to store it. We have to set its version NULL.
                InstrumentedStatement new_obj2 = obj.clone();

                ROSE_ASSERT(isSgBasicBlock(new_obj2.rvs_stmt));

                // The second transformation is not to store it.
                SgStatement* just_decl = buildVariableDeclaration(
                        init_name->get_name(),
                        init_name->get_type(),
                        NULL, isSgBasicBlock(new_obj2.rvs_stmt));

                appendStatement(just_decl, isSgBasicBlock(new_obj2.rvs_stmt));
                ROSE_ASSERT(just_decl->get_parent() == new_obj2.rvs_stmt);

                new_obj2.var_table.setNullVersion(init_name);

                queue[1-i].push_back(new_obj1);
                queue[1-i].push_back(new_obj2);
            }

            foreach (InstrumentedStatement& obj, queue[i])
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
        foreach (InstrumentedStatement& obj, queue[i])
        {
            InstrumentedStatementVec result = processStatement(stmt, obj.var_table);
            
            ROSE_ASSERT(!result.empty());

            foreach (InstrumentedStatement& res, result)
            {
                // Currently, we cannot directly deep copy variable declarations. So we rebuild another one
                // with the same name, type and initializer.

                InstrumentedStatement new_obj = obj.clone();

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
                new_obj.var_table = res.var_table;

                fixVariableReferences(new_obj.fwd_stmt);
                fixVariableReferences(new_obj.rvs_stmt);

                queue[1-i].push_back(new_obj);
            }
        }
        
        foreach (InstrumentedStatement& obj, queue[i])
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


    // Since we build a varref before building its declaration, we may use the following function to fix them.
    foreach (InstrumentedStatement& obj, queue[i])
    {
        //cout << "Fixed: " << fixVariableReferences(obj.fwd_stmt) << endl;
        //fixVariableReferences(obj.rvs_stmt);
    }

    foreach (SgStatement* stmt, to_delete)
        deepDelete(stmt);

    return queue[i];

#if 0
    InstrumentedStatementVec outputs;

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

                InstrumentedStatement new_obj2 = obj; // = obj.clone();
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

#if 0
StmtPairs BasicStatementProcessor::processIfStmt(
        SgIfStmt* if_stmt,
        const VariableVersionTable& var_table)
{
    //SgStatement *fwd_true_body, *fwd_false_body;
    //SgStatement *rvs_true_body, *rvs_false_body;

    SgStatement* true_body = if_stmt->get_true_body();
    SgStatement* false_body = if_stmt->get_false_body();

    // Here we have do decide whether to store the flag. We don't have to store
    // the flag if the value of that flag will not change after the if statement.
    // Otherwise, we will push the flag at the end of if statement.

    // After normalization, we require that the condition part of if statement
    // does not need to be reversed. In other word, the expression of if condition
    // does not modify any value.

    StmtPairs transformed_true_bodies = processStatement(true_body);
    StmtPairs transformed_false_bodies = processStatement(false_body);

    //if (transformed_false_bodies.empty())
       // transformed_false_bodies.push_back(NULL_STMT_PAIR);

    StmtPairs output;

    foreach (StmtPair true_bodies, transformed_true_bodies)
    {
        foreach (StmtPair false_bodies, transformed_false_bodies)
        {
            SgIfStmt* fwd_if_stmt = buildIfStmt(
                    copyStatement(if_stmt->get_conditional()),
                    copyStatement(true_bodies.first),
                    copyStatement(false_bodies.first));

            // Note that after normalization, both true/false bodies are basic blocks.
            ROSE_ASSERT(isSgBasicBlock(fwd_if_stmt->get_true_body()));
            ROSE_ASSERT(isSgBasicBlock(fwd_if_stmt->get_false_body()));


            // At the end of true/false body, push the flag into stack.
            isSgBasicBlock(fwd_if_stmt->get_true_body())->append_statement(
                    buildExprStatement(pushVal(
                        buildBoolValExp(true),
                        buildBoolType())));
            isSgBasicBlock(fwd_if_stmt->get_false_body())->append_statement(
                    buildExprStatement(pushVal(
                        buildBoolValExp(false),
                        buildBoolType())));


            SgIfStmt* rvs_if_stmt = buildIfStmt(
                    popVal(buildBoolType()),
                    copyStatement(true_bodies.second),
                    copyStatement(false_bodies.second));
            output.push_back(StmtPair(fwd_if_stmt, rvs_if_stmt));
        }
    }

    return output;
}
#endif

