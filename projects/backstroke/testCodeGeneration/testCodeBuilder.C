#include "testCodeBuilder.h"
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;
using namespace SageBuilder;
using namespace SageInterface;

#define foreach BOOST_FOREACH

void ExpressionBuilderPool::build()
{
	foreach (ExpressionBuilderPtr builder, exp_builders_)
	{
		builder->build();
		vector<SgExpression*> res = builder->getGeneratedExpressions();
		results_.insert(results_.end(), res.begin(), res.end());
	}
}

void UnaryExpressionBuilder::build()
{
	results_.clear();

	SgType* type = operand_->get_type();

	if (isScalarType(type))
	{
		if (operand_->isLValue())
		{
			results_.push_back(buildPlusPlusOp(copyExpression(operand_), SgUnaryOp::prefix));
			results_.push_back(buildMinusMinusOp(copyExpression(operand_), SgUnaryOp::prefix));
			results_.push_back(buildPlusPlusOp(copyExpression(operand_), SgUnaryOp::postfix));
			results_.push_back(buildMinusMinusOp(copyExpression(operand_), SgUnaryOp::postfix));
		}

		results_.push_back(buildUnaryExpression<SgNotOp>(copyExpression(operand_)));
		results_.push_back(buildUnaryExpression<SgMinusOp>(copyExpression(operand_)));
		results_.push_back(buildUnaryExpression<SgUnaryAddOp>(copyExpression(operand_)));
		if (isStrictIntegerType(type))
			results_.push_back(buildUnaryExpression<SgBitComplementOp>(copyExpression(operand_)));
	}
}

void BinaryExpressionBuilder::build()
{
	SgType* lhs_type = lhs_operand_->get_type();
	SgType* rhs_type = rhs_operand_->get_type();

	if (lhs_operand_->isLValue())
	{
		results_.push_back(buildBinaryExpression<SgAssignOp>(copyExpression(lhs_operand_), copyExpression(rhs_operand_)));
		results_.push_back(buildBinaryExpression<SgPlusAssignOp>(copyExpression(lhs_operand_), copyExpression(rhs_operand_)));
		results_.push_back(buildBinaryExpression<SgMinusAssignOp>(copyExpression(lhs_operand_), copyExpression(rhs_operand_)));
		results_.push_back(buildBinaryExpression<SgMultAssignOp>(copyExpression(lhs_operand_), copyExpression(rhs_operand_)));
	}

	// temporarily remove DivAssign to avoid exceptions
	//results_.push_back(buildBinaryExpression<SgDivAssignOp>(copyExpression(lhs_operand_), copyExpression(rhs_operand_)));

	//results_.push_back(buildBinaryExpression<SgAddOp>(copyExpression(lhs_operand_), copyExpression(rhs_operand_)));
	results_.push_back(buildBinaryExpression<SgAndOp>(copyExpression(lhs_operand_), copyExpression(rhs_operand_)));
	results_.push_back(buildBinaryExpression<SgOrOp>(copyExpression(lhs_operand_), copyExpression(rhs_operand_)));
	results_.push_back(buildBinaryExpression<SgCommaOpExp>(copyExpression(lhs_operand_), copyExpression(rhs_operand_)));
	results_.push_back(buildBinaryExpression<SgEqualityOp>(copyExpression(lhs_operand_), copyExpression(rhs_operand_)));

	if (isStrictIntegerType(lhs_type) && isStrictIntegerType(rhs_type))
	{
		results_.push_back(buildBinaryExpression<SgModAssignOp>(copyExpression(lhs_operand_), copyExpression(rhs_operand_)));
		results_.push_back(buildBinaryExpression<SgIorAssignOp>(copyExpression(lhs_operand_), copyExpression(rhs_operand_)));
		results_.push_back(buildBinaryExpression<SgAndAssignOp>(copyExpression(lhs_operand_), copyExpression(rhs_operand_)));
		results_.push_back(buildBinaryExpression<SgXorAssignOp>(copyExpression(lhs_operand_), copyExpression(rhs_operand_)));
		results_.push_back(buildBinaryExpression<SgLshiftAssignOp>(copyExpression(lhs_operand_), copyExpression(rhs_operand_)));
		results_.push_back(buildBinaryExpression<SgRshiftAssignOp>(copyExpression(lhs_operand_), copyExpression(rhs_operand_)));
	}
}

