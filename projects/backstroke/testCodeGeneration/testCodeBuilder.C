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
		std::vector<SgExpression*> res = builder->getGeneratedExpressions();
		results_.insert(results_.end(), res.begin(), res.end());
	}
}

void UnaryExpressionBuilder::build()
{
	results_.clear();

	SgType* type = operand_->get_type();

	if (isScalarType(type))
	{
		if (operand_->isDefinable())
		{
			results_.push_back(buildPlusPlusOp(copyExpression(operand_), SgUnaryOp::prefix));
			results_.push_back(buildMinusMinusOp(copyExpression(operand_), SgUnaryOp::prefix));
			results_.push_back(buildPlusPlusOp(copyExpression(operand_), SgUnaryOp::postfix));
			results_.push_back(buildMinusMinusOp(copyExpression(operand_), SgUnaryOp::postfix));
		}

		results_.push_back(buildUnaryExpression<SgNotOp>(copyExpression(operand_)));
		results_.push_back(buildUnaryExpression<SgMinusOp>(copyExpression(operand_)));
		results_.push_back(buildUnaryExpression<SgUnaryAddOp>(copyExpression(operand_)));
		results_.push_back(buildUnaryExpression<SgBitComplementOp>(copyExpression(operand_)));
	}
}

void BinaryExpressionBuilder::build()
{
	if (lhs_operand_->isDefinable())
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

	results_.push_back(buildBinaryExpression<SgModAssignOp>(copyExpression(lhs_operand_), copyExpression(rhs_operand_)));
	results_.push_back(buildBinaryExpression<SgIorAssignOp>(copyExpression(lhs_operand_), copyExpression(rhs_operand_)));
	results_.push_back(buildBinaryExpression<SgAndAssignOp>(copyExpression(lhs_operand_), copyExpression(rhs_operand_)));
	results_.push_back(buildBinaryExpression<SgXorAssignOp>(copyExpression(lhs_operand_), copyExpression(rhs_operand_)));
	results_.push_back(buildBinaryExpression<SgLshiftAssignOp>(copyExpression(lhs_operand_), copyExpression(rhs_operand_)));
	results_.push_back(buildBinaryExpression<SgRshiftAssignOp>(copyExpression(lhs_operand_), copyExpression(rhs_operand_)));
}

void ExpressionStatementBuilder::build()
{
	results_.push_back(buildExprStatement(exp_));
}

SgFunctionDeclaration* EventFunctionBuilder::build()
{
	// Build the parameter list.
	SgFunctionParameterList* para_list = buildFunctionParameterList();
	foreach (SgInitializedName* para, parameters_)
		para_list->append_arg(para);
	
	SgFunctionDeclaration* func_decl = buildDefiningFunctionDeclaration(event_name_, return_type_, para_list);

	SgBasicBlock* event_body = func_decl->get_definition()->get_body();
	ROSE_ASSERT(event_body);
	replaceStatement(event_body, event_body_);

	return func_decl;
}

#if 0
SgFunctionDeclaration* EventFunctionBuilder::buildEventFunction(const std::string& event_name, const std::vector<SgStatement*>& stmts)
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


SgExpression* StateClassBuilder::getMemberExpression(const std::string& name) const
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

std::vector<SgExpression*> StateClassBuilder::getMemberExpression(SgType* type) const
{
	std::vector<SgExpression*> exps;
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
	SgSourceFile* source_file_ = isSgSourceFile((*project)[0]);
	ROSE_ASSERT(source_file_);

	// Build the concrete test code here.
	build_();

	backend(project);
}

void BasicExpressionTest::build_()
{
	// First, we build a state structure.
	StateClassBuilder state_builder("Model");
	state_builder.addMember("i", buildIntType());
	state_builder.addMember("f", buildFloatType());
	state_builder.build();

	// Build the state object first, which will be added into event function as a parameter later.
	SgInitializedName* state_init_name = buildInitializedName("m", state_builder.getStateClassType());
	SgExpression* state_obj = buildVarRefExp(state_init_name);

	SgExpression* int_var = buildBinaryExpression<SgArrowExp>(
			buildVarRefExp(state_init_name), state_builder.getMemberExpression("i"));
	SgExpression* float_var = buildBinaryExpression<SgArrowExp>(
			buildVarRefExp(state_init_name), state_builder.getMemberExpression("f"));


	// A expression builder pool uses its child builders to build expressions.
	ExpressionBuilderPool builders;
	builders.addExpressionBuilder(new UnaryExpressionBuilder(int_var));
	builders.addExpressionBuilder(new UnaryExpressionBuilder(float_var));
	builders.addExpressionBuilder(new BinaryExpressionBuilder(int_var, float_var));
	builders.addExpressionBuilder(new BinaryExpressionBuilder(float_var, int_var));
	builders.build();

	int counter = 0;
	std::vector<SgExpression*> exps = builders.getGeneratedExpressions();
	foreach (SgExpression* exp, exps)
	{
		SgExprStatement* stmt = buildExprStatement(exp);
		SgBasicBlock* body = buildBasicBlock(stmt);

		string event_name = "event" + lexical_cast<string>(counter++);
		EventFunctionBuilder event_builder(event_name, body);
		event_builder.addParameter(state_builder.getStateClassType(), state_builder.getStateObjectName());
		SgFunctionDeclaration* event_decl = event_builder.build();
		// Add the new defined event function in the event collection.
		events_.push_back(event_decl);
	}

	// Once we get the declarations of the state class and event functions, we can generate a C++ source file.

	//SgSourceFile* src_file = isSgSourceFile((*project_)[i]);
	//src_file->set_unparse_output_filename(src_file->getFileName());

	SgGlobal* global_scope = source_file_->get_globalScope();
	pushScopeStack(isSgScopeStatement(global_scope));

	// Add state declaration.
	appendStatement(state_builder.getStateClassDeclaration());
	// Add events.
	foreach (SgFunctionDeclaration* event, events_)
		appendStatement(event);

	// Fix variable references here because of bottom up build.
	fixVariableReferences(global_scope);

	popScopeStack();
}