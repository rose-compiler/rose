#include "facilityBuilder.h"
#include <rose.h>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#define foreach BOOST_FOREACH

using namespace std;
using namespace boost;
using namespace SageBuilder;
using namespace SageInterface;

SgFunctionDeclaration* buildInitializationFunction(SgClassType* model_type)
{
    SgInitializedName* model_obj = buildInitializedName("m", buildPointerType(model_type));
    SgFunctionParameterList* para_list = buildFunctionParameterList(model_obj);
    SgFunctionDeclaration* func_decl = 
        buildDefiningFunctionDeclaration(
                "initialize",
                buildVoidType(),
                para_list);

    pushScopeStack(isSgScopeStatement(func_decl->get_definition()->get_body()));

    // Initialize int member
    SgArrowExp* int_var = buildBinaryExpression<SgArrowExp>(
            buildVarRefExp(model_obj),
            buildVarRefExp(INT_MEM_NAME)); 
    SgAssignOp* init_int = buildBinaryExpression<SgAssignOp>(int_var, buildIntVal(0xFFFF));
    appendStatement(buildExprStatement(init_int));

    // Initialize int array member
    SgArrowExp* int_array_var = buildBinaryExpression<SgArrowExp>(
            buildVarRefExp(model_obj),
            buildVarRefExp(INT_ARRAY_MEM_NAME)); 
    SgExpression* size_exp = buildBinaryExpression<SgDivideOp>(
                buildSizeOfOp(int_array_var),
                buildSizeOfOp(buildIntType()));
    SgExprListExp* memset_para = buildExprListExp(
            copyExpression(int_array_var),
            buildIntVal(0xFFFF),
            size_exp);
    appendStatement(buildFunctionCallStmt("memset", buildPointerType(buildVoidType()), memset_para));

    popScopeStack();
    return func_decl;
}

SgFunctionDeclaration* buildCompareFunction(SgClassType* model_type)
{
    SgInitializedName* model_obj1 = buildInitializedName("m1", buildPointerType(model_type));
    SgInitializedName* model_obj2 = buildInitializedName("m2", buildPointerType(model_type));
    SgFunctionParameterList* para_list = buildFunctionParameterList(model_obj1, model_obj2);
    SgFunctionDeclaration* func_decl = 
        buildDefiningFunctionDeclaration(
                "compare",
                buildIntType(),
                para_list);
    pushScopeStack(isSgScopeStatement(func_decl->get_definition()->get_body()));

    SgArrowExp* int_var1 = buildBinaryExpression<SgArrowExp>(
            buildVarRefExp(model_obj1),
            buildVarRefExp(INT_MEM_NAME)); 
    SgArrowExp* int_var2 = buildBinaryExpression<SgArrowExp>(
            buildVarRefExp(model_obj2),
            buildVarRefExp(INT_MEM_NAME)); 
    SgNotEqualOp* compare_int_exp = buildBinaryExpression<SgNotEqualOp>(int_var1, int_var2);
    SgIfStmt* if_compare_int_stmt = buildIfStmt(compare_int_exp, buildReturnStmt(buildIntVal(0)), NULL);
    appendStatement(if_compare_int_stmt);

    // return 1 if two models are equal
    appendStatement(buildReturnStmt(buildIntVal(1)));

    popScopeStack();
    return func_decl;
}