void ExpressionStatementBuilder::build()
{
	results_.push_back(buildExprStatement(exp_));
}

SgFunctionDeclaration* EventFunctionBuilder::buildEventFunction(bool is_cxx_style)
{
	// Build the parameter list.
	SgFunctionParameterList* para_list = buildFunctionParameterList();
	foreach (SgInitializedName* para, parameters_)
		para_list->append_arg(para);

	SgFunctionDeclaration* func_decl = NULL;
	if (is_cxx_style)
		func_decl = buildDefiningMemberFunctionDeclaration(event_name_, return_type_, para_list, scope_);
	else
		func_decl = buildDefiningFunctionDeclaration(event_name_, return_type_, para_list, scope_);

	SgBasicBlock* event_body = func_decl->get_definition()->get_body();
	ROSE_ASSERT(event_body);
	replaceStatement(event_body, event_body_);

	return func_decl;
}

#if 0
SgFunctionDeclaration* EventFunctionBuilder::buildEventFunction(const string& event_name, const vector<SgStatement*>& stmts)
{
	SgType* state_type = state_decl_->get_type();
	//if (state_obj_ == NULL)
	//state_object_ = buildInitializedName(state_obj_name_, buildPointerType(state_type_));

	SgFunctionParameterList* para_list = buildFunctionParameterList(state_object_);
	SgFunctionDeclaration* func_decl = buildDefiningFunctionDeclaration(event_name, buildVoidType(), para_list);

	SgBasicBlock* event_body = func_decl->get_definition()->get_body();
	ROSE_ASSERT(event_body);

	foreach (SgStatement* stmt, stmts)
	{
		appendStatement(event_body, stmt);
	}
	//buildBasicBlock());
	//fixVariableReferences(decl);
	return func_decl;
}
#endif


SgExpression* StateClassBuilder::getMemberExpression(const string& name) const
{
	foreach (const MemberType& member, members_)
	{
		if (member.get<0>() == name)
		{
			ROSE_ASSERT(member.get<2>());
			return member.get<2>();
		}
	}
	return NULL;
}

vector<SgExpression*> StateClassBuilder::getMemberExpression(SgType* type) const
{
	vector<SgExpression*> exps;
	foreach (const MemberType& member, members_)
	{
		// It seems that every type has only one object in the memory pool in ROSE.
		if (member.get<1>() == type)
		{
			ROSE_ASSERT(member.get<2>());
			exps.push_back(member.get<2>());
		}
	}
	return exps;
}

void StateClassBuilder::build()
{
	state_decl_ = buildStructDeclaration(name_);
	SgClassDefinition* state_def = buildClassDefinition(state_decl_);
	//SgInitializedName* state_init_name = buildInitializedName(name_, buildPointerType(state_decl_->get_type()));

	pushScopeStack(isSgScopeStatement(state_def));

	// Build declarations for all members.
	foreach (MemberType& member, members_)
	{
		SgVariableDeclaration* var_decl = buildVariableDeclaration(member.get<0>(), member.get<1>());
		state_def->append_member(var_decl);
		// Build an expression for each member like m->i.
		member.get<2>() = buildVarRefExp(var_decl);
		//member.get<2>() = buildBinaryExpression<SgArrowExp>(
		//		buildVarRefExp(state_init_name), buildVarRefExp(var_decl));
	}

	popScopeStack();
}

void TestCodeBuilder::buildStateClass()
{
	ROSE_ASSERT(state_builder_);
	state_builder_->build();

	// Build the initialized name of the state object parameter in event functions.
	if (state_init_name_)
		delete state_init_name_;
	state_init_name_ =
		buildInitializedName("state", buildPointerType(state_builder_->getStateClassType()));
}

