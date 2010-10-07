#include "testCodeBuilder.h"
#include <boost/foreach.hpp>

using namespace std;
using namespace boost;
using namespace SageBuilder;
using namespace SageInterface;

#define foreach BOOST_FOREACH

void UnaryExpressionBuilder::build()
{
	results_.clear();

	SgType* type = operand_->get_type();

	if (isScalarType(type))
	{
		if (operand_->isDefinable())
		{
			results_.push_back(buildPlusPlusOp(copyExpression(operand), SgUnaryOp::prefix));
			results_.push_back(buildMinusMinusOp(copyExpression(operand), SgUnaryOp::prefix));
			results_.push_back(buildPlusPlusOp(copyExpression(operand), SgUnaryOp::postfix));
			results_.push_back(buildMinusMinusOp(copyExpression(operand), SgUnaryOp::postfix));
		}

		results_.push_back(buildUnaryExpression<SgNotOp>(copyExpression(operand)));
		results_.push_back(buildUnaryExpression<SgMinusOp>(copyExpression(operand)));
		results_.push_back(buildUnaryExpression<SgUnaryAddOp>(copyExpression(operand)));
		results_.push_back(buildUnaryExpression<SgBitComplementOp>(copyExpression(operand)));
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

SgFunctionDeclaration* EventFunctionBuilder::buildEventFunction(const std::string& event_name, const std::vector<SgStatement*>& stmts)
{
	SgType* model_type = model_decl_->get_type();
	//if (model_obj_ == NULL)
	//model_object_ = buildInitializedName(model_obj_name_, buildPointerType(model_type_));

	SgFunctionParameterList* para_list = buildFunctionParameterList(model_object_);
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


SgExpression* ModelBuilder::getMemberExpression(const std::string& name) const
{
	foreach (const MemberType& member, members_)
	{
		if (member->get<0> == name)
		{
			ROSE_ASSERT(member->get<2>());
			return member->get<2>();
		}
	}
	return NULL;
}

std::vector<SgExpression*> ModelBuilder::getMemberExpression(SgType* type) const
{
	std::vector<SgExpression*> exps;
	foreach (const MemberType& member, members_)
	{
		// It seems that every type has only one object in the memory pool in ROSE.
		if (member->get<1> == type)
		{
			ROSE_ASSERT(member->get<2>());
			exps.push_back(member->get<2>());
		}
	}
	return exps;
}

void ModelBuilder::build()
{
	model_decl_ = buildStructDeclaration(name_);
	SgClassDefinition* model_def = buildClassDefinition(model_decl_);
	SgInitializedName* model_init_name_ = buildInitializedName(name_, buildPointerType(model_decl_->get_type()));

	pushScopeStack(isSgScopeStatement(def));

	// Build declarations for all members.
	foreach (MemberType& member, members_)
	{
		SgVariableDeclaration* var_decl = buildVariableDeclaration(member.get<0>(), member.get<1>());
		model_def->append_member(var_decl);
		// Build an expression for each member like m->i.
		member.get<2> = buildBinaryExpression<SgArrowExp>(
				buildVarRefExp(model_decl_), buildVarRefExp(var_decl));
	}

	popScopeStack();
}