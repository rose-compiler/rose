// Example ROSE Translator: used within ROSE/tutorial

#include "eventReverser.h"
#include "facilityBuilder.h"
#include <stack>
#include <boost/algorithm/string.hpp>
#include <boost/tuple/tuple.hpp>

using namespace std;
using namespace boost;


set<SgFunctionDeclaration*> EventReverser::func_processed_;
const ExpPair EventReverser::NULL_EXP_PAIR = ExpPair(NULL, NULL);
const StmtPair EventReverser::NULL_STMT_PAIR = StmtPair(NULL, NULL);


SgExpression* EventReverser::reverseExpression(SgExpression* exp)
{
    return NULL;
}


ExpPair EventReverser::instrumentAndReverseExpression(SgExpression* exp)
{
    // We can just make copy of statements.
    // Before processing this expression, we replace it with its copy. This can avoid
    // to modify the expression passed in, or the same node appears in AST more than once.
    //exp = copyExpression(exp);

    // if this expression is a binary one
    if (SgBinaryOp* bin_op = isSgBinaryOp(exp))
        return processBinaryOp(bin_op);

    // if the expression is a unary one
    if (SgUnaryOp* unary_op = isSgUnaryOp(exp))
        return processUnaryOp(unary_op);

    // process the conditional expression (?:).
    if (SgConditionalExp* cond_exp = isSgConditionalExp(exp))
        return processConditionalExp(cond_exp);

    // process the function call expression
    if (SgFunctionCallExp* func_exp = isSgFunctionCallExp(exp))
        return processFunctionCallExp(func_exp);

    //if (isSgVarRefExp(exp) || isSgValueExp(exp) || isSgSizeOfOp(exp))

    return ExpPair(copyExpression(exp), NULL);
    //return ExpPair(copyExpression(exp), buildNullExpression());
    //return ExpPair(copyExpression(exp), copyExpression(exp));
}


StmtPair EventReverser::instrumentAndReverseStatement(SgStatement* stmt)
{
    if (stmt == NULL)
        return NULL_STMT_PAIR;

    if (SgExprStatement* exp_stmt = isSgExprStatement(stmt))
        return processExprStatement(exp_stmt);

    // if it's a block, process each statement inside
    if (SgBasicBlock* body = isSgBasicBlock(stmt))
        return processBasicBlock(body);

    // if it's a local variable declaration
    if (SgVariableDeclaration* var_decl = isSgVariableDeclaration(stmt))
        return processVariableDeclaration(var_decl);

    // if it's a if statement, process it according to the rule
    if (SgIfStmt* if_stmt = isSgIfStmt(stmt))
        return processIfStmt(if_stmt);

    if (SgForStatement* for_stmt = isSgForStatement(stmt))
        return processForStatement(for_stmt);

    if (SgForInitStatement* for_init_stmt = isSgForInitStatement(stmt))
        return processForInitStatement(for_init_stmt);

    if (SgWhileStmt* while_stmt = isSgWhileStmt(stmt))
        return processWhileStmt(while_stmt);

    if (SgDoWhileStmt* do_while_stmt = isSgDoWhileStmt(stmt))
        return processDoWhileStmt(do_while_stmt);

    if (SgSwitchStatement* switch_stmt = isSgSwitchStatement(stmt))
        return processSwitchStatement(switch_stmt);

#if 0
    if (SgBreakStmt* break_stmt = isSgBreakStmt(stmt))
        return copyStatement(break_stmt);

    if (SgContinueStmt* continue_stmt = isSgContinueStmt(stmt))
        return copyStatement(continue_stmt);

    if (SgReturnStmt* return_stmt = isSgReturnStmt(stmt))
        return copyStatement(return_stmt);
#endif

    // The following output should include break, continue and other ones.
    return StmtPair(
            copyStatement(stmt),
            copyStatement(stmt));
}

