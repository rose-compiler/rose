#include "facilityBuilder.h"
#include "utilities/utilities.h"
#include <rose.h>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#define foreach BOOST_FOREACH

using namespace std;
using namespace boost;
using namespace SageBuilder;
using namespace SageInterface;

SgStatement* initializeMember(SgExpression* exp)
{
    SgType* exp_type = exp->get_type();
    SgType* real_type = exp_type->stripTypedefsAndModifiers();

    if (isSgArrayType(real_type))
    {
        // Initialize int array member
        SgExprListExp* memset_para = buildExprListExp(
                copyExpression(exp),
                buildIntVal(0xFFFF),
                buildSizeOfOp(exp));
        return buildFunctionCallStmt("memset", buildPointerType(buildVoidType()), memset_para);
    }
	else if (BackstrokeUtility::isSTLContainer(real_type))
	{
        return NULL;
	}
    else if (SgClassType* class_t = isSgClassType(real_type))
    {
        SgBasicBlock* block = buildBasicBlock();
        SgClassDeclaration* class_decl = isSgClassDeclaration(class_t->get_declaration()->get_definingDeclaration());
        SgDeclarationStatementPtrList members = class_decl->get_definition()->get_members();
        foreach (SgDeclarationStatement* decl, members)
        {
            if (SgVariableDeclaration* var_decl = isSgVariableDeclaration(decl))
            {
                SgVarRefExp* member_var = buildVarRefExp(var_decl->get_variables()[0]);
                SgExpression* var = buildBinaryExpression<SgDotExp>(exp, member_var);
                appendStatement(initializeMember(var), block);
            }
        }
        return block;
    }
    else
    {
        SgAssignOp* init = buildBinaryExpression<SgAssignOp>(exp, buildIntVal(0xFFFF));
        return buildExprStatement(init);
    }
}

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

    // Automatically initialize its members
    SgClassDeclaration* model_decl = isSgClassDeclaration(model_type->get_declaration()->get_definingDeclaration());
    ROSE_ASSERT(model_decl);
    ROSE_ASSERT(model_decl->get_definition());
    SgDeclarationStatementPtrList members = model_decl->get_definition()->get_members();
    foreach (SgDeclarationStatement* decl, members)
    {
        if (SgVariableDeclaration* var_decl = isSgVariableDeclaration(decl))
        {
            SgVarRefExp* member_var = buildVarRefExp(var_decl->get_variables()[0]);
            SgExpression* var = buildBinaryExpression<SgArrowExp>(
                        buildVarRefExp(model_obj), member_var);
            if (SgStatement* init = initializeMember(var))
                appendStatement(init);
        }
    }

#if 0
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
#endif

    popScopeStack();
    return func_decl;
}

SgStatement* compareValue(SgExpression* var1, SgExpression* var2)
{
    SgType* var_type = var1->get_type();
    if (isSgArrayType(var_type))
    {
        SgExprListExp* memcmp_para = buildExprListExp(
                var1, var2,
                buildSizeOfOp(var1));
        SgStatement* cmp = buildFunctionCallStmt("memcmp", buildPointerType(buildVoidType()), memcmp_para);
        return buildIfStmt(cmp, buildReturnStmt(buildIntVal(0)), NULL);
    }
    else
    {
        SgNotEqualOp* compare_exp = buildBinaryExpression<SgNotEqualOp>(var1, var2);
        return buildIfStmt(compare_exp, buildReturnStmt(buildIntVal(0)), NULL);
    }
    return NULL;
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

    // Automatically initialize its members
    SgClassDeclaration* model_decl = isSgClassDeclaration(model_type->get_declaration()->get_definingDeclaration());
    ROSE_ASSERT(model_decl);
    ROSE_ASSERT(model_decl->get_definition());
    SgDeclarationStatementPtrList members = model_decl->get_definition()->get_members();
    foreach (SgDeclarationStatement* decl, members)
    {
        if (SgVariableDeclaration* var_decl = isSgVariableDeclaration(decl))
        {
            SgVarRefExp* member_var = buildVarRefExp(var_decl->get_variables()[0]);
            SgExpression* var1 = buildBinaryExpression<SgArrowExp>(
                        buildVarRefExp(model_obj1), member_var);
            SgExpression* var2 = buildBinaryExpression<SgArrowExp>(
                        buildVarRefExp(model_obj2), member_var);
            if (SgStatement* comp = compareValue(var1, var2))
                appendStatement(comp);
        }
    }

    // return 1 if two models are equal
    appendStatement(buildReturnStmt(buildIntVal(1)));

    popScopeStack();
    return func_decl;
}


