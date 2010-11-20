#include "buildTestCode.h"
#include <vector>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace boost;
using namespace SageBuilder;
using namespace SageInterface;

vector<SgSymbol*> to_delete;

#define foreach BOOST_FOREACH



SgClassDeclaration* TestCodeBuilder::buildModelStruct()
{
    model_decl_ = buildStructDeclaration("model");
    SgClassDefinition* def = buildClassDefinition(model_decl_);

    pushScopeStack(isSgScopeStatement(def));

    SgVariableDeclaration* int_var = buildVariableDeclaration(int_var_name_, buildIntType());
    SgVariableDeclaration* int_array_var = 
        buildVariableDeclaration(int_array_var_name_, buildArrayType(buildIntType(), buildIntVal(array_size_)));

    def->append_member(int_var);
    def->append_member(int_array_var);

    popScopeStack();

#if 0
    model_type_ = model_decl_->get_type();
    model_obj_ = buildInitializedName(model_obj_name_, buildPointerType(model_type_));
    // Build the int var.
    int_var_ = buildBinaryExpression<SgArrowExp>(
            buildVarRefExp(model_obj_name_),
            buildVarRefExp(int_var_name_)); 
    int_var_->set_lvalue(true);

    // Build the int array var
    int_array_var_ = buildBinaryExpression<SgArrowExp>(
            buildVarRefExp(model_obj_name_),
            buildVarRefExp(int_array_var_name_));
#endif
    return model_decl_;
}

SgFunctionDeclaration* TestCodeBuilder::buildEventDeclaration(int i)
{
    model_type_ = model_decl_->get_type();
    //if (model_obj_ == NULL)
    model_obj_ = buildInitializedName(model_obj_name_, buildPointerType(model_type_));

    SgFunctionParameterList* para_list = buildFunctionParameterList(model_obj_);
    SgFunctionDeclaration* decl = 
        buildDefiningFunctionDeclaration(
                "event" + lexical_cast<string>(i),
                buildVoidType(),
                para_list);
    //buildBasicBlock());
    //fixVariableReferences(decl);
    return decl;
}

SgFunctionDeclaration* TestCodeBuilder::buildEventFunction(SgStatement* stmt, int idx)
{
#if 1
    if (SgBasicBlock* block = isSgBasicBlock(stmt))
        ;//return buildEventFunction(block->get_statements(), idx);

    SgFunctionDeclaration* decl = buildEventDeclaration(idx);
    //stmt = buildExprStatement(buildIntVal(0));
    appendStatement(stmt, isSgScopeStatement(decl->get_definition()->get_body()));

    //cout << fixStatement(stmt, isSgScopeStatement(decl->get_definition()->get_body())) << endl;

    //fixVariableReferences(decl->get_definition());
    //cout << fixVariableReferences(decl) << endl;
#else
    // For experiment
    pushScopeStack(isSgScopeStatement(decl->get_definition()->get_body()));
    appendStatement(buildVariableDeclaration("i", buildIntType()));
    appendStatement(
            buildExprStatement(
                buildBinaryExpression<SgAddOp>(
                    int_var_,
                    buildBinaryExpression<SgAssignOp>(
                        int_var_,
                        buildPlusPlusOp(
                            int_var_,
                            SgUnaryOp::prefix)))));
    popScopeStack();

#endif
    return decl;
}

SgFunctionDeclaration* TestCodeBuilder::buildEventFunction(const vector<SgStatement*> stmts, int idx)
{
    SgFunctionDeclaration* decl = buildEventDeclaration(idx);
    pushScopeStack(isSgScopeStatement(decl->get_definition()->get_body()));
    foreach (SgStatement* stmt, stmts) appendStatement(stmt);
    popScopeStack();
    return decl;
}

/* 
   template <class T>
   SgFunctionDeclaration* buildUnaryExpStmtEvent()
   {
   SgStatement* stmt = buildExprStmt(buildUnaryExpression<T>(var));
   SgFunctionDeclaration* func = isSgFunctionDeclaration(copeStatement(func_decl_));
   func->get_defination()->get_body()->append_statement(stmt);
   return func;
   }
   */