// This function add the loop counter related statements (counter declaration, increase, and store)
// to a forward for statement. 
SgStatement* EventReverser::assembleLoopCounter(SgStatement* loop_stmt)
{
    string counter_name = function_name_ + "_loop_counter_" + lexical_cast<string>(counter_++);

    SgStatement* counter_decl = buildVariableDeclaration(
            counter_name, 
            buildIntType(), 
            buildAssignInitializer(buildIntVal(0)));

#if 1
    SgStatement* incr_counter = buildExprStatement(
            buildPlusPlusOp(buildVarRefExp(counter_name), SgUnaryOp::prefix));

    if (SgForStatement* for_stmt = isSgForStatement(loop_stmt))
    {
        SgStatement* loop_body = for_stmt->get_loop_body();
        if (SgBasicBlock* block_body = isSgBasicBlock(loop_body))
            block_body->append_statement(incr_counter);
        else
        {
            SgBasicBlock* block_body = buildBasicBlock(loop_body, incr_counter);
            for_stmt->set_loop_body(block_body);
            block_body->set_parent(for_stmt);
        }
    }
    else if (SgWhileStmt* while_stmt = isSgWhileStmt(loop_stmt))
    {
        SgStatement* loop_body = while_stmt->get_body();
        if (SgBasicBlock* block_body = isSgBasicBlock(loop_body))
            block_body->append_statement(incr_counter);
        else
        {
            SgBasicBlock* block_body = buildBasicBlock(loop_body, incr_counter);
            while_stmt->set_body(block_body);
            block_body->set_parent(while_stmt);
        }
    }
    else if (SgDoWhileStmt* do_while_stmt = isSgDoWhileStmt(loop_stmt))
    {
        SgStatement* loop_body = do_while_stmt->get_body();
        if (SgBasicBlock* block_body = isSgBasicBlock(loop_body))
            block_body->append_statement(incr_counter);
        else
        {
            SgBasicBlock* block_body = buildBasicBlock(loop_body, incr_counter);
            do_while_stmt->set_body(block_body);
            block_body->set_parent(do_while_stmt);
        }
    }
#endif

    SgStatement* push_counter = buildFunctionCallStmt(
            "push", 
            buildIntType(), 
            buildExprListExp(
                buildVarRefExp(counter_stack_name_),
                buildVarRefExp(counter_name))); 

    return buildBasicBlock(counter_decl, loop_stmt, push_counter);
}

SgStatement* EventReverser::buildForLoop(SgStatement* loop_body)
{
    // build a simple for loop like: for (int i = N; i > 0; --i)

    // FIXME test the validation of this name.
    string counter_name = "i";

    SgStatement* init = buildVariableDeclaration(
            counter_name, buildIntType(), buildAssignInitializer(popLoopCounter()));
    SgStatement* test = buildExprStatement(
            buildBinaryExpression<SgGreaterThanOp>(
                buildVarRefExp(counter_name), 
                buildIntVal(0)));
    SgExpression* incr = buildMinusMinusOp(buildVarRefExp(counter_name), SgUnaryOp::prefix);

    SgStatement* for_stmt = buildForStatement(init, test, incr, loop_body);
    return for_stmt;
}

vector<FuncDeclPair> EventReverser::outputFunctions()
{
    SgBasicBlock* body = func_decl_->get_definition()->get_body();
    // Function body is a basic block, which is a kind of statement.
    SgStatement *fwd_body, *rev_body;
    tie(fwd_body, rev_body) = instrumentAndReverseStatement(body);

    SgName func_name = func_decl_->get_name() + "_forward";
    SgFunctionDeclaration* fwd_func_decl = 
        buildDefiningFunctionDeclaration(func_name, func_decl_->get_orig_return_type(), 
                isSgFunctionParameterList(copyStatement(func_decl_->get_parameterList())));
    SgFunctionDefinition* fwd_func_def = fwd_func_decl->get_definition();
    fwd_func_def->set_body(isSgBasicBlock(fwd_body));
    fwd_body->set_parent(fwd_func_def);

#if 0
    pushScopeStack(isSgScopeStatement(fwd_func_decl->get_definition()->get_body()));

    SgStatementPtrList fwd_stmt_list = isSgBasicBlock(fwd_body)->get_statements();
    foreach (SgStatement* stmt, fwd_stmt_list)
        appendStatement(stmt);

    popScopeStack();
#endif

    func_name = func_decl_->get_name() + "_reverse";
    SgFunctionDeclaration* rev_func_decl = 
        buildDefiningFunctionDeclaration(func_name, func_decl_->get_orig_return_type(), 
                isSgFunctionParameterList(copyStatement(func_decl_->get_parameterList()))); 
    SgFunctionDefinition* rev_func_def = rev_func_decl->get_definition();
    rev_func_def->set_body(isSgBasicBlock(rev_body));
    rev_body->set_parent(rev_func_def);

#if 0
    pushScopeStack(isSgScopeStatement(rev_func_decl->get_definition()->get_body()));

    SgStatementPtrList rev_stmt_list = isSgBasicBlock(rev_body)->get_statements();
    foreach (SgStatement* stmt, rev_stmt_list)
        appendStatement(stmt);

    popScopeStack();
#endif

    output_func_pairs_.push_back(FuncDeclPair(fwd_func_decl, rev_func_decl));
    return output_func_pairs_;
}