SgFunctionDeclaration* buildMainFunction(const vector<SgStatement*>& inits, int events_num)
{
    // build the main function which performs the test
    SgFunctionDeclaration* func_decl = 
        buildDefiningFunctionDeclaration(
                "main",
                buildIntType(),
                buildFunctionParameterList());
    pushScopeStack(isSgScopeStatement(func_decl->get_definition()->get_body()));

    // First, input all initializing statements.
    foreach (SgStatement* stmt, inits) 
        appendStatement(stmt);

    /////////////////////////////////////////////////////////////////////////////////////////
    // There are two tests: one is testing event and event_fwd get the same value of the model,
    // the other is performing event_fwd and event_reverse to see if the value of the model changes

    // reset the seed of the random number generator
    SgExprListExp* get_clock = buildExprListExp(
            buildFunctionCallExp("time", buildIntType(), 
                buildExprListExp(buildIntVal(0))));
    SgStatement* reset_seed = buildFunctionCallStmt("srand", buildVoidType(), get_clock);
    SgType* model_type = buildStructDeclaration("model")->get_type();

    // Declare two variables 
    SgVariableDeclaration* var1 = buildVariableDeclaration("m1", model_type);
    SgVariableDeclaration* var2 = buildVariableDeclaration("m2", model_type);


    // Output PASS or FAIL information
    SgExprListExp* para_pass = buildExprListExp(buildStringVal("PASS!\\n"));
    //SgExprListExp* para_fail = buildExprListExp(buildStringVal("FAIL!\\n"));
    SgExprStatement* test_pass = buildFunctionCallStmt("printf", buildVoidType(), para_pass);
    //SgExprStatement* test_fail = buildFunctionCallStmt("printf", buildVoidType(), para_fail);

#if 0
    // Call event and event_forward then check two models' values
    SgExprStatement* call_event = buildFunctionCallStmt("event", buildVoidType(), para1);
    SgExprStatement* call_event_fwd = buildFunctionCallStmt("event_forward", buildVoidType(), para2);
    SgExprListExp* para_comp = buildExprListExp(
            buildUnaryExpression<SgAddressOfOp>(buildVarRefExp("m1")),
            buildUnaryExpression<SgAddressOfOp>(buildVarRefExp("m2")));
    SgExpression* call_compare_exp = buildFunctionCallExp("compare", buildVoidType(), para_comp);
    SgExprListExp* para_assert = buildExprListExp(call_compare_exp);
    SgStatement* test_compare = buildFunctionCallStmt("assert", buildVoidType(), para_assert);
#endif

    appendStatement(reset_seed);
    appendStatement(var1);
    appendStatement(var2);
    //appendStatement(init1);
    //appendStatement(init2);
    //appendStatement(call_event);
    //appendStatement(call_event_fwd);
    // comment the following code is because random thing may happen
    //appendStatement(test_compare);
    //appendStatement(copyStatement(init1));
    //appendStatement(copyStatement(init2));
    //appendStatement(copyStatement(call_event_fwd));
    //appendStatement(call_event_rev);
    //appendStatement(copyStatement(test_compare));
    //appendStatement(test_pass);


    // Build a counter which can output which event function crashes
    //SgStatement* counter = buildVariableDeclaration("counter", buildIntType(), 
    //	    buildAssignInitializer(buildIntVal(0)));

    // We have to know which event has passed the test. So a counter is necessary
    SgStatement* counter = buildVariableDeclaration("counter", buildIntType(), buildAssignInitializer(buildIntVal(0)));
    appendStatement(counter);

    for (int i = 0; i < events_num; ++i)
    {
        // Initialize two model objects
        SgExprListExp* para1 = buildExprListExp(
                buildUnaryExpression<SgAddressOfOp>(buildVarRefExp("m1")));
        SgExprStatement* init1 = buildFunctionCallStmt("initialize", buildVoidType(), para1);
        SgExprListExp* para2 = buildExprListExp(
                buildUnaryExpression<SgAddressOfOp>(buildVarRefExp("m2")));
        SgExprStatement* init2 = buildFunctionCallStmt("initialize", buildVoidType(), para2);

        // Call event_forward and event_reverse then check if the model's value changes
        SgStatement* call_event_fwd = buildFunctionCallStmt(
                "event" + lexical_cast<string>(i) + "_forward", 
                buildVoidType(), 
                isSgExprListExp(copyExpression(para1)));
        SgStatement* call_event_rev = buildFunctionCallStmt(
                "event" + lexical_cast<string>(i) + "_reverse", 
                buildVoidType(), 
                isSgExprListExp(copyExpression(para1)));

        SgExprListExp* para_comp = buildExprListExp(
                buildUnaryExpression<SgAddressOfOp>(buildVarRefExp("m1")),
                buildUnaryExpression<SgAddressOfOp>(buildVarRefExp("m2")));
        SgExpression* call_compare_exp = buildFunctionCallExp("compare", buildIntType(), para_comp);
        SgExprListExp* para_assert = buildExprListExp(call_compare_exp);
        SgStatement* test_compare = buildFunctionCallStmt("assert", buildVoidType(), para_assert);

        SgExprListExp* para_print = buildExprListExp(
                buildStringVal("event%d PASS!\\n"), 
                buildPlusPlusOp(buildVarRefExp("counter"), SgUnaryOp::postfix));
        SgExprStatement* print_pass = buildFunctionCallStmt("printf", buildVoidType(), para_print);
        //SgStatement* incr_counter = buildExprStatement(
        //	buildPlusPlusOp(buildVarRefExp("counter", SgUnaryOp::prefix)));

        appendStatement(init1);
        appendStatement(init2);
        appendStatement(call_event_fwd);
        appendStatement(call_event_rev);
        appendStatement(test_compare);
        appendStatement(print_pass);
    }

    appendStatement(test_pass);
    /////////////////////////////////////////////////////////////////////////////////////////
    popScopeStack();
    return func_decl;
}