SgExpression* TestCodeBuilder::buildRandomBoolValExp()
{
    return buildBinaryExpression<SgGreaterThanOp>(
            buildFunctionCallExp("rand", buildIntType()),
            buildIntVal(RAND_MAX/2));
}

#if 0
enum expName
{
    PP_PRE = 0,
    PP_POST,
    MM_PRE,
    MM_POST,
    ASS,
    PASS,
    MASS,
    MULASS,
    DIVASS
};
#endif

void TestCodeBuilder::buildAllUnaryExpressions(SgExpression* operand, vector<SgExpression*>& exps)
{
    //if (operand->get_lvalue())
    if (/* lhs->get_lvalue() ||*/ isSgArrowExp(operand))
    {
        exps.push_back(buildPlusPlusOp(copyExpression(operand), SgUnaryOp::prefix)); 
        //exps.push_back(buildMinusMinusOp(copyExpression(operand), SgUnaryOp::prefix)); 
        exps.push_back(buildPlusPlusOp(copyExpression(operand), SgUnaryOp::postfix)); 
        //exps.push_back(buildMinusMinusOp(copyExpression(operand), SgUnaryOp::postfix)); 
    }
    exps.push_back(buildUnaryExpression<SgNotOp>(copyExpression(operand))); 
}

void TestCodeBuilder::buildAllBinaryExpressions(SgExpression* lhs, SgExpression* rhs, vector<SgExpression*>& exps)
{
    //if (lhs->get_lvalue())
    if (/* lhs->get_lvalue() ||*/ isSgArrowExp(lhs))
    {
        exps.push_back(buildBinaryExpression<SgAssignOp>(copyExpression(lhs), copyExpression(rhs)));
        //exps.push_back(buildBinaryExpression<SgPlusAssignOp>(copyExpression(lhs), copyExpression(rhs)));
        //exps.push_back(buildBinaryExpression<SgMinusAssignOp>(copyExpression(lhs), copyExpression(rhs)));
        //exps.push_back(buildBinaryExpression<SgMultAssignOp>(copyExpression(lhs), copyExpression(rhs)));
    }
    // temporarily remove DivAssign to avoid exceptions
    //exps.push_back(buildBinaryExpression<SgDivAssignOp>(copyExpression(lhs), copyExpression(rhs)));
    //exps.push_back(buildBinaryExpression<SgAddOp>(copyExpression(lhs), copyExpression(rhs)));
    exps.push_back(buildBinaryExpression<SgAndOp>(copyExpression(lhs), copyExpression(rhs)));
    exps.push_back(buildBinaryExpression<SgOrOp>(copyExpression(lhs), copyExpression(rhs)));
    //exps.push_back(buildBinaryExpression<SgCommaOpExp>(copyExpression(lhs), copyExpression(rhs)));
    //exps.push_back(buildBinaryExpression<SgEqualityOp>(copyExpression(lhs), copyExpression(rhs)));

#ifdef UNUSUAL_EXPRESSION
    exps.push_back(buildBinaryExpression<SgModAssignOp>(copyExpression(lhs), copyExpression(rhs)));
    exps.push_back(buildBinaryExpression<SgIorAssignOp>(copyExpression(lhs), copyExpression(rhs)));
    exps.push_back(buildBinaryExpression<SgAndAssignOp>(copyExpression(lhs), copyExpression(rhs)));
    exps.push_back(buildBinaryExpression<SgXorAssignOp>(copyExpression(lhs), copyExpression(rhs)));
    exps.push_back(buildBinaryExpression<SgLshiftAssignOp>(copyExpression(lhs), copyExpression(rhs)));
    exps.push_back(buildBinaryExpression<SgRshiftAssignOp>(copyExpression(lhs), copyExpression(rhs)));
#endif
    //foreach (SgExpression* exp, exps)
    //  cout << exp->class_name() << ' ' << exp->get_lvalue() << endl;
}

void TestCodeBuilder::buildAllBasicExpressions(SgExpression* var1, SgExpression* var2, vector<SgExpression*>& exps)//SgExpression* var3 = NULL)
{
    // First, build all possible unary expressions
    buildAllUnaryExpressions(var1, exps);
    // Second, build all possible binary expressions
    buildAllBinaryExpressions(var1, var2, exps);
    // Add a simple function call
    //exps.push_back(buildFunctionCallExp("rand", buildIntType()));
}