class reverserTraversal : public AstSimpleProcessing
{
    public:
        reverserTraversal() 
            : events_num(0),  
            model_type(0),
            AstSimpleProcessing()
    {}
        virtual void visit(SgNode* n);
        vector<SgFunctionDeclaration*> funcs_gen;
        vector<SgFunctionDeclaration*> all_funcs;
        vector<SgStatement*> var_decls;
        vector<SgStatement*> var_inits;
        SgClassType* model_type;
        int events_num;
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


        EventReverser reverser(func_decl);
        vector<FuncDeclPair> func_pairs = reverser.outputFunctions();
        foreach(const FuncDeclPair& func_pair, func_pairs)
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
}


// Put the functions generated in place
void insertFunctionInPlace(SgFunctionDeclaration* func, const vector<SgFunctionDeclaration*>& all_funcs)
{
    string func_name = func->get_name();
    replace_last(func_name, "_forward", "");
    replace_last(func_name, "_reverse", "");

    foreach (SgFunctionDeclaration* f, all_funcs)
        if (func_name == string(f->get_name()))
            insertStatementAfter(f, func);
}



int fixVariableReferences2(SgNode* root)
{
    ROSE_ASSERT(root);
    int counter=0;
    Rose_STL_Container<SgNode*> nodeList;

    SgVarRefExp* varRef=NULL;
    Rose_STL_Container<SgNode*> reflist = NodeQuery::querySubTree(root, V_SgVarRefExp);
    for (Rose_STL_Container<SgNode*>::iterator i=reflist.begin();i!=reflist.end();i++)
    {
        //cout << get_name(isSgVarRefExp(*i)) << endl;
        varRef= isSgVarRefExp(*i);
        ROSE_ASSERT(varRef->get_symbol());
        SgInitializedName* initname= varRef->get_symbol()->get_declaration();
        //ROSE_ASSERT(initname);

        if (initname->get_type()==SgTypeUnknown::createType())
            //    if ((initname->get_scope()==NULL) && (initname->get_type()==SgTypeUnknown::createType()))
        {
            SgName varName=initname->get_name();
            SgSymbol* realSymbol = NULL;
            //cout << varName << endl;

            // CH (5/7/2010): Before searching SgVarRefExp objects, we should first deal with class/structure
            // members. Or else, it is possible that we assign the wrong symbol to those members if there is another
            // variable with the same name in parent scopes. Those members include normal member referenced using . or ->
            // operators, and static members using :: operators.
            //
            if (SgArrowExp* arrowExp = isSgArrowExp(varRef->get_parent()))
            {
                if (varRef == arrowExp->get_rhs_operand())
                {
                    // make sure the lhs operand has been fixed
                    counter += fixVariableReferences2(arrowExp->get_lhs_operand());

                    SgPointerType* ptrType = isSgPointerType(arrowExp->get_lhs_operand()->get_type());
                    ROSE_ASSERT(ptrType);
                    SgClassType* clsType = isSgClassType(ptrType->get_base_type());
                    ROSE_ASSERT(clsType);
                    SgClassDeclaration* decl = isSgClassDeclaration(clsType->get_declaration());
                    decl = isSgClassDeclaration(decl->get_definingDeclaration());
                    ROSE_ASSERT(decl);
                    realSymbol = lookupSymbolInParentScopes(varName, decl->get_definition());
                }
                else
                    realSymbol = lookupSymbolInParentScopes(varName,getScope(varRef));
            }
            else if (SgDotExp* dotExp = isSgDotExp(varRef->get_parent()))
            {
                if (varRef == dotExp->get_rhs_operand())
                {
                    // make sure the lhs operand has been fixed
                    counter += fixVariableReferences2(dotExp->get_lhs_operand());

                    SgClassType* clsType = isSgClassType(dotExp->get_lhs_operand()->get_type());
                    ROSE_ASSERT(clsType);
                    SgClassDeclaration* decl = isSgClassDeclaration(clsType->get_declaration());
                    decl = isSgClassDeclaration(decl->get_definingDeclaration());
                    ROSE_ASSERT(decl);
                    realSymbol = lookupSymbolInParentScopes(varName, decl->get_definition());
                }
                else
                    realSymbol = lookupSymbolInParentScopes(varName,getScope(varRef));
            }
            else
                realSymbol = lookupSymbolInParentScopes(varName,getScope(varRef));

            // should find a real symbol at this final fixing stage!
            // This function can be called any time, not just final fixing stage
            if (realSymbol==NULL) 
            {
                //cerr<<"Error: cannot find a symbol for "<<varName.getString()<<endl;
                //ROSE_ASSERT(realSymbol);
            }
            else {
                // release placeholder initname and symbol
                ROSE_ASSERT(realSymbol!=(varRef->get_symbol()));

                bool flag = false;

                SgSymbol* symbol_to_delete = varRef->get_symbol();
                varRef->set_symbol(isSgVariableSymbol(realSymbol));
                counter ++;

                if (nodeList.empty())
                {
                    VariantVector vv(V_SgVarRefExp);
                    nodeList = NodeQuery::queryMemoryPool(vv);
                }
                foreach(SgNode* node, nodeList)
                {
                    if (SgVarRefExp* var = isSgVarRefExp(node))
                    {
                        if (var->get_symbol() == symbol_to_delete)
                        {
                            flag = true;
                            break;
                        }
                    }
                }
                if (!flag)
                {
                    //cout <<initname->get_name().str() << endl;
                    delete initname; // TODO deleteTree(), release File_Info nodes etc.
                    delete symbol_to_delete;
                }
            }
        }
    } // end for
    return counter;
}









int main( int argc, char * argv[] )
{
    SgProject* project = frontend(argc,argv);
    reverserTraversal reverser;
    bool klee = true;

    SgGlobal *globalScope = getFirstGlobalScope(project);
    string includes = "#include \"rctypes.h\"\n"
        "#include <stdio.h>\n"
        "#include <stdlib.h>\n"
        "#include <time.h>\n"
        "#include <assert.h>\n"
        "#include <memory.h>\n";
    if (klee)
      includes += "#include <klee.h>\n";
    addTextForUnparser(globalScope,includes,AstUnparseAttribute::e_before); 

    pushScopeStack(isSgScopeStatement(globalScope));

    reverser.traverseInputFiles(project,preorder);
    cout << "Traverse complete\n";

    ROSE_ASSERT(reverser.model_type);

    //SgStatement* init_func = buildInitializationFunction();


    for (size_t i = 0; i < reverser.var_decls.size(); ++i)
        prependStatement(reverser.var_decls[i]);
    for (size_t i = 0; i < reverser.funcs_gen.size(); ++i)
        insertFunctionInPlace(reverser.funcs_gen[i], reverser.all_funcs);
    //appendStatement(reverser.funcs[i]);

    appendStatement(buildInitializationFunction(reverser.model_type));
    appendStatement(buildCompareFunction(reverser.model_type));
    appendStatement(buildMainFunction(reverser.var_inits, reverser.events_num, klee));


    popScopeStack();

    // Write or find a function to clear all nodes in memory pool who don't have parents.
#if 1
    cout << "Start to fix variables references\n";
    fixVariableReferences2(globalScope);
    cout << "Fix finished\n";

    AstTests::runAllTests(project);
#endif
    return backend(project);
}

