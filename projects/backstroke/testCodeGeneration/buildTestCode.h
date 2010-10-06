#ifndef ROSE_BACKSTROKE_TESTCODEGENERATION_H
#define ROSE_BACKSTROKE_TESTCODEGENERATION_H

#include <rose.h>


const std::string RC_DIR = "/home/hou1/new_rose/ROSE2/ROSE/projects/backstroke/reverseComputation";
const std::string INT_MEM_NAME = "i_";
const std::string INT_ARRAY_MEM_NAME = "a_";

class TestCodeBuilder
{
    SgProject* project_;
    SgFunctionDeclaration* func_decl_;
    SgClassDeclaration* model_;
    std::string filename_;	// test code
    int counter_;
    std::vector<SgStatement*> stmts_;

    // variables names in the model
    std::string model_obj_name_;
    SgType* model_type_;
    SgClassDeclaration* model_decl_;
    SgInitializedName* model_obj_;

    //*****************************
    // Data members
    SgExpression* int_var_;
    SgExpression* int_array_var_;

    //*****************************

    //*****************************
    // Data member names
    std::string int_var_name_;
    std::string int_array_var_name_;
    //*****************************

    int array_size_;

public:

    TestCodeBuilder(SgProject* project)
        : project_(project),
        counter_(0),
        model_obj_name_("m"),
        model_obj_(NULL),
        int_var_name_(INT_MEM_NAME),
        int_array_var_name_(INT_ARRAY_MEM_NAME),
        array_size_(1000)
    {
#if 0
        // Build the model structure (struct model {};)
        model_decl_ = buildModelStruct();
        // Get the type of the model.
        model_type_ = model_decl_->get_type();
        model_obj_ = buildInitializedName(model_obj_name_, buildPointerType(model_type_));
        //model_obj_ = buildInitializedName(model_obj_name_, buildIntType());//buildPointerType(buildClassType));

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
    }

    SgClassDeclaration* buildModelStruct();
    
    SgFunctionDeclaration* buildEventDeclaration(int i = 0);

    SgFunctionDeclaration* buildEventFunction(SgStatement* stmt, int idx);

    SgFunctionDeclaration* buildEventFunction(const std::vector<SgStatement*> stmts, int idx);

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

    SgExpression* buildRandomBoolValExp();

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

    void buildAllUnaryExpressions(SgExpression* operand, std::vector<SgExpression*>& exps);

    void buildAllBinaryExpressions(SgExpression* lhs, SgExpression* rhs, std::vector<SgExpression*>& exps);

    void buildAllBasicExpressions(SgExpression* var1, SgExpression* var2, std::vector<SgExpression*>& exps);//SgExpression* var3 = NULL);

    void buildCompoundBasicExpressions(
            SgExpression* var1, 
            SgExpression* var2, 
            std::vector<SgExpression*>& exps);

    void buildConditionalExpressions(
            const std::vector<SgExpression*>& input, 
            std::vector<SgExpression*>& exps);

    void buildFunctionCallExpressions(
            const std::vector<SgExpression*>& input, 
            std::vector<SgExpression*>& exps);

#if 0
    vector<SgStatement*> buildConditionalStatements(const vector<SgExpression*> exps)
    {
        vector<SgStatement*> stmts;
        stmts.push_back(buildExprStatement(buildConditionalExp(exps[0], exps[1], exps[2])));
        stmts.push_back(buildExprStatement(buildConditionalExp(buildRandomBoolValExp(), exps[1], exps[2])));
        return stmts;
    }
#endif

    void buildIfStatements(
            SgExpression* cond, 
            SgStatement* true_body, 
            SgStatement* false_body,
            std::vector<SgStatement*>& if_stmts);

    // Build a for statement by giving a loop number and its body
    SgForStatement* buildForStatementWithLoopNum(SgExpression* num, SgStatement* body);

    void addStatements(const std::vector<SgStatement*>& stmts)
    {
        stmts_.insert(stmts_.end(), stmts.begin(), stmts.end());
    }

    void buildTestCode(const std::vector<std::vector<SgStatement*> >& stmt_groups);

    void buildTestCode();

    void modifyMakefile(int test_num);
};


#endif