void TestCodeBuilder::buildCompoundBasicExpressions(
        SgExpression* var1, 
        SgExpression* var2, 
        vector<SgExpression*>& exps)
{
    vector<SgExpression*> rhs; 
    buildAllBasicExpressions(var1, var2, rhs);
    buildConditionalExpressions(vector<SgExpression*>(rhs), rhs);
    // Note that only var1 here can be a lvalue, which is a variable reference
    for (size_t i = 0; i < rhs.size(); ++i)
        buildAllBinaryExpressions(var1, rhs[i], exps); 
#if 0
    // more compound expressions
    int size = exps.size();
    for (int i = 0; i < size; ++i)
        buildAllBinaryExpressions(var1, exps[i], exps);
#endif
}

void TestCodeBuilder::buildConditionalExpressions(
        const vector<SgExpression*>& input, 
        vector<SgExpression*>& exps)
{
    for (size_t i = 0; i < input.size(); ++i)
    {
        for (size_t j = i + 1; j < input.size(); ++j)
        {
            if (input[i]->get_type() == input[j]->get_type())
            {

#if 1
                exps.push_back(buildConditionalExp(
                            copyExpression(input[0]),
                            copyExpression(input[i]),
                            copyExpression(input[j])));
                exps.push_back(buildConditionalExp(
                            buildRandomBoolValExp(), 
                            copyExpression(input[i]), 
                            copyExpression(input[j])));
#endif
            }
        }
    }
}

void TestCodeBuilder::buildFunctionCallExpressions(
        const vector<SgExpression*>& input, 
        vector<SgExpression*>& exps)
{
    //foreach (SgExpression* exp, input)
    //  cout << exp->get_type()->class_name() << endl;
    foreach (SgExpression* exp, input)
    {
        //if (exp->get_type() == buildIntType())
        exps.push_back(buildFunctionCallExp(
                    "srand",
                    buildVoidType(),
                    buildExprListExp(
                        copyExpression(exp))));
    }
}

#if 0
vector<SgStatement*> buildConditionalStatements(const vector<SgExpression*> exps)
{
    vector<SgStatement*> stmts;
    stmts.push_back(buildExprStatement(buildConditionalExp(exps[0], exps[1], exps[2])));
    stmts.push_back(buildExprStatement(buildConditionalExp(buildRandomBoolValExp(), exps[1], exps[2])));
    return stmts;
}
#endif


void TestCodeBuilder::buildIfStatements(
        SgExpression* cond, 
        SgStatement* true_body, 
        SgStatement* false_body,
        vector<SgStatement*>& if_stmts)
{
    if_stmts.push_back(buildIfStmt(cond, true_body, NULL));
    if_stmts.push_back(buildIfStmt(cond, true_body, false_body));
}

// Build a for statement by giving a loop number and its body
SgForStatement* TestCodeBuilder::buildForStatementWithLoopNum(SgExpression* num, SgStatement* body)
{
    SgStatement* init = buildVariableDeclaration("i", buildIntType(), 
            buildAssignInitializer(buildIntVal(0)));
    SgStatement* cond = buildExprStatement(buildBinaryExpression<SgLessThanOp>(
                buildVarRefExp("i"), num));
    SgExpression* incr = buildPlusPlusOp(buildVarRefExp("i"), SgUnaryOp::prefix);
    return buildForStatement(init, cond, incr, body);
}

void TestCodeBuilder::buildTestCode(const vector<vector<SgStatement*> >& stmt_groups)
{
    for (size_t i = 0; i < stmt_groups.size(); ++i)
    {
        SgSourceFile* src_file = isSgSourceFile((*project_)[i]);
        src_file->set_unparse_output_filename(src_file->getFileName());

        SgGlobal* global_scope = src_file->get_globalScope();
        pushScopeStack(isSgScopeStatement(global_scope));

        SgClassDeclaration* model_decl = buildModelStruct();
        appendStatement(model_decl);

        for (size_t j = 0; j < stmt_groups[i].size(); ++j)
        {
            SgStatement* event_func = buildEventFunction(stmt_groups[i][j], j);
            appendStatement(event_func);
        }

        cout << fixVariableReferences(global_scope) << endl;

        //scope = SageBuilder::topScopeStack();


        popScopeStack();
    }

    // deal with the remain files
    for (size_t i = stmt_groups.size(); i < project_->numberOfFiles(); ++i)
    {
        SgFile* fl = (*project_)[i];
        string filename = fl->getFileName();
        fl->set_unparse_output_filename(filename);
    }

    foreach (SgSymbol* sym, to_delete)
        delete sym;

    //AstTests::runAllTests(project_);
    backend(project_);

    // delete the remain files
    for (int i = stmt_groups.size(); i < project_->numberOfFiles(); ++i)
    {
        SgFile* fl = (*project_)[i];
        string filename = fl->getFileName();
        remove(filename.c_str());
    }

    modifyMakefile(stmt_groups.size());
}