SgExpression* TestCodeBuilder::buildStateMemberExpression(const string& name)
{
	ROSE_ASSERT(state_builder_);
	if (SgExpression* member_exp = state_builder_->getMemberExpression(name))
	{
		if (is_cxx_style_)
			return member_exp;
		else
			return buildBinaryExpression<SgArrowExp>(buildVarRefExp(state_init_name_), member_exp);
	}
	return NULL;
}

void TestCodeBuilder::buildTestCode(const vector<SgBasicBlock*> bodies)
{
	ROSE_ASSERT(state_builder_);
	
	// Push global scope here to make sure every event funciton is built with a valid scope.
	SgGlobal* global_scope = source_file_->get_globalScope();
	ROSE_ASSERT(global_scope);

	int counter = 0;
	foreach (SgBasicBlock* body, bodies)
	{
		string event_name = "event" + lexical_cast<string>(counter++);
		EventFunctionBuilder event_builder(event_name, body);
		event_builder.addParameter(state_init_name_);
		if (is_cxx_style_)
			event_builder.setScope(state_builder_->getStateClassDeclaration()->get_definition());
		else
			event_builder.setScope(global_scope);
		
		SgFunctionDeclaration* event_decl = event_builder.buildEventFunction(is_cxx_style_);
		// Add the new defined event function in the event collection.
		events_.push_back(event_decl);
	}

	// Add state declaration.
	SgClassDeclaration* state_decl = state_builder_->getStateClassDeclaration();
	appendStatement(state_decl, global_scope);

	// Add events.
	foreach (SgFunctionDeclaration* event, events_)
	{
		if (is_cxx_style_)
		{
			//SgMemberFunctionDeclaration* decl = isSgMemberFunctionDeclaration(event);
			//ROSE_ASSERT(decl);
			//decl->set_associatedClassDeclaration(state_decl);
			appendStatement(event, state_decl->get_definition());
			//state_decl->get_definition()->append_member(event);
			//event->set_parent(state_decl);
		}
		else
			appendStatement(event, global_scope);
	}

	// Fix variable references here because of bottom up build.
	fixVariableReferences(global_scope);
}

void TestCodeBuilder::build()
{
	// We give the first arg empty since it is supposed to be the name of the executed program.
	vector<string> args(1, "");

	// Then we create a new empty file which will be our output.
	FILE* file = ::fopen(file_name_.c_str(), "w");
	::fclose(file);

	args.push_back(file_name_);

	SgProject* project = frontend(args);

	// Since we have only one file as the input, the first file is what we want.
	source_file_ = isSgSourceFile((*project)[0]);
	ROSE_ASSERT(source_file_);
	source_file_->set_unparse_output_filename(file_name_);

	// Build the concrete test code here.
	build_();

	AstTests::runAllTests(project);
	backend(project);
}

void BasicExpressionTest::build_()
{
	// First build the state class.
	setStateClassName("Model");
	addStateMember("i", buildIntType());
	addStateMember("f", buildFloatType());
	buildStateClass();

	SgExpression* int_var = buildStateMemberExpression("i");
	SgExpression* float_var = buildStateMemberExpression("f");


	// A expression builder pool uses its child builders to build expressions.
	ExpressionBuilderPool builders;
	builders.addExpressionBuilder(new UnaryExpressionBuilder(int_var));
	builders.addExpressionBuilder(new UnaryExpressionBuilder(float_var));
	builders.addExpressionBuilder(new BinaryExpressionBuilder(int_var, float_var));
	builders.addExpressionBuilder(new BinaryExpressionBuilder(float_var, int_var));
	builders.build();

	vector<SgExpression*> exps = builders.getGeneratedExpressions();
	vector<SgBasicBlock*> bodies;
	foreach (SgExpression* exp, exps)
	{
		SgExprStatement* stmt = buildExprStatement(exp);
		SgBasicBlock* body = buildBasicBlock(stmt);
		bodies.push_back(body);
	}

	// Finally, build the test code.
	buildTestCode(bodies);
}