#include <rose.h>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;
using namespace SageBuilder;
using namespace SageInterface;


const string INT_MEM_NAME = "m_int";

// build the structure which is the model data
SgClassDeclaration* buildModelStruct()
{
    SgClassDeclaration* decl = buildStructDeclaration("model");
    SgClassDefinition* def = buildClassDefinition(decl);

    SgVariableDeclaration* int_var = buildVariableDeclaration(INT_MEM_NAME, buildIntType());
    def->append_member(int_var);
    return decl;
}

SgFunctionDeclaration* buildInitializationFunction(SgType* model_type)
{
    SgInitializedName* model_obj = buildInitializedName("m", buildPointerType(model_type));
    SgFunctionParameterList* para_list = buildFunctionParameterList(model_obj);
    SgFunctionDeclaration* func_decl = 
	buildDefiningFunctionDeclaration(
		"initialize",
		buildVoidType(),
		para_list);
    pushScopeStack(isSgScopeStatement(func_decl->get_definition()->get_body()));

    SgArrowExp* int_var = buildBinaryExpression<SgArrowExp>(
	    buildVarRefExp(model_obj),
	    buildVarRefExp(INT_MEM_NAME)); 
    SgAssignOp* assign_0_to_int = buildBinaryExpression<SgAssignOp>(int_var, buildIntVal(0));
    SgStatement* stmt = buildExprStatement(assign_0_to_int);
    appendStatement(stmt);

    popScopeStack();
    return func_decl;
}

SgFunctionDeclaration* buildCompareFunction(SgType* model_type)
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
    SgIfStmt* if_compare_int_stmt = buildIfStmt(compare_int_exp, buildReturnStmt(buildIntVal(1)), NULL);
    appendStatement(if_compare_int_stmt);

    // return 0 if two models are equal
    appendStatement(buildReturnStmt(buildIntVal(0)));

    popScopeStack();
    return func_decl;
}

SgFunctionDeclaration* buildEventFunction(SgType* model_type)
{
    SgInitializedName* model_obj = buildInitializedName("m", buildPointerType(model_type));
    SgFunctionParameterList* para_list = buildFunctionParameterList(model_obj);
    SgFunctionDeclaration* func_decl = 
	buildDefiningFunctionDeclaration(
		"event",
		buildVoidType(),
		para_list);
    pushScopeStack(isSgScopeStatement(func_decl->get_definition()->get_body()));

    /////////////////////////////////////////////////////////////////////////////////////////
    // Start to add operations into the event function
    SgArrowExp* int_var = buildBinaryExpression<SgArrowExp>(
	    buildVarRefExp(model_obj),
	    buildVarRefExp(INT_MEM_NAME)); 
    SgUnaryOp::Sgop_mode mm_mode = SgUnaryOp::prefix;
    SgExpression* plusplus_exp = buildPlusPlusOp(int_var, mm_mode);
    SgStatement* stmt = buildExprStatement(plusplus_exp);
    appendStatement(stmt);

    /////////////////////////////////////////////////////////////////////////////////////////

    popScopeStack();
    return func_decl;
}

SgFunctionDeclaration* buildMainFunction(SgType* model_type)
{
    SgFunctionDeclaration* func_decl = 
	buildDefiningFunctionDeclaration(
		"main",
		buildIntType(),
		buildFunctionParameterList());
    pushScopeStack(isSgScopeStatement(func_decl->get_definition()->get_body()));

    /////////////////////////////////////////////////////////////////////////////////////////
    // There are two tests: one is testing event and event_fwd get the same value of the model,
    // the other is performing event_fwd and event_reverse to see if the value of the model changes
    
    // Declare two variables 
    SgVariableDeclaration* var1 = buildVariableDeclaration("m1", model_type);
    SgVariableDeclaration* var2 = buildVariableDeclaration("m2", model_type);

    // Initialize them
    SgExprListExp* para1 = buildExprListExp(
	    buildUnaryExpression<SgAddressOfOp>(buildVarRefExp("m1")));
    SgExprStatement* init1 = buildFunctionCallStmt("initialize", buildVoidType(), para1);
    SgExprListExp* para2 = buildExprListExp(
	    buildUnaryExpression<SgAddressOfOp>(buildVarRefExp("m2")));
    SgExprStatement* init2 = buildFunctionCallStmt("initialize", buildVoidType(), para2);

    // Call event and event_forward then check two models' values
    SgExprStatement* call_event = buildFunctionCallStmt("event", buildVoidType(), para1);
    SgExprStatement* call_event_fwd = buildFunctionCallStmt("event_forward", buildVoidType(), para2);
    SgExprListExp* para_comp = buildExprListExp(
	    buildUnaryExpression<SgAddressOfOp>(buildVarRefExp("m1")),
	    buildUnaryExpression<SgAddressOfOp>(buildVarRefExp("m2")));
    SgExprStatement* call_compare = buildFunctionCallStmt("compare", buildVoidType(), para_comp);
    SgIfStmt* test_compare = buildIfStmt(call_compare, buildReturnStmt(buildIntVal(1)), NULL);

    // Call event_forward and event_reverse then check if the model's value changes
    SgExprStatement* call_event_rev = buildFunctionCallStmt("event_reverse", buildVoidType(), para2);

    appendStatement(var1);
    appendStatement(var2);
    appendStatement(init1);
    appendStatement(init2);
    appendStatement(call_event);
    appendStatement(call_event_fwd);
    appendStatement(test_compare);
    appendStatement(copyStatement(init1));
    appendStatement(copyStatement(init2));
    appendStatement(copyStatement(call_event_fwd));
    appendStatement(call_event_rev);
    appendStatement(copyStatement(test_compare));

    /////////////////////////////////////////////////////////////////////////////////////////

    popScopeStack();
    return func_decl;
}


int main(int argc, char* argv[])
{
    int counter = 0;
    int num = 1;

    for (int i = 0; i < num; ++i)
    {
	string filename = "test" + lexical_cast<string>(i) + ".c";
	ofstream ofs(filename.c_str());
	ofs << "#include \"rctypes.h\"\n;struct model;\n";
	ofs.close();

	vector<string> args;
	args.push_back("");
	args.push_back(filename);
	SgProject* project = frontend(args);
	SgGlobal* global_scope = getFirstGlobalScope(project);
	pushScopeStack(isSgScopeStatement(global_scope));

	SgClassDeclaration* model = buildModelStruct();
	SgFunctionDeclaration* init = buildInitializationFunction(model->get_type());
	SgFunctionDeclaration* comp = buildCompareFunction(model->get_type());
	SgFunctionDeclaration* event = buildEventFunction(model->get_type());
	SgFunctionDeclaration* main_func = buildMainFunction(model->get_type());

	appendStatement(model);
	appendStatement(init);
	appendStatement(comp);
	appendStatement(event);
	appendStatement(main_func);

	popScopeStack();

	backend(project);
    }
}