SgFunctionDeclaration* buildMainFunction(const vector<SgAssignOp*>& inits, const vector<string>& event_names, bool klee)
{
	// build the main function which performs the test
	SgFunctionDeclaration* func_decl =
			buildDefiningFunctionDeclaration(
			"main",
			buildIntType(),
			buildFunctionParameterList());
	pushScopeStack(isSgScopeStatement(func_decl->get_definition()->get_body()));

	// First, input all initializing statements.
	foreach(SgAssignOp* stmt, inits)
	{
		appendStatement(SageBuilder::buildExprStatement(stmt));
	}

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
	SgVariableDeclaration* varM1 = buildVariableDeclaration("m1", model_type);
	SgVariableDeclaration* varM2 = buildVariableDeclaration("m2", model_type);


	// Output PASS or FAIL information
	SgExprListExp* para_pass = buildExprListExp(buildStringVal("PASS!\\n"));
	SgExprStatement* test_pass = buildFunctionCallStmt("printf", buildVoidType(), para_pass);

	appendStatement(reset_seed);
	appendStatement(varM1);
	appendStatement(varM2);

	// We have to know which event has passed the test. So a counter is necessary
	SgVariableDeclaration* counter = buildVariableDeclaration("counter", buildIntType(), buildAssignInitializer(buildIntVal(0)));
	appendStatement(counter);


	foreach(const string& event, event_names)
	{
		// Initialize two model objects
		SgExprStatement *init1, *init2;

		if (klee)
		{
			SgExprListExp* para1 = buildExprListExp(
					buildUnaryExpression<SgAddressOfOp > (buildVarRefExp(varM1)),
					buildSizeOfOp(buildVarRefExp(varM1)),
					buildStringVal("m1"));
			init1 = buildFunctionCallStmt("klee_make_symbolic", buildVoidType(), para1);
			init2 = buildAssignStatement(buildVarRefExp(varM2), buildVarRefExp(varM1));
		}
		else
		{
			SgExprListExp* para1 = buildExprListExp(
					buildUnaryExpression<SgAddressOfOp > (buildVarRefExp(varM1)));
			init1 = buildFunctionCallStmt("initialize", buildVoidType(), para1);
			SgExprListExp* para2 = buildExprListExp(
					buildUnaryExpression<SgAddressOfOp > (buildVarRefExp(varM2)));
			init2 = buildFunctionCallStmt("initialize", buildVoidType(), para2);
		}

		// Call event_forward and event_reverse then check if the model's value changes
		SgStatement* call_event_fwd = buildFunctionCallStmt(
				event + "_forward",
				buildVoidType(),
				buildExprListExp(buildUnaryExpression<SgAddressOfOp > (buildVarRefExp(varM1))));
		SgStatement* call_event_rev = buildFunctionCallStmt(
				event + "_reverse",
				buildVoidType(),
				buildExprListExp(buildUnaryExpression<SgAddressOfOp > (buildVarRefExp(varM1))));

		SgExprListExp* para_comp = buildExprListExp(
				buildUnaryExpression<SgAddressOfOp > (buildVarRefExp(varM1)),
				buildUnaryExpression<SgAddressOfOp > (buildVarRefExp(varM2)));
		SgExpression* call_compare_exp = buildFunctionCallExp("compare", buildIntType(), para_comp);
		SgExprListExp* para_assert = buildExprListExp(call_compare_exp);
		SgStatement* test_compare = buildFunctionCallStmt("assert", buildVoidType(), para_assert);

		SgExprListExp* para_print = buildExprListExp(
				buildStringVal("event%d PASS!\\n"),
				buildPlusPlusOp(buildVarRefExp(counter), SgUnaryOp::postfix));
		SgExprStatement* print_pass = buildFunctionCallStmt("printf", buildVoidType(), para_print);

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