void TestCodeBuilder::buildTestCode()
{
    //cout << int_var_->class_name() << int_var_->get_lvalue() << endl;
    vector<vector<SgStatement*> > stmt_groups;

    // To build some function calls, there must be a scope in the stack
    SgGlobal *globalScope = getFirstGlobalScope (project_);
    pushScopeStack (isSgScopeStatement (globalScope));


    //foreach (SgExpression* exp, cond_exps)
    //    buildCompoundBasicExpressions(copyExpression(exp), buildIntVal(rand() % 0x1000), comp_exps);

    //vector<SgStatement*> exp_stmts;

    // Push the expression statements into list
    //foreach (SgExpression* exp, exps)
    //  exp_stmts.push_back(buildExprStatement(exp));

    // Push the if statements into list
    //SgExpression* cond = buildBinaryExpression<SgGreaterThanOp>(buildRandomBoolValExp());
    //vector<SgIfStmt*> if_stmts = buildIfStatements(cond, exp_stmts[0], exp_stmts[2]);

    //addStatements(exp_stmts);
    //addStatements(buildConditionalStatements(exps));
    //addStatements(buildIfStatements(buildRandomBoolValExp(), exp_stmts[0], exp_stmts[2]));
    //vector<SgStatement*> stmts;
    //stmts.insert(stmts.end(), exp_stmts.begin(), exp_stmts.end());
    //stmts.insert(stmts.end(), if_stmts.begin(), if_stmts.end());
    //


    /////////////////////////////////////////////////////////////////////////////////////////
    // Build statement groups
    //
    // **************************************************************************************
    // 0. Basic expression statements test (include conditional expressions).
    //    Output: local_var, basic_exps, cond_exps; 
    // **************************************************************************************
    //
    // Build local variable
#if 1
    int_var_ = buildBinaryExpression<SgArrowExp>(
            buildVarRefExp(model_obj_name_),
            buildVarRefExp(int_var_name_)); 
#endif
#if 1
    SgStatement* local_var = 
        buildVariableDeclaration("i", buildIntType(), 
                buildAssignInitializer(
                    //buildPlusPlusOp(copyExpression(int_var_), 
        buildPlusPlusOp(copyExpression(int_var_), SgUnaryOp::prefix)));
#else
                SgStatement* local_var = buildVariableDeclaration("i", buildIntType());
#endif

                // Build basic expressions   
                vector<SgExpression*> basic_exps;

                //int_var_->set_lvalue(true);
                //basic_exps.push_back(buildVarRefExp(model_obj_name_));

                buildAllBasicExpressions(int_var_, buildIntVal(rand() % 0x1000), basic_exps);
                //buildAllBasicExpressions(buildIntVal(0), buildIntVal(rand() % 0x1000), basic_exps);
                //buildAllBasicExpressions(int_var_, buildVarRefExp("i"), basic_exps);

                // Build conditional expressions
                vector<SgExpression*> cond_exps;
                buildConditionalExpressions(vector<SgExpression*>(basic_exps), cond_exps);

                stmt_groups.push_back(vector<SgStatement*>());
                foreach (SgExpression* exp, basic_exps)
#if 0
                    stmt_groups.back().push_back(buildExprStatement(exp));
#else
                stmt_groups.back().push_back(buildBasicBlock(
                            (local_var),
                            //copyStatement(local_var),
                            //buildExprStatement(buildIntVal(0))));
                    buildExprStatement(exp)));
#endif
                foreach (SgExpression* exp, cond_exps)
                    stmt_groups.back().push_back(buildBasicBlock(
                                (local_var),
                                buildExprStatement(exp)));

                // **************************************************************************************
                // 1. Build simple combinations of basic expressions.
                //    Output: comp_exps;
                // **************************************************************************************
                //
                // Build the combinations of basic expressions
                vector<SgExpression*> comp_exps;
                buildCompoundBasicExpressions(copyExpression(int_var_), buildIntVal(rand() % 0x1000), comp_exps);

                stmt_groups.push_back(vector<SgStatement*>());
                foreach (SgExpression* exp, comp_exps)
                    stmt_groups.back().push_back(buildExprStatement(exp));

                // **************************************************************************************
                // 2. Build function call expressions.
                //    Output: func_call_exps;
                // **************************************************************************************
                vector<SgExpression*> func_call_exps;
                buildFunctionCallExpressions(basic_exps, func_call_exps);
                buildFunctionCallExpressions(comp_exps, func_call_exps);

                stmt_groups.push_back(vector<SgStatement*>());
                foreach (SgExpression* exp, func_call_exps)
                    stmt_groups.back().push_back(buildBasicBlock(
                                copyStatement(local_var),
                                buildExprStatement(exp)));

                // **************************************************************************************
                // 3. Build if statements.
                //    Output: func_call_exps;
                // **************************************************************************************
                vector<SgStatement*> if_stmts;
#if 0
                // randomly choose branches
                for (int i = 0; i + 1 < comp_exps.size(); i += 2)
                    buildIfStatements(
                            buildRandomBoolValExp(), 
                            buildExprStatement(copyExpression(comp_exps[i])), 
                            buildExprStatement(copyExpression(comp_exps[i+1])), 
                            if_stmts);
#endif
#if 1
                for (size_t i = 0; i + 2 < comp_exps.size(); i += 3)
                    buildIfStatements(
                            copyExpression(comp_exps[i]),
                            buildExprStatement(copyExpression(comp_exps[i+1])), 
                            buildExprStatement(copyExpression(comp_exps[i+2])), 
                            if_stmts);
#endif

#if 1
                // nested if
                int if_stmts_size = if_stmts.size();
                for (size_t i = 0; i + 1 < if_stmts_size; i += 2)
                    buildIfStatements(
                            buildRandomBoolValExp(), 
                            copyStatement(if_stmts[i]), 
                            copyStatement(if_stmts[i+1]), 
                            if_stmts);
#endif

                stmt_groups.push_back(vector<SgStatement*>());
                foreach (SgStatement* stmt, if_stmts)
                    stmt_groups.back().push_back(buildBasicBlock(
                                copyStatement(local_var),
                                stmt));
                /////////////////////////////////////////////////////////////////////////////////////////
                // Tests have to perform:
                // 1. Short circuit:  (a || b) (a && b)
                /////////////////////////////////////////////////////////////////////////////////////////
                // Build the test code.
                popScopeStack();
                buildTestCode(stmt_groups);
}

void TestCodeBuilder::modifyMakefile(int test_num)
{
    string all_files;
    for (int i = 0; i < test_num; ++i)
        all_files += "test" + lexical_cast<string>(i) + ".C ";

    ifstream ifs((RC_DIR + "/Makefile.am").c_str());
    string text, s;
    while (getline(ifs, s))
    {
        if (istarts_with(s, "TEST_Objects"))
            s = "TEST_Objects=" + all_files;
        text += s + "\n";
    }

    ifs.close();
    ofstream ofs((RC_DIR + "/Makefile.am").c_str());
    ofs << text;
    ofs.close();
}

#if 0
int main()
{
    srand(time(0));

    vector<string> args;
    args.push_back("");
    //args.push_back("-rose:C99");

    for (int i = 0; i < 10; ++i)
    {
        string filename = RC_DIR + "/test/test" + lexical_cast<string>(i) + ".C";
        ofstream ofs(filename.c_str());
        //ofs << "#include \"rctypes.h\"\n#include <stdio.h>\nstruct model;\n";
        ofs << "#include <stdlib.h>\n";
        ofs << "struct model;\n";
        ofs.close();
        args.push_back(filename);
    }

    SgProject* project = frontend(args);
    TestCodeBuilder builder(project);
    builder.buildTestCode();
}
#endif
